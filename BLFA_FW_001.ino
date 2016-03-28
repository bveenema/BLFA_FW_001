#include "application.h"

//Release Identifier String
String releaseString = "3/28/16 1241 Development";

//Sysem Settings
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

#include "KS103J2.h"
#include "Heater.h"
#include "Fridge.h"
#include "Humidifier.h"
#include "Fan.h"
#include "EnvironmentController.h"
#include "HDC1000.h"
#include "nextion.h"
#include "ChamberLED.h"

//Define Pins
#define AMBIENT_THERMISTOR A0
#define CHAMBER_THERMISTOR A1
#define LOOP_THERMISTOR A2
#define SOLENOID_1 A3
#define CHAMBER_LED A4
#define VENTILATION_FAN A5
#define FRIDGE_CTRL D5
#define HEATER_CTRL D6
#define HUMIDIFIER_CTRL D7

uint16_t ctof(uint16_t celsius);
uint16_t ftoc(uint16_t Farenheit);

//Globals
const String isFarenheit[2]{
	"Celsius",
	"Farenheit"
};
const String isON[2]{
	"OFF",
	"ON"
};

const String Mode[5]{
	"Beer",
	"Cheese",
	"Meat",
	"Custom",
  "Off",
};

bool testHDC = 0;

bool nexTemperatureUnit = 0; //0: Celsius, 1: Farenheit - For display only, software uses Celsius
bool FLAG_nexIsSleeping = 0;
bool FLAG_nexWokeUp = 0;
uint8_t nexCurrentPage = 0;

uint32_t timeStartedListening = 0;
uint32_t timeNexWokeUp = 0;

//Cloud Variables
int32_t chamberTemp = 0;
int32_t liquidTemp = 0;
int32_t loopTemp = 0;
int32_t ambTemp = 0;
int32_t chamberHumidity = 0;
int32_t ambHumidity = 0;

//Instantiate Library Objects
Heater heater(HEATER_CTRL);
Fridge fridge(FRIDGE_CTRL);
Humidifier humidifier(SOLENOID_1);
Fan fan(VENTILATION_FAN);
KS103J2 liquid_TR(CHAMBER_THERMISTOR,9990,0);
KS103J2 loop_TR(LOOP_THERMISTOR,9990,0);
HDC1000 chamber_HDC1008 = HDC1000();
HDC1000 ambient_HDC1008 = HDC1000();
EnvironmentController chamber(heater, fridge, humidifier, fan, liquid_TR, loop_TR, chamber_HDC1008, ambient_HDC1008);
ChamberLED ChamberLED(CHAMBER_LED,100);



//Instantiate Nextion Display Objects + Callbacks
//Splash Screen (Page 0) Objects and Callbacks
NexPage SplashScreen = NexPage(0,0,"splash");
NexSleep Wake = NexSleep(0,0,"sleep");
void WakeCallback(void *ptr);

//Page 1 Objects + Callbacks
NexNumber nexTempTarget = NexNumber(1, 8, "nexTempTarget");
NexNumber nexRHTarget = NexNumber(1, 10, "nexRHTarget");
NexNumber nexFanTarget = NexNumber(1, 11, "nexFanTarget");
NexNumber nexTempCurrent = NexNumber(1, 12, "nexTempCurrent");
NexNumber nexRHCurrent = NexNumber(1, 13, "nexRHCurrent");
NexNumber nexFanCurrent = NexNumber(1, 14, "nexFanCurrent");
NexHotspot nexSettingPlus = NexHotspot(1, 9, "nexSettingPlus");
NexHotspot nexSettingMinu = NexHotspot(1, 7, "nexSettingMinu");
NexTimer Page1Load = NexTimer(1, 18, "Page1Load");
NexVar item = NexVar(1,17,"item");
void nexSettingPlusPopCallback(void *ptr);
void nexSettingMinuPopCallback(void *ptr);
void Page1LoadCallback(void *ptr);

//Page 2 Objects + Callbacks
NexDSButton LightSwitch = NexDSButton(2,3,"LightSwitch");
NexTimer Page2Load = NexTimer(2,9,"Page2Load");
NexHotspot Sleep = NexHotspot(2,6,"Sleep");
void LightSwitchCallback(void *ptr);
void Page2LoadCallback(void *ptr);
void SleepCallback(void *ptr);

