#include "KS103J2.h"
//#include "lib/Heater.h"
//#include "lib/Fridge.h"

//Define Pins
#define LOOP_THERMISTOR A0

#define VENTILATION_FAN D0
#define HEATER_CTRL D5
#define FRIDGE_CTRL D6

SYSTEM_MODE(SEMI_AUTOMATIC);

//Instantiate Library Objects
// Heater heater(HEATER_CTRL);
// Fridge fridge(FRIDGE_CTRL);
KS103J2 loop_TR(LOOP_THERMISTOR,10000,1);


void setup(){
	Serial.begin(57600);
	delay(250);

	loop_TR.setUnit(1); //Set Farenheit

}
void loop(){
	//Check Serial
	//TODO

	//Read Loop Thermistor
	int16_t loopTemp = loop_TR.read();
	
	//Read Loop Humidity Sensor
	//TODO

	//Output Current Values (serial)
	// if(previousSerialOutput - millis() > serialDisplayRate){
	// 	float displayTemp = (float)loopTemp/100.0;
	// 	Serial.printlnf("Loop Temp: %f",displayTemp);
	// }
	
	//Control Heater
	// if(heaterEnabled){
	// 	heater.update(loopTemp);
	// }

	//Control Fridge
	//TODO
	delay(10);
}
