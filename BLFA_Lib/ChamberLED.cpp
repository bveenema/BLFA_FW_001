#include "ChamberLED.h"

	ChamberLED::ChamberLED(uint8_t pin, uint8_t brightness){
    _pin = pin;
    _brightness = constrain(brightness,MINDUTY,MAXDUTY);

    pinMode(_pin, OUTPUT);
    OFF();
  }

  void ChamberLED::setState(bool state){
    if(state){
      ON();
    }else{
      OFF();
    }
  }
  bool ChamberLED::getState(void){
    return _state;
  }

  void ChamberLED::setBrightness(uint8_t brightness){
    _brightness = constrain(brightness,MINDUTY,MAXDUTY);
  }
	uint16_t ChamberLED::getBrightness(void){
    return _brightness;
  }

  void ChamberLED::ON(){
    _state = 1;
    uint16_t pwmValue = _brightness*255/100;
		digitalWrite(_pin, HIGH);
    //analogWrite(_pin, pwmValue);
  }
  void ChamberLED::OFF(){
    _state = 0;
    pinResetFast(_pin);
  }
