#include <wiringPi.h>

#include <iostream>

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command 
// which uses gpio export for setup for wiringPiSetupSys
#define	LED	17

#include "sense-hat.h"
#include "sense-hat-linux.h"

int main(void)
{
/*
wiringPiSetupSys();

	pinMode(LED, OUTPUT);

	while (true)
	{
		digitalWrite(LED, HIGH);  // On
		delay(500); // ms
		digitalWrite(LED, LOW);	  // Off
		delay(500);
	}
*/
   SenseHAT::ISenseHAT *Hat = new SenseHAT::SenseHATLinux();

   std::cout << "Humidity: " << Hat->get_humidity() << std::endl;
   std::cout << "Pressure: " << Hat->get_pressure() << std::endl;

   SenseHAT::d3 data;

   data = Hat->get_temperature();
   std::cout << "Temp1: " << data.x << std::endl;
   std::cout << "Temp2: " << data.y << std::endl;


   data = Hat->get_gyro();
   while (!data.valid)
   {
      delay(100);
      data = Hat->get_gyro();
   }
   std::cout << "Gyro: {" << data.x << "," << data.y << "," << data.z << "}" << std::endl;

   data = Hat->get_accel();
   while (!data.valid)
   {
      delay(100);
      data = Hat->get_accel();
   }
   std::cout << "Accel: {" << data.x << "," << data.y << "," << data.z << "}" << std::endl;

   data = Hat->get_magno();
   while (!data.valid)
   {
      delay(100);
      data = Hat->get_magno();
   }
   std::cout << "Magno: {" << data.x << "," << data.y << "," << data.z << "}" << std::endl;

   delete Hat;

   delay(5000);

	return 0;
}

