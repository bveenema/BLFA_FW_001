/*
** Humidifier.h
** Controls low level functions of the Humidifier
** Output: Relay through solendoid 1
** Input: Humidity
** Platform: Particle Photon
** Author: Benjamin Veenema - Veenema Design Works
** Date Created: 3/2/16
*/

#ifndef Humidifier_h
#define Humidifier_h

#define MAXRH 9900
#define MINRH 0

#include "application.h"

class Humidifier{
public:
	Humidifier(uint8_t pin);
  void update(uint16_t rh);
	bool stop();
  void setTarget(uint16_t rh);
	uint16_t getTarget(void);
  bool isON(void);
private:
  void ON();
  void OFF();

	uint8_t _pin;
  bool _isON;
  uint16_t _setpoint;

};

#endif
