/*
** EnvironmentController.h
** Controls the fermenting environment. Decides which actors to use, and how
**  to implement them
** Output:
** Input: Heater, Fridge, Humidifier, Fan, Chamber Temp, Liquid Temp
** Platform: Particle Photon
** Author: Benjamin Veenema - Veenema Design Works
** Date Created: 1/31/16
*/
#ifndef EnvironmentController_h
#define EnvironmentController_h

#define MAXTEMP 3800
#define MINTEMP 0
#define MAXRH 100
#define MINRH 0
#define MAXFAN 10
#define MINFAN 0

#define MINFANONLOOPTEMP 3800
#define HEATERONFANSPEED 10

#define DEADBAND 50

#include "application.h"
#include "Heater.h"
#include "Fridge.h"
#include "Humidifier.h"
#include "Fan.h"
#include "KS103J2.h"
#include "HDC1000.h"

class EnvironmentController{
public:
	EnvironmentController(Heater&, Fridge&, Humidifier&, Fan&, KS103J2&, KS103J2&, HDC1000&, HDC1000&);
  void init(uint16_t targetTemp, uint8_t targetRH, uint8_t targetFan);
  void init(void);
  void update(void);

	//Mode
	void setMode(uint8_t mode);
	uint8_t getMode();

  //Allowances
  void setAllowances(bool heater, bool fridge, bool humidifier, bool fan);
  void setHeaterAllowed(bool allowed);
  void setFridgeAllowed(bool allowed);
  void setHumidifierAllowed(bool allowed);
  void setFanAllowed(bool allowed);
  bool getHeaterAllowed(void);
  bool getFridgeAllowed(void);
  bool getHumidiferAllowed(void);
  bool getFanAllowed(void);

  //Targets
  void setTargets(uint16_t targetTemp, uint8_t targetRH, uint8_t targetFan);
  void setTargetTemp(uint16_t temperature);
  void setTargetRH(uint16_t RH);
  void setTargetFan(uint8_t Fan);
	int16_t getTargetTemp(void);
  uint8_t getTargetRH(void);
  uint8_t getTargetFan(void);

	//Control Points
	void setTemperatureControlPoint(bool isLiquid);
	bool getTemperatureControlPoint(void);

private:

  int16_t _targetTemp;
	uint16_t _targetRH;
  uint8_t _targetFan, _mode;
  bool _heaterAllowed, _fridgeAllowed, _humidifierAllowed, _fanAllowed, _temperatureControlPoint;
	bool _heaterFridgeDirection;

	Heater& _cHeater;
	Fridge& _cFridge;
	Humidifier& _cHumidifier;
	Fan& _cFan;
	KS103J2& _cLiquidTemp, _cLoopTemp;
	HDC1000& _cChamberHDC1008, _cAmbientHDC1008;
};

#endif
