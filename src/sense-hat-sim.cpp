#include "sense-hat-sim.h"
#include <memory>

SenseHAT::ISenseHAT *HATInstance = nullptr;

SenseHAT::SenseHATSim::SenseHATSim() : ISenseHAT()
{
   LEDMatrix = std::make_unique<SenseHAT::SenseHATLedMatrixSim>();
}

double SenseHAT::SenseHATSim::get_humidity()
{
   return 0.0;
}

double SenseHAT::SenseHATSim::get_pressure()
{
   return 0.0;
}

SenseHAT::d3 SenseHAT::SenseHATSim::get_gyro()
{
   return{ 0, 0, 0, true };
}

SenseHAT::d3 SenseHAT::SenseHATSim::get_accel()
{
   return{ 0, 0, 0, true };
}

SenseHAT::d3 SenseHAT::SenseHATSim::get_magno()
{
   return{ 0, 0, 0, true };
}

SenseHAT::ISenseHAT * SenseHAT::SenseHATSim::Instance()
{
   if (HATInstance == nullptr)
   {
      HATInstance = new SenseHAT::SenseHATSim();
   }

   return HATInstance;
}

SenseHAT::d3 SenseHAT::SenseHATSim::get_temperature()
{
   return {0,0,0,true};
}

SenseHAT::SenseHATLedMatrixSim::SenseHATLedMatrixSim() : ISenseHATLEDMatrix()
{
}

void SenseHAT::SenseHATLedMatrixSim::Clear()
{
}

void SenseHAT::SenseHATLedMatrixSim::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
}

void SenseHAT::SenseHATLedMatrixSim::SetFromRgbaMatrix(const SenseHATColor_t matrix[8][8])
{
}
