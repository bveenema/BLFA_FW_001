#include "Heater.h"

Heater::Heater(uint8_t pin, uint16_t setpoint){
  _pin = pin;
  _setpoint = setpoint;
  pinMode(_pin, OUTPUT);
  OFF();
}
void Heater::update(uint16_t temperature){
  if(temperature < _setpoint){
    ON();
  }else{
    OFF();
  }
}

void Heater::setTarget(uint16_t temperature){
  constrain(temperature,MINTEMP,MAXTEMP);
  _setpoint = temperature;
}

uint16_t Heater::getTarget(void){
  return _setpoint;
}
bool Heater::isON(void){
  return _isON;
}
void Heater::ON(){
  pinSetFast(_pin);
  _isON = 1;
}
void Heater::OFF(){
  pinResetFast(_pin);
  _isON = 0;
}
