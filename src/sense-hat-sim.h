#pragma once

#include "sense-hat.h"

namespace SenseHAT
{
   class SenseHATLedMatrixSim : public ISenseHATLEDMatrix {
   public:
      SenseHATLedMatrixSim();

      virtual void Clear() override;

      virtual void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) override;

      virtual void SetFromRgbaMatrix(const SenseHATColor_t matrix[8][8]) override;
   };

   class SenseHATSim: public ISenseHAT {
   public:
      SenseHATSim();

      virtual double get_humidity() override;
      virtual double get_pressure() override;
      virtual d3 get_temperature() override;

      virtual d3 get_gyro() override;
      virtual d3 get_accel() override;
      virtual d3 get_magno() override;

      static ISenseHAT *Instance();
   };
};
