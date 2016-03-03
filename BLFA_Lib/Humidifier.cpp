#include "Humidifier.h"

Humidifier::Humidifier(uint8_t pin){
  _pin = pin;
  pinMode(_pin, OUTPUT);
  OFF();
}
void Humidifier::update(uint16_t rh){
  if(rh < _setpoint){
    ON();
  }else{
    OFF();
  }
}
bool Humidifier::stop(){
  OFF();
  return 0;
}
void Humidifier::setTarget(uint16_t rh){
  _setpoint = constrain(rh,MINRH,MAXRH);
}
uint16_t Humidifier::getTarget(void){
  return _setpoint;
}
bool Humidifier::isON(void){
  return _isON;
}
void Humidifier::ON(){
  pinSetFast(_pin);
  _isON = 1;
}
void Humidifier::OFF(){
  pinResetFast(_pin);
  _isON = 0;
}
