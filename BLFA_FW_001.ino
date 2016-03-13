//Release Identifier String
String releaseString = "3/13/16 15:36 Development";

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
  "Off"
};

bool temperatureUnit = 0; //0: Celsius, 1: Farenheit - For display only, software uses Celsius
bool testHDC = 0;
bool FLAG_nexPage1flag = 0;
bool FLAG_nexPage3flag = 0;
bool FLAG_nexPage3PopUpflag = 0;

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
void LightSwitchCallback(void *ptr);
void Page2LoadCallback(void *ptr);

//Page 3 Objects + Callbacks
NexPage SettingsPage = NexPage(3,0,"settings");
NexTimer Page3Load = NexTimer(3,4,"Page3Load");
NexDSButton CorF = NexDSButton(3,3,"CorF");
NexDSButton WifiButton = NexDSButton(3,5,"WifiButton");
NexDSButton ListenButton = NexDSButton(3,6,"bt0");
NexCrop ModeButton = NexCrop(3,9,"ModeButton");
NexHotspot ModeYes = NexHotspot(3,12,"ModeYes");
NexVar modeVar = NexVar(3,7,"modeVar");
void Page3LoadCallback(void *ptr);
void CorFCallback(void *ptr);
void WifiButtonCallback(void *ptr);
void ListenButtonCallback(void *ptr);
void ModeButtonCallback(void *ptr);
void ModeYesCallback(void *ptr);


NexTouch *nex_listen_list[] =
{
  &nexSettingPlus,
  &nexSettingMinu,
	&Page1Load,
	&LightSwitch,
	&Page2Load,
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
	Particle.connect();

  //Turn Electronics fan on
  pinMode(HUMIDIFIER_CTRL, OUTPUT);
  pinSetFast(HUMIDIFIER_CTRL);

	// Initialize Serial and Nex Display
	nexInit(115200);
	nexSettingPlus.attachPop(nexSettingPlusPopCallback);
	nexSettingMinu.attachPop(nexSettingMinuPopCallback);
	Page1Load.attachTimer(Page1LoadCallback);
	LightSwitch.attachPop(LightSwitchCallback);
	Page2Load.attachTimer(Page2LoadCallback);
	Page3Load.attachTimer(Page3LoadCallback);
	CorF.attachPop(CorFCallback);
	WifiButton.attachPop(WifiButtonCallback);
	ModeButton.attachPop(ModeButtonCallback);
	ModeYes.attachPop(ModeYesCallback);
	delay(250);

	// Initialize Chamber
	chamber.init();
	chamber.setTemperatureControlPoint(0);
  chamber.setMode(4);
	chamber.setTargets(2000,0,0);

	// Initialize HDC1008
	testHDC = ambient_HDC1008.begin(0x41);
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
	int16_t liquidTemp = liquid_TR.read();

  //Read Chamber Temp and Humidity
  chamber_HDC1008.ReadTempHumidity();
  int16_t chamberTemp = chamber_HDC1008.GetTemperatureInt();
  uint16_t chamberHumidity = chamber_HDC1008.GetHumidityInt();

	// Read Ambient Humidity
	ambient_HDC1008.ReadTempHumidity();
  int16_t ambTemp = ambient_HDC1008.GetTemperatureInt();
	uint16_t ambHumidity = ambient_HDC1008.GetHumidityInt();

	//Read Loop Thermistor
	int16_t loopTemp = loop_TR.read();

	//Output Current Values + Loop Time (serial + Nextion Display)
	static uint32_t previousSerialOutput;
	const uint16_t serialDisplayRate = 1000;
	if(millis() - previousSerialOutput > serialDisplayRate){
		previousSerialOutput = millis();

		if(temperatureUnit){
			chamberTemp = ctof(chamberTemp);
		}

		if(FLAG_nexPage1flag){
			uint32_t nexTemp = 0;
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
		}

		float displayChamberTemp = (float)chamberTemp/100.0;
    float displayLiquidTemp = (float)liquidTemp/100.0;
		float displayLoopTemp = (float)loopTemp/100.0;
    float displayAmbTemp = (float)ambTemp/100.0;
    float displayChamberHumidity = (float)chamberHumidity/100.0;
		float displayAmbHumidity = (float)ambHumidity/100.0;
		bool heaterIsOn = heater.isON();
		Serial.printf("Chamber Temp: %3.1f ",displayChamberTemp);Serial.print(isFarenheit[temperatureUnit]);
			Serial.print("\tHeater is: ");Serial.println(isON[heaterIsOn]);
    Serial.printf("Liquid Temp: %3.1f ",displayLiquidTemp);Serial.println(isFarenheit[temperatureUnit]);
		Serial.printf("Loop Temp: %3.1f ",displayLoopTemp);Serial.println(isFarenheit[temperatureUnit]);
    Serial.printf("Ambient Temp: %3.1f ",displayAmbTemp);Serial.println(isFarenheit[temperatureUnit]);
		Serial.printlnf("Chamber Humidity: %2.1f",displayChamberHumidity);
    Serial.printlnf("Ambient Humidity: %2.1f",displayAmbHumidity);
			Serial.println(" ");
	}
}



