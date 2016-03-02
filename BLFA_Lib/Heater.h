/*
** Heater.h
** Controls low level functions of the heater
** Output:
** Input: Temperature, assumes unit is Celcius
** Platform: Particle Photon
** Author: Benjamin Veenema - Veenema Design Works
** Date Created: 12/23/15
*/

#ifndef Heater_h
#define Heater_h

#define MAXTEMP 3800
#define MINTEMP 0
#define MAXONTIME 7500
#define MINDOWNTIME 35000
#define MINONTIME 1000
#define MINOFFTIME 50000

#include "application.h"

class Heater{
public:
	Heater(uint8_t pin);
  void update(int16_t temperature, int16_t loopTemperature);
	bool stop(int16_t loopTemperature);
  void setTarget(int16_t temperature);
	uint16_t getTarget(void);
  bool isON(void);
private:
  void ON();
  void OFF();

	uint8_t _pin;
  bool _isON, _cycleUp;
  int16_t _setpoint;
	uint32_t _lastOnTime, _lastOffTime, _lastCycleTime;

};

#endif
