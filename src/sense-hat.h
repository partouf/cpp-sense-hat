#pragma once

#include <cstdint>

namespace SenseHAT
{
   struct d3
   {
      double x;
      double y;
      double z;
      bool valid;
   };

   class ISenseHAT
   {
   public:
      virtual double get_humidity() = 0;
      virtual double get_pressure() = 0;
      virtual d3 get_gyro() = 0;
      virtual d3 get_accel() = 0;
      virtual d3 get_magno() = 0;
      virtual double get_temperature() = 0;
   };
};
