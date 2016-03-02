#include "Heater.h"

Heater::Heater(uint8_t pin){
  _pin = pin;
  pinMode(_pin, OUTPUT);
  OFF();
}
void Heater::update(int16_t temperature, int16_t loopTemperature){
  if( (_isON && ((millis()-_lastOnTime)<MINONTIME)) || (!_isON && ((millis()-_lastOffTime)<MINOFFTIME))){
    return;
  }
  if(temperature < _setpoint){
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

bool Heater::stop(int16_t loopTemperature){
  OFF();
  return 0;
}

void Heater::setTarget(int16_t temperature){
  _setpoint = constrain(temperature,MINTEMP,MAXTEMP);
}

uint16_t Heater::getTarget(void){
  return _setpoint;
}
bool Heater::isON(void){
  return _isON;
}
void Heater::ON(){
  if(_cycleUp && (millis()-_lastCycleTime>MAXONTIME)){
    pinResetFast(_pin);
    _cycleUp = 0;
    _lastCycleTime = millis();
  }else if(!_cycleUp && (millis()-_lastCycleTime>MINDOWNTIME)){
    pinSetFast(_pin);
    _cycleUp = 1;
    _lastCycleTime = millis();
  }
  _isON = 1;
}
void Heater::OFF(){
  pinResetFast(_pin);
  _isON = 0;
}


//---------------------------------------
//OLD Code
//---------------------------------------
/*void Heater::update(int16_t temperature, int16_t loopTemperature){

  if( (_isON && ((millis()-_lastOnTime)>MINONTIME)) || (!_isON && ((millis()-_lastOffTime)>MINOFFTIME)) ){
    if(temperature < _setpoint){
      ON();
      _lastOnTime = millis();
    }else{
      OFF();
      _lastOffTime = millis();
    }
  }
}*/