//Page 3 Objects + Callbacks
NexPage SettingsPage = NexPage(3,0,"settings");
NexTimer Page3Load = NexTimer(3,6,"Page3Load");
NexDSButton CorF = NexDSButton(3,2,"CorF");
NexDSButton WifiButton = NexDSButton(3,7,"WifiButton");
NexDSButton ListenButton = NexDSButton(3,3,"bt0");
NexCrop ModeButton = NexCrop(3,1,"ModeButton");
NexHotspot ModeYes = NexHotspot(3,12,"ModeYes");
NexVar modeVar = NexVar(3,8,"modeVar");
void Page3LoadCallback(void *ptr);
void CorFCallback(void *ptr);
void WifiButtonCallback(void *ptr);
void ListenButtonCallback(void *ptr);
void ModeButtonCallback(void *ptr);
void ModeYesCallback(void *ptr);


NexTouch *nex_listen_list[] =
{
	&Wake,
  &nexSettingPlus,
  &nexSettingMinu,
	&Page1Load,
	&LightSwitch,
	&Page2Load,
	&Sleep,
	&Page3Load,
	&CorF,
	&WifiButton,
	&ListenButton,
	&ModeButton,
	&ModeYes,
  NULL
};


void setup(){
	//Release String Variable Setup
	Particle.variable("release",releaseString);
	Particle.variable("chamberTemp",chamberTemp);
	Particle.variable("liquidTemp",liquidTemp);
	Particle.variable("loopTemp",loopTemp);
	Particle.variable("ambientTemp",ambTemp);
	Particle.variable("chamberHum",chamberHumidity);
	Particle.variable("ambientHum",ambHumidity);
	Particle.connect();

  //Turn Electronics fan on
  pinMode(HUMIDIFIER_CTRL, OUTPUT);
  pinSetFast(HUMIDIFIER_CTRL);

	// Initialize Serial and Nex Display
	nexInit(115200);
	Wake.attachPop(WakeCallback);
	nexSettingPlus.attachPop(nexSettingPlusPopCallback);
	nexSettingMinu.attachPop(nexSettingMinuPopCallback);
	Page1Load.attachTimer(Page1LoadCallback);
	LightSwitch.attachPop(LightSwitchCallback);
	Page2Load.attachTimer(Page2LoadCallback);
	Sleep.attachPop(SleepCallback);
	Page3Load.attachTimer(Page3LoadCallback);
	CorF.attachPop(CorFCallback);
	WifiButton.attachPop(WifiButtonCallback);
	ListenButton.attachPop(ListenButtonCallback);
	ModeButton.attachPop(ModeButtonCallback);
	ModeYes.attachPop(ModeYesCallback);
	delay(250);

	// Initialize Chamber
	chamber.init();
	chamber.setTemperatureControlPoint(0);
  chamber.setMode(4);
	chamber.setTargets(2000,0,0);
	Serial.println("Setup Chamber Set Targets");

	// Initialize HDC1008
	ambient_HDC1008.begin(0x41);
  chamber_HDC1008.begin(0x40);

	delay(15);
}

