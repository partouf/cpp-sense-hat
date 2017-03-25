#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>

namespace SenseHAT
{
   struct d3
   {
      double x;
      double y;
      double z;
      bool valid;
   };

   class SenseHATException : public std::runtime_error
   {
   public:
      SenseHATException(const char *message) : std::runtime_error(message) {}
   };

   class SenseHATErrnoException : public SenseHATException
   {
   public:
      SenseHATErrnoException(int errornumber) : SenseHATException("ErrnoException") {
         this->ErrorNumber = errornumber;
      }

      int ErrorNumber;
   };

   typedef uint32_t SenseHATColor_t;

   struct SenseHATColor_rgba
   {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t a;

      SenseHATColor_rgba(const SenseHATColor_t &rawint)
      {
         r = static_cast<uint8_t>((rawint >> 24) & 0xff);
         g = static_cast<uint8_t>((rawint >> 16) & 0xff);
         b = static_cast<uint8_t>((rawint >> 8) & 0xff);
         a = static_cast<uint8_t>((rawint) & 0xff);
      }
   };

   class ISenseHATLEDMatrix
   {
   public:
      virtual ~ISenseHATLEDMatrix();

      virtual void Clear() = 0;

      virtual void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) = 0;

      virtual void SetFromRgbaMatrix(const SenseHATColor_t matrix[8][8]) = 0;
   };

   class ISenseHAT
   {
   public:
      virtual ~ISenseHAT();

      virtual double get_humidity() = 0;
      virtual double get_pressure() = 0;
      virtual d3 get_gyro() = 0;
      virtual d3 get_accel() = 0;
      virtual d3 get_magno() = 0;
      virtual d3 get_temperature() = 0;

      std::unique_ptr<SenseHAT::ISenseHATLEDMatrix> LEDMatrix;
   };
};
