#pragma once

#include "sense-hat.h"
#include <cstdint>

class RTHumidity;
class RTPressure;
class RTIMUSettings;
class RTIMU;

namespace SenseHAT
{
   class SenseHATLEDMatrixLinux : public ISenseHATLEDMatrix {
   private:
      int fbfd;

      void init_fb();
   public:
      SenseHATLEDMatrixLinux();
      ~SenseHATLEDMatrixLinux();

      virtual void Clear() override;

      virtual void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) override;

      virtual void SetFromRgbaMatrix(const SenseHATColor_t matrix[8][8]) override;
   };

   class SenseHATLinux : public ISenseHAT {
   public:
      SenseHATLinux();
      ~SenseHATLinux();

      // Sensor methods
      virtual double get_humidity() override;
      virtual double get_pressure() override;
      virtual d3 get_temperature() override;

      virtual d3 get_gyro() override;
      virtual d3 get_accel() override;
      virtual d3 get_magno() override;

      static ISenseHAT *Instance();
   private:
      double get_temperature_from_humidity();
      double get_temperature_from_pressure();

      RTIMUSettings* settings;
      RTIMU* imu;
      RTHumidity* humidity;
      RTPressure* pressure;
   };
};