void loop(){
	static uint32_t loopTimer;
	uint16_t loopTime = micros()-loopTimer;
	//Serial.printlnf("LoopTime: %d",loopTime);
	loopTimer = micros();
	//Update Nextion Loop
	nexLoop(nex_listen_list);


	//Update Chamber
	chamber.update();

	//Check Serial
	//TODO

	//Read Liquid Thermistor
	liquidTemp = liquid_TR.read();

  //Read Chamber Temp and Humidity
  chamber_HDC1008.ReadTempHumidity();
  chamberTemp = chamber_HDC1008.GetTemperatureInt();
  chamberHumidity = chamber_HDC1008.GetHumidityInt();

	// Read Ambient Humidity
	ambient_HDC1008.ReadTempHumidity();
  ambTemp = ambient_HDC1008.GetTemperatureInt();
	ambHumidity = ambient_HDC1008.GetHumidityInt();

	//Read Loop Thermistor
	loopTemp = loop_TR.read();

	//Output Current Values + Loop Time (serial + Nextion Display)
	static uint32_t previousSerialOutput;
	const uint16_t serialDisplayRate = 1000;
	if(millis() - previousSerialOutput > serialDisplayRate){
		previousSerialOutput = millis();

		if(nexTemperatureUnit){
			chamberTemp = ctof(chamberTemp);
		}

		if(nexCurrentPage == 1){
			uint32_t nexTemp = 0;
			uint32_t number = 0;
			if(chamber.getTemperatureControlPoint()){
				nexTemp = (liquidTemp/100)+((liquidTemp%100)/50);
			}else{
				nexTemp = (chamberTemp/100)+((chamberTemp%100)/50);
			}
			uint32_t nexHumidity = (chamberHumidity/100)+((chamberHumidity%100)/50);
			uint8_t nexFanSpeed = fan.getTarget();
			nexTempCurrent.setValue(nexTemp);
			nexRHCurrent.setValue(nexHumidity);
			nexFanCurrent.setValue(nexFanSpeed);

			//Serial Output
			float displayChamberTemp = (float)chamberTemp/100.0;
	    float displayLiquidTemp = (float)liquidTemp/100.0;
			float displayLoopTemp = (float)loopTemp/100.0;
	    float displayAmbTemp = (float)ambTemp/100.0;
	    float displayChamberHumidity = (float)chamberHumidity/100.0;
			float displayAmbHumidity = (float)ambHumidity/100.0;
			bool heaterIsOn = heater.isON();
			Serial.printf("Chamber Temp: %3.1f ",displayChamberTemp);Serial.print(isFarenheit[nexTemperatureUnit]);
				Serial.print("\tHeater is: ");Serial.println(isON[heaterIsOn]);
	    Serial.printf("Liquid Temp: %3.1f ",displayLiquidTemp);Serial.println(isFarenheit[nexTemperatureUnit]);
			Serial.printf("Loop Temp: %3.1f ",displayLoopTemp);Serial.println(isFarenheit[nexTemperatureUnit]);
	    Serial.printf("Ambient Temp: %3.1f ",displayAmbTemp);Serial.println(isFarenheit[nexTemperatureUnit]);
			Serial.printlnf("Chamber Humidity: %2.1f",displayChamberHumidity);
	    Serial.printlnf("Ambient Humidity: %2.1f",displayAmbHumidity);
				Serial.println(" ");
		}
	}
	if(WiFi.listening()){
		if(millis()-timeStartedListening > 60000){
			WiFi.listen(false);
			Serial1.begin(115200);
		}
	}
	if(FLAG_nexWokeUp){
		if(millis()-timeNexWokeUp>55){
			SplashScreen.show();
			nexCurrentPage = 0;
			Serial.println("Apparate Splash Screen");
			FLAG_nexWokeUp = 0;
		}
	}
}



int32_t ctof(int32_t celsius){
	int32_t farenheit = celsius*9/5+3200;
	return farenheit;
}
int32_t ftoc(int32_t farenheit){
	int32_t celsius = (farenheit-3200)*5/9;
	return celsius;
}

void WakeCallback(void *ptr){
	Serial.println("Nex has awoken");
	NexSendCommand(NexSLEEP,0,"");
	FLAG_nexWokeUp = 1;
	timeNexWokeUp = millis();
}

void nexSettingPlusPopCallback(void *ptr){
  dbSerialPrintln("nexSettingPlus Touched");

	uint32_t number;
  uint32_t value;

  item.getValue(&value);
  if(value==0){
		nexTempTarget.getValue(&number);
		number *= 100;
		if(nexTemperatureUnit){
			number = ftoc((int32_t)number);
		}
		Serial.printlnf("heater target: %d",number);
		chamber.setTargetTemp(number);
  }else if(value==1){
		nexRHTarget.getValue(&number);
		number=number*100;
		Serial.printlnf("RH target: %d",number);
		chamber.setTargetRH(number);
  }else if(value==2){
		nexFanTarget.getValue(&number);
		Serial.printlnf("Fan target: %d",number);
		chamber.setTargetFan(number);
  }
}

void nexSettingMinuPopCallback(void *ptr){
  dbSerialPrintln("nexSettingMinu Touched");
  uint32_t number;
  uint32_t value;

  item.getValue(&value);
  if(value==0){
		nexTempTarget.getValue(&number);
		number *= 100;
		if(nexTemperatureUnit){
			number = ftoc((int32_t)number);
		}
		Serial.printlnf("heater target: %d",number);
		chamber.setTargetTemp(number);
  }else if(value==1){
		nexRHTarget.getValue(&number);
		number *= 100;
		Serial.printlnf("RH target: %d",number);
		chamber.setTargetRH(number);
  }else if(value==2){
		nexFanTarget.getValue(&number);
		Serial.printlnf("Fan target: %d",number);
		chamber.setTargetFan(number);
  }
}

