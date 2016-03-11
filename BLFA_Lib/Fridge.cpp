#include "Fridge.h"

Fridge::Fridge(uint8_t pin){
  _pin = pin;
  pinMode(_pin, OUTPUT);
  OFF();
}
void Fridge::update(int16_t temperature){

  if( (_isON && ((millis()-_lastOnTime)>MINONTIME)) || (!_isON && ((millis()-_lastOffTime)>MINOFFTIME)) ){
    if(temperature >= _setpoint){
      if(!_isON){
        _lastOnTime = millis();
      }
      ON();
    }else{
      if(_isON){
        _lastOffTime = millis();
      }
      OFF();
    }
  }
}

bool Fridge::stop(){
  if(_isON && (millis()-_lastOnTime>(MINONTIME/2)) || !_isON){
    OFF();
    return 0;
  }
  return 1;
}

void Fridge::setTarget(int16_t temperature){
  _setpoint = constrain(temperature,MINTEMP,MAXTEMP);
}

uint16_t Fridge::getTarget(void){
  return _setpoint;
}
bool Fridge::isON(void){
  return _isON;
}
void Fridge::ON(){
  digitalWrite(_pin, HIGH);
  _isON = 1;
}
void Fridge::OFF(){
  digitalWrite(_pin, LOW);
  _isON = 0;
}
