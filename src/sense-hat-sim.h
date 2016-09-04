#pragma once

#include "sense-hat.h"

namespace SenseHAT
{
   class SenseHATSim: public ISenseHAT {
   public:
      virtual double get_humidity() override;
      virtual double get_pressure() override;
      virtual double get_temperature() override;

      virtual d3 get_gyro() override;
      virtual d3 get_accel() override;
      virtual d3 get_magno() override;

      static ISenseHAT *Instance();
   };
};