void Page1LoadCallback(void *ptr){
	//Stop timer from running again
	Page1Load.disable();

	//Set Nextion Page Number
	nexCurrentPage = 1;

	// Update Temperature Setpoint
	int32_t number = chamber.getTargetTemp();
	if(nexTemperatureUnit){
		number = ctof(number);
	}
	number = (number/100)+((number%100)/50);
	nexTempTarget.setValue(number);

	// Update RH Setpoint
	uint16_t rh = chamber.getTargetRH();
	Serial.printlnf("Chamber RH Load: %d",rh);
	rh = (rh/100)+((rh%100)/50);
	nexRHTarget.setValue(rh);

	// Update Fan Setpoint
	uint8_t speed = chamber.getTargetFan();
	nexFanTarget.setValue(speed);
	//TODO

	Serial.println("Page 1 Load");
}

void LightSwitchCallback(void *ptr){
	uint32_t lightState = 0;
	LightSwitch.getValue(&lightState);
	if(lightState == 1){
		ChamberLED.setState(1);
		Serial.println("Light ON!");
	}else if (lightState == 0){
		ChamberLED.setState(0);
		Serial.println("Light OFF!");
	}else{
		Serial.println("LightSwitch value error!!!");
	}
}

void SleepCallback(void *ptr){
	FLAG_nexIsSleeping = 1;
	Serial.println("Shhh, Nex is sleeping...");
}
void Page2LoadCallback(void *ptr){
	//Stop timer from running again
	Page2Load.disable();

	//Set Nextion Page Number
	nexCurrentPage = 2;

	// Update Chamber Light Button
	uint32_t lightState = 0;
	lightState = ChamberLED.getState();
	LightSwitch.setValue(lightState);

	Serial.println("Page 2 Load");
}

void Page3LoadCallback(void *ptr){
	//Stop timer from running again
	Page3Load.disable();

	//Set Nextion Page Number
	nexCurrentPage = 3;

	//Update Mode Button
  uint8_t number = chamber.getMode();
	ModeButton.setPic(number+7);
	modeVar.setValue(number);

	// Update C or F Button
	CorF.setValue(nexTemperatureUnit);

	//Update Wifi Button
	WifiButton.setValue(Particle.connected());

	Serial.println("Page 3 Load");

}
void CorFCallback(void *ptr){
	uint32_t number;
	CorF.getValue(&number);
	if(number==1){
		nexTemperatureUnit=number;
		Serial.println("Farenheit");
	}else if(number==0){
		nexTemperatureUnit=number;
		Serial.println("Celsius");
	}else{
		Serial.println("Temperature Unit detection Error!");
	}
}

void WifiButtonCallback(void *ptr){
	uint32_t number;
	WifiButton.getValue(&number);
	if(number == 1){
		Serial.print("Connecting to cloud...");
		Particle.connect();
		Serial.println(" Connected!");
	}else if(number == 0){
		Serial.print("Disconnecting from cloud...");
		Particle.disconnect();
		Serial.println(" Disconnected :(");
	}else{
		Serial.println("WifiButton value error!!!");
	}
}

void ListenButtonCallback(void *ptr){
	uint32_t number;
	ListenButton.getValue(&number);
	if(number == 1){
		WiFi.listen();
		Serial.println("Listening...");
		timeStartedListening = millis();
	}else if(number == 0){
		WiFi.listen(false);
		Serial.println("No Longer Listening");
	}else{
		Serial.println("Listen button ERROR!!!");
	}
}

void ModeButtonCallback(void *ptr){
	Serial.println("Mode PopUp");
}

void ModeYesCallback(void *ptr){
	Serial.print("New Mode: ");
	uint32_t number;

	modeVar.getValue(&number);
	ModeButton.setPic(number+7);
	if(number<5){
		chamber.setMode(number);
		Serial.println(Mode[number]);
	}else{
		Serial.println("ERROR!");
	}

}
