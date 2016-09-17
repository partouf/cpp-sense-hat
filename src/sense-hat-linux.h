#pragma once

#include "sense-hat.h"
#include <cstdint>

class RTHumidity;
class RTPressure;
class RTIMUSettings;
class RTIMU;

namespace SenseHAT
{
   class SenseHATLinux : public ISenseHAT {
   public:
      SenseHATLinux();

      // Methods for manipulating LED matrix
      int blank();
      int set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

      // Sensor methods
      virtual double get_humidity() override;
      virtual double get_pressure() override;
      virtual d3 get_temperature() override;

      virtual d3 get_gyro() override;
      virtual d3 get_accel() override;
      virtual d3 get_magno() override;

      static ISenseHAT *Instance();
   private:
      int fbfd;

      int init_fb();

      double get_temperature_from_humidity();
      double get_temperature_from_pressure();

      RTIMUSettings* settings;
      RTIMU* imu;
      RTHumidity* humidity;
      RTPressure* pressure;

   };
};
