/*
** KS103J2.h
** Reads the temperature of KS103J2 thermistor
** Output: 	uint16_t in farenheit (1) or Celcius (0)
						output is *100 of real value (pseudo floating point)
** Input:
** Platform: Particle Photon
** Author: Benjamin Veenema - Veenema Design Works
** Date Created: 12/22/15
*/

#ifndef KS103J2_h
#define KS103J2_h

#include "application.h"

class KS103J2{
public:
	KS103J2(uint8_t pin, uint16_t fixedRes, bool isFarenheit);
	void setUnit(bool isFarenheit);
	bool getUnit(void);
	void setResistance(uint16_t fixedRes);
	int16_t read(void);
private:
	uint16_t analogReadModeFilter(uint8_t numReads);

	uint8_t _pin;
	bool _isFarenheit;
	uint16_t _fixedRes;

};

#endif
