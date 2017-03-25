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
#include <memory>

#include <RTIMULib.h>


static const char SENSE_HAT_FB_NAME[] = "RPi-Sense FB";

static void sense_hat_rstrip(char* s)
{
   size_t l = strlen(s);
   if (l == 0) return;
   char* end = s + l - 1;
   while (end >= s && isspace(*end)) {
      end--;
   }
   end[1] = '\0';
}

static uint16_t pack_pixel(uint8_t r, uint8_t g, uint8_t b)
{
   uint16_t r16 = (r >> 3) & 0x1F;
   uint16_t g16 = (g >> 2) & 0x3F;
   uint16_t b16 = (b >> 3) & 0x1F;
   return static_cast<uint16_t>((r16 << 11) + (g16 << 5) + b16);
}

#include <functional>
class Finally
{
public:
   Finally(std::function<void(void)> callback) : callback_(callback)
   {
   }
   ~Finally()
   {
      callback_();
   }
   std::function<void(void)> callback_;
};


SenseHAT::ISenseHAT *HATInstance = nullptr;

SenseHAT::SenseHATLinux::SenseHATLinux() : SenseHAT::ISenseHAT()
{
   this->Matrix = std::make_unique<SenseHAT::SenseHATLEDMatrixLinux>();

   settings = new RTIMUSettings();

   imu = RTIMU::createIMU(settings);
   if (imu)
   {
      imu->IMUInit();
   }
   else
   {
      throw new SenseHATException("RTIMU could not be initialized");
   }

   humidity = RTHumidity::createHumidity(settings);
   if (humidity)
   {
      humidity->humidityInit();
   }
   else
   {
      throw new SenseHATException("RTHumidity could not be initialized");
   }

   pressure = RTPressure::createPressure(settings);
   if (pressure)
   {
      pressure->pressureInit();
   }
   else
   {
      throw new SenseHATException("RTPressure could not be initialized");
   }
}

SenseHAT::SenseHATLinux::~SenseHATLinux()
{
   delete imu;
   delete humidity;
   delete pressure;
   delete settings;
}

void SenseHAT::SenseHATLEDMatrixLinux::init_fb()
{
   if (this->fbfd != -1) {
      return;
   }

   DIR* d = opendir("/sys/class/graphics");
   if (d == NULL) {
      throw new SenseHATErrnoException(errno);
   }
   auto delete_dir = Finally([d] { closedir(d); });

   struct dirent* dent = (struct dirent*)malloc(offsetof(struct dirent, d_name) + NAME_MAX + 1);
   struct dirent* dentp;

   auto delete_dent = Finally([dent] { free(dent); });

   while (true) {
      int rc = readdir_r(d, dent, &dentp);
      if (rc == 0 && dentp == NULL) {
         rc = ENOENT;
      }

      if (rc != 0) {
         throw new SenseHATErrnoException(rc);
      }

      if (strncmp("fb", dent->d_name, 2) == 0) {
         char path[PATH_MAX];
         snprintf(path, PATH_MAX, "/sys/class/graphics/%s/name", dent->d_name);

         FILE* f = fopen(path, "r");
         if (f == NULL) continue;
         auto delete_f = Finally([f] { fclose(f); });

         char name[1024];
         fgets(name, sizeof(name), f);

         sense_hat_rstrip(name);
         if (strcmp(name, SENSE_HAT_FB_NAME) == 0) {
            snprintf(path, PATH_MAX, "/dev/%s", dent->d_name);
            this->fbfd = open(path, O_RDWR);

            if (this->fbfd == -1) {
               throw new SenseHATErrnoException(errno);
            }
            break;
         }
      }
   }
}

SenseHAT::SenseHATLEDMatrixLinux::SenseHATLEDMatrixLinux() : ISenseHATLEDMatrix()
{
   this->fbfd = -1;

   init_fb();
}

SenseHAT::SenseHATLEDMatrixLinux::~SenseHATLEDMatrixLinux()
{
   if (fbfd != -1)
   {
      close(fbfd);
   }
}

void SenseHAT::SenseHATLEDMatrixLinux::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
   init_fb();

   uint16_t p = pack_pixel(r, g, b);
   if (pwrite(fbfd, &p, 2, (x + y * 8) * 2) != 2) {
      throw new SenseHATErrnoException(errno);
   }
}

void SenseHAT::SenseHATLEDMatrixLinux::SetFromRgbaMatrix(const SenseHATColor_t matrix[8][8])
{
   for (int y = 0; y < 8; ++y)
   {
      for (int x = 0; x < 8; ++x)
      {
         SenseHATColor_rgba color = matrix[y][x];

         SetPixel(x, y, color.r, color.g, color.b);
      }
   }
}

void SenseHAT::SenseHATLEDMatrixLinux::Clear()
{
   init_fb();

   char buf[128];
   memset(buf, 0, 128);
   if (pwrite(this->fbfd, buf, 128, 0) != 128) {
      throw new SenseHATErrnoException(errno);
   }
}

double SenseHAT::SenseHATLinux::get_humidity()
{
   if (!humidity)
   {
      return nan("");
   }

   RTIMU_DATA data;

   if (!humidity->humidityRead(data)) {
      return nan("");
   }
   if (!data.humidityValid) {
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

SenseHAT::d3 SenseHAT::SenseHATLinux::get_temperature()
{
   d3 d3data = { nan(""), nan(""), nan(""), false };

   bool humok = false;
   bool pressok = false;

   RTIMU_DATA data;
   if (humidity->humidityRead(data)) {
      if (data.temperatureValid) {
         d3data.x = data.temperature;
         humok = true;
      }
   }

   if (pressure->pressureRead(data)) {
      if (data.temperatureValid) {
         d3data.y = data.temperature;
         pressok = true;
      }
   }

   d3data.valid = humok && pressok;

   return d3data;
}

double SenseHAT::SenseHATLinux::get_temperature_from_humidity()
{
   RTIMU_DATA data;
   if (!humidity->humidityRead(data)) {
      return nan("");
   }
   if (!data.temperatureValid) {
      return nan("");
   }
   return data.temperature;
}

double SenseHAT::SenseHATLinux::get_temperature_from_pressure()
{
   RTIMU_DATA data;
   if (!pressure->pressureRead(data)) {
      return nan("");
   }
   if (!data.temperatureValid) {
      return nan("");
   }
   return data.temperature;
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
