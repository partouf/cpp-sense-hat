#include "sense-hat-sim.h"

SenseHAT::ISenseHAT *HATInstance = nullptr;

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

double SenseHAT::SenseHATSim::get_temperature()
{
   return 0.0;
}
