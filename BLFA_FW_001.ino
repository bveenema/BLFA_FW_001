#include "KS103J2.h"
#include "Heater.h"
//#include "Fridge.h"
#include "nextion.h"

//Define Pins
#define LOOP_THERMISTOR A0

#define VENTILATION_FAN D0
#define HEATER_CTRL D5
#define FRIDGE_CTRL D6

SYSTEM_MODE(AUTOMATIC);

//Globals
const String isFarenheit[2]{
	"Celcius",
	"Farenheit"
};
const String isON[2]{
	"OFF",
	"ON"
};

//Instantiate Library Objects
Heater heater(HEATER_CTRL, 3250);

// Fridge fridge(FRIDGE_CTRL);
KS103J2 loop_TR(LOOP_THERMISTOR,9990,0);

//Instantiate Nextion Display Objects
NexNumber n0 = NexNumber(1, 8, "n0");
NexNumber n1 = NexNumber(1, 10, "n1");
NexNumber n2 = NexNumber(1, 11, "n2");
NexNumber n3 = NexNumber(1, 12, "n3");
NexNumber n4 = NexNumber(1, 13, "n4");
NexNumber n5 = NexNumber(1, 14, "n5");
NexHotspot m5 = NexHotspot(1, 9, "m5");
NexHotspot m6 = NexHotspot(1, 7, "m6");
NexVar item = NexVar(1,17,"item");
void m5PopCallback(void *ptr);
void m6PopCallback(void *ptr);
NexTouch *nex_listen_list[] =
{
  &m5,
  &m6,
  NULL
};


void setup(){

	nexInit(115200);
	m5.attachPop(m5PopCallback);
	m6.attachPop(m6PopCallback);
	delay(250);

	//Initialize Display Values
	uint32_t nexTempTarget = heater.getTarget()/100;
	n0.setValue(nexTempTarget);

	//Turn on fan
	pinMode(VENTILATION_FAN,OUTPUT);
	digitalWrite(VENTILATION_FAN, 1);


}
void loop(){
	//Update Nextion Loop
	nexLoop(nex_listen_list);

	//Check Serial
	//TODO

	//Read Loop Thermistor
	int16_t loopTemp = loop_TR.read();

	//Read Loop Humidity Sensor
	//TODO

	//Control Heater
	bool heaterEnabled = 1;
	if(heaterEnabled){
		heater.update(loopTemp);
	}

	//Control Fridge
	//TODO

	//Output Current Values (serial + Nextion Display)
	static uint32_t previousSerialOutput;
	uint16_t serialDisplayRate = 1000;
	if(millis() - previousSerialOutput > serialDisplayRate){
		previousSerialOutput = millis();
		
		uint32_t nexTemp = loopTemp/100;
		n3.setValue(nexTemp);

		float displayTemp = (float)loopTemp/100.0;
		bool i = loop_TR.getUnit();
		bool heaterIsOn = heater.isON();
		Serial.printf("Loop Temp: %3.1f ",displayTemp);Serial.print(isFarenheit[i]);
			Serial.print("\tHeater is: ");Serial.print(isON[heaterIsOn]);
			Serial.println(" ");
	}
}





void m5PopCallback(void *ptr)
{
  dbSerialPrintln("m5 Touched");

	uint32_t number;
  uint32_t value;

  item.getValue(&value);
  if(value==0){

		n0.getValue(&number);
		Serial.printlnf("number = %d",number);
		number *= 100;
		Serial.printlnf("number*100 = %d",number);
		heater.setTarget(number);

		float displayNumber = (float)number/100.0;
		bool i = loop_TR.getUnit();
		Serial.printlnf("Heater Target: %3.1f ",displayNumber);Serial.print(isFarenheit[i]);

		number /=100;
		Serial.printlnf("number/100 = %d",number);
    n0.setValue(number);


  }else if(value==1){

  }else if(value==2){

  }
}

void m6PopCallback(void *ptr)
{
  dbSerialPrintln("m6 Touched");
  uint32_t number;
  uint32_t value;

  item.getValue(&value);
  if(value==0){
    n3.getValue(&number);
    number -= 1;
    n3.setValue(number);
  }else if(value==1){
    n4.getValue(&number);
    number -= 1;
    n4.setValue(number);
  }else if(value==2){
    n5.getValue(&number);
    number -= 1;
    n5.setValue(number);
  }
}
