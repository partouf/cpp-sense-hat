#include <pthread.h>
#include <errno.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "sense-hat-linux.h"

#include <RTIMULib.h>

static pthread_mutex_t sense_hat_mutex = PTHREAD_MUTEX_INITIALIZER;
static const char SENSE_HAT_FB_NAME[] = "RPi-Sense FB";

SenseHAT::ISenseHAT *HATInstance = nullptr;

SenseHAT::SenseHATLinux::SenseHATLinux()
	: SenseHAT::ISenseHAT(), fbfd(-1)
{
   settings = new RTIMUSettings();

   imu = RTIMU::createIMU(settings);
   if (imu)
   {
      imu->IMUInit();
   }

	humidity = RTHumidity::createHumidity(settings);
   if (humidity)
   {
      humidity->humidityInit();
   }

   pressure = RTPressure::createPressure(settings);
   if (pressure)
   {
      pressure->pressureInit();
   }
}

static uint16_t pack_pixel(uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t r16 = (r >> 3) & 0x1F;
	uint16_t g16 = (g >> 2) & 0x3F;
	uint16_t b16 = (b >> 3) & 0x1F;
	return (r16 << 11) + (g16 << 5) + b16;
}

static void sense_hat_rstrip(char* s)
{
	size_t l = strlen(s);
	if(l == 0) return;
	char* end = s + l - 1;
	while(end >= s && isspace(*end)) {
		end--;
	}
	end[1] = '\0';
}

int SenseHAT::SenseHATLinux::init_fb()
{
	if(fbfd != -1) {
		return 0;
	}
	DIR* d = opendir("/sys/class/graphics");
	if(d == NULL) {
		return errno;
	}
	struct dirent* dent = (struct dirent*)malloc(offsetof(struct dirent, d_name) + NAME_MAX + 1);
	struct dirent* dentp;
	while(1) {
		int rc = readdir_r(d, dent, &dentp);
		if(rc == 0 && dentp == NULL) {
			rc = ENOENT;
		}
		if(rc != 0) {
			free(dent);
			closedir(d);
			return rc;
		}
		if(strncmp("fb", dent->d_name, 2) == 0) {
			char path[PATH_MAX];
			snprintf(path, PATH_MAX, "/sys/class/graphics/%s/name", dent->d_name);
			FILE* f = fopen(path, "r");
			if(f == NULL) continue;
			char name[1024];
			fgets(name, sizeof(name), f);
			sense_hat_rstrip(name);
			if(strcmp(name, SENSE_HAT_FB_NAME) == 0) {
				snprintf(path, PATH_MAX, "/dev/%s", dent->d_name);
				fbfd = open(path, O_RDWR);
				free(dent);
				closedir(d);
				if(fbfd == -1) {
					return errno;
				}
				break;
			}
		}
	}
	return 0;
}

int SenseHAT::SenseHATLinux::blank()
{
	int rc = init_fb();
	if(rc != 0) {
		return rc;
	}
	char buf[128];
	memset(buf, 0, 128);
	if(pwrite(fbfd, buf, 128, 0) != 128) {
		return errno;
	}
	return 0;
}

int SenseHAT::SenseHATLinux::set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
	int rc = init_fb();
	if(rc != 0) {
		return rc;
	}
	uint16_t p = pack_pixel(r, g, b);
	if(pwrite(fbfd, &p, 2, (x + y*8)*2) != 2) {
		return errno;
	}
	return 0;
}

double SenseHAT::SenseHATLinux::get_humidity()
{
   if (!humidity)
   {
      return nan("");
   }

   RTIMU_DATA data;

	if(!humidity->humidityRead(data)) {
		return nan("");
	}
	if(!data.humidityValid) {
		return nan("");
	}
	return data.humidity;
}

double SenseHAT::SenseHATLinux::get_pressure()
{
   if (!pressure)
   {
      return nan("");
   }

   RTIMU_DATA data;
   if (!pressure->pressureRead(data)) {
      return nan("");
   }
   if (!data.pressureValid) {
      return nan("");
   }
   return data.pressure;
}

double SenseHAT::SenseHATLinux::get_temperature()
{
   return 0.0;
}

SenseHAT::d3 SenseHAT::SenseHATLinux::get_gyro()
{
   d3 data = { nan(""), nan(""), nan(""), false };

   if (!imu)
   {
      return data;
   }

   imu->setGyroEnable(true);
   imu->setAccelEnable(false);
   imu->setCompassEnable(false);

   if (imu->IMURead())
   {
      RTIMU_DATA imudata;
      imudata = imu->getIMUData();

      if (imudata.gyroValid)
      {
         data.x = imudata.gyro.x();
         data.y = imudata.gyro.y();
         data.z = imudata.gyro.z();
         data.valid = true;
      }
   }

   return data;
}

SenseHAT::d3 SenseHAT::SenseHATLinux::get_accel()
{
   d3 data = { nan(""), nan(""), nan(""), false };

   if (!imu)
   {
      return data;
   }

   imu->setGyroEnable(false);
   imu->setAccelEnable(true);
   imu->setCompassEnable(false);

   if (imu->IMURead())
   {
      RTIMU_DATA imudata;
      imudata = imu->getIMUData();

      if (imudata.accelValid)
      {
         data.x = imudata.accel.x();
         data.y = imudata.accel.y();
         data.z = imudata.accel.z();
         data.valid = true;
      }
   }

   return data;
}

SenseHAT::d3 SenseHAT::SenseHATLinux::get_magno()
{
   d3 data = { nan(""), nan(""), nan(""), false };

   if (!imu)
   {
      return data;
   }

   imu->setGyroEnable(false);
   imu->setAccelEnable(false);
   imu->setCompassEnable(true);

   if (imu->IMURead())
   {
      RTIMU_DATA imudata;
      imudata = imu->getIMUData();

      if (imudata.compassValid)
      {
         data.x = imudata.compass.x();
         data.y = imudata.compass.y();
         data.z = imudata.compass.z();
         data.valid = true;
      }
   }

   return data;
}

SenseHAT::ISenseHAT * SenseHAT::SenseHATLinux::Instance()
{
   if (HATInstance == nullptr)
   {
      HATInstance = new SenseHAT::SenseHATLinux();
   }

   return HATInstance;
}
