/*
** Fan.h
** Controls low level functions of the fan
** Output: PWM on fan pin
** Input: Setpoint
** Platform: Particle Photon
** Author: Benjamin Veenema - Veenema Design Works
** Date Created: 2/5/16
*/

#ifndef Fan_h
#define Fan_h

#define MAXFANSPEED 10
#define MINFANSPEED 0
#define MAXFANPWM 255
#define MINFANPWM 200

#include "application.h"

class Fan{
public:
	Fan(uint8_t pin);
  void update(bool turnON);
	void update(bool turnON, uint8_t fanSpeed);
	bool stop(void);
  void setTarget(uint8_t fanSpeed);
	uint8_t getTarget(void);
  bool isON(void);
private:
  void ON(void);
  void OFF(void);

	uint8_t _pin;
  bool _isON;
  uint8_t _fanSpeed, _stopCount;
};

#endif