int16_t ctof(int16_t celsius){
	int16_t farenheit = celsius*9/5+3200;
	return farenheit;
}
int16_t ftoc(int16_t farenheit){
	int16_t celsius = (farenheit-3200)*5/9;
	return celsius;
}

void nexSettingPlusPopCallback(void *ptr){
  dbSerialPrintln("nexSettingPlus Touched");

	uint32_t number;
  uint32_t value;

  item.getValue(&value);
  if(value==0){
		nexTempTarget.getValue(&number);
		number *= 100;
		if(temperatureUnit){
			number = ftoc((int16_t)number);
		}
		Serial.printlnf("heater target: %d",number);
		chamber.setTargetTemp(number);
  }else if(value==1){
		nexRHTarget.getValue(&number);
		Serial.printlnf("RH target: %d",number);
		number=number*100;
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
		if(temperatureUnit){
			number = ftoc((int16_t)number);
		}
		Serial.printlnf("heater target: %d",number);
		chamber.setTargetTemp(number);
  }else if(value==1){
		nexRHTarget.getValue(&number);
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

	//Set Page 1 FLAG Clear Page 3 FLAG
	FLAG_nexPage1flag = 1;
	FLAG_nexPage3flag = 0;

	// Update Temperature Setpoint
	int16_t number = chamber.getTargetTemp();
	if(temperatureUnit){
		number = ctof(number);
	}
	number /= 100;
	nexTempTarget.setValue(number);

	// Update RH Setpoint
	uint8_t rh = chamber.getTargetRH();
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

void Page2LoadCallback(void *ptr){
	//Stop timer from running again
	Page2Load.disable();

	//Clear Page 1 FLAG and Page 3 FLAG
	FLAG_nexPage1flag = 0;
	FLAG_nexPage3flag = 0;

	// Update Chamber Light Button
	uint32_t lightState = 0;
	lightState = ChamberLED.getState();
	LightSwitch.setValue(lightState);

	Serial.println("Page 2 Load");
}

void Page3LoadCallback(void *ptr){
	//Stop timer from running again
	Page3Load.disable();

	//Set Page 3 FLAG, Clear Page 1 FLAG, Clear PopUp FLAG
	FLAG_nexPage3flag = 1;
	FLAG_nexPage1flag = 0;
	FLAG_nexPage3PopUpflag = 0;

	//Update Mode Button
  uint8_t number = chamber.getMode();
	ModeButton.setPic(number+7);
	modeVar.setValue(number);

	// Update C or F Button
	CorF.setValue(temperatureUnit);

	//Update Wifi Button
	WifiButton.setValue(Particle.connected());

	Serial.println("Page 3 Load");

}
void CorFCallback(void *ptr){
	uint32_t number;
	CorF.getValue(&number);
	if(number==1){
		temperatureUnit=number;
		Serial.println("Farenheit");
	}else if(number==0){
		temperatureUnit=number;
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
		Serial.println("Listening...");
		WiFi.listen();
		Serial.println(" Setup Complete");
	}else if(number == 0){
		WiFi.listen();
		Serial.println("No Longer Listening");
	}else{
		Serial.println("Listen button ERROR!!!");
	}
}

void ModeButtonCallback(void *ptr){
	//Set PopUp FLAG
	FLAG_nexPage3PopUpflag = 1;

	Serial.println("Mode PopUp");
}

void ModeYesCallback(void *ptr){
	Serial.print("New Mode: ");
	uint32_t number;

	modeVar.getValue(&number);
	if(number<5){
		chamber.setMode(number);
		Serial.println(Mode[number]);
	}else{
		Serial.println("ERROR!");
	}
  SettingsPage.show();
}
