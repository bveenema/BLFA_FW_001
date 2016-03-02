#include "Fan.h"

	Fan::Fan(uint8_t pin){
    _pin = pin;
    pinMode(_pin, OUTPUT);
  }

  void Fan::update(bool turnON){
    if(turnON){
      ON();
    }else{
      OFF();
    }
		_stopCount = 0;
  }

	void Fan::update(bool turnON, uint8_t fanSpeed){
		_fanSpeed = fanSpeed;
		if(turnON){
			ON();
		}else{
			OFF();
		}
		_stopCount = 0;
	}

	bool Fan::stop(){
		if(_stopCount > 2){
			_stopCount = 3;
	    OFF();
		}else{
			_stopCount += 1;
		}
    return _isON;
  }

  void Fan::setTarget(uint8_t fanSpeed){
    _fanSpeed = constrain(fanSpeed, MINFANSPEED, MAXFANSPEED);
  }

	uint8_t Fan::getTarget(void){
    return _fanSpeed;
  }

  bool Fan::isON(void){
    return _isON;
  }

  void Fan::ON(){
		if(_fanSpeed == 0){
			OFF();
			return;
		}
    uint8_t pwm = map(_fanSpeed, MINFANSPEED, MAXFANSPEED, MINFANPWM, MAXFANPWM);
    //analogWrite(_pin, pwm, 65535);
		pinSetFast(_pin);
    _isON = 1;
  }

  void Fan::OFF(){
    pinResetFast(_pin);
    _isON = 0;
  }
