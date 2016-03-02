#include "EnvironmentController.h"

EnvironmentController::EnvironmentController(Heater& cHeater, Fridge& cFridge, Fan& cFan, KS103J2& cLiquidTemp, HDC1000& cLoopHDC1008, HDC1000& cAmbientHDC1008):
  _cHeater(cHeater),
  _cFridge(cFridge),
  _cFan(cFan),
  _cLiquidTemp(cLiquidTemp),
  _cLoopHDC1008(cLoopHDC1008),
  _cAmbientHDC1008(cAmbientHDC1008)
  {}

void EnvironmentController::init(uint16_t targetTemp, uint8_t targetRH, uint8_t targetFan){
  setTargets(targetTemp,targetRH,targetFan);
  setAllowances(0,0,0,0);
}
void EnvironmentController::init(void){
  setTargets(0,0,0);
  setAllowances(0,0,0,0);
}

void EnvironmentController::update(void){
  // Temperature Control
    //Get Temperatures
    _cLoopHDC1008.ReadTempHumidity();
    int16_t currentTemperature = 0;
    if(_temperatureControlPoint == 0){
      currentTemperature = _cLoopHDC1008.GetTemperatureInt();
    }else if(_temperatureControlPoint == 1){
      currentTemperature = _cLiquidTemp.read();
    }
    int16_t loopTemp = currentTemperature;

    //IF only Heater allowed
    if(_heaterAllowed && !_fridgeAllowed){
      _cHeater.update(currentTemperature,loopTemp);
      _cFridge.stop();
    }

    if(_fridgeAllowed && !_heaterAllowed){
      _cFridge.update(currentTemperature);
      _cHeater.stop(loopTemp);
    }

    //IF Heater and Fridge allowed
    if(_heaterAllowed && _fridgeAllowed){
      if(currentTemperature < (_targetTemp - DEADBAND)){
        _cHeater.update(currentTemperature,loopTemp);
        _cFridge.stop();
        _heaterFridgeDirection = 1;
      }else if(currentTemperature > (_targetTemp + DEADBAND)){
        _cHeater.stop(loopTemp);
        _cFridge.update(currentTemperature);
        _heaterFridgeDirection = 0;
      }else if(_heaterFridgeDirection == 1){
        _cHeater.update(currentTemperature,loopTemp);
        _cFridge.stop();
      }else if(_heaterFridgeDirection == 0){
        _cHeater.stop(loopTemp);
        _cFridge.update(currentTemperature);
      }
    }

    //if neither Heater of Fridge allowed
    if(!_heaterAllowed && !_fridgeAllowed){
      _cFridge.stop();
      _cHeater.stop(loopTemp);
    }


  // Humidity Control
    //IF Humidifier allowed
      //Update Humidifier
    //Else Stop Humidifer

  // Fan Control
    if(_fanAllowed){
      _cFan.update(1,_targetFan);
    }else if(_cHeater.isON()){
      _cFan.update(1,HEATERONFANSPEED);
    }else if(loopTemp > MINFANONLOOPTEMP){
      _cFan.update(1,HEATERONFANSPEED);
    }else{
      _cFan.stop();
    }


}

//Modes
void EnvironmentController::setMode(uint8_t mode){
  _mode = constrain(mode,0,4);
  switch (_mode){
    case 0: //Beer
      setAllowances(1,1,0,1);
      break;
    case 1: //Cheese
      setAllowances(1,1,1,1);
      break;
    case 2: //Meat
      setAllowances(1,0,1,1);
      break;
    case 3: //Custom
      setAllowances(0,1,0,1);
      break;
    case 4: //Off
      setAllowances(0,0,0,0);
      break;
    default:
      Serial.println("Mode Selection Error");
      break;
  }
}
uint8_t EnvironmentController::getMode(){
  return _mode;
}

//Allowances
void EnvironmentController::setAllowances(bool heater, bool fridge, bool humidifier, bool fan){
  _heaterAllowed = heater;
  _fridgeAllowed = fridge;
  _humidifierAllowed = humidifier;
  _fanAllowed = fan;
}
void EnvironmentController::setHeaterAllowed(bool allowed){
  _heaterAllowed = allowed;
}
void EnvironmentController::setFridgeAllowed(bool allowed){
  _fridgeAllowed = allowed;
}
void EnvironmentController::setHumidifierAllowed(bool allowed){
  _humidifierAllowed = allowed;
}
void EnvironmentController::setFanAllowed(bool allowed){
  _fanAllowed = allowed;
}
bool EnvironmentController::getHeaterAllowed(void){
  return _heaterAllowed;
}
bool EnvironmentController::getFridgeAllowed(void){
  return _fridgeAllowed;
}
bool EnvironmentController::getHumidiferAllowed(void){
  return _humidifierAllowed;
}
bool EnvironmentController::getFanAllowed(void){
  return _fanAllowed;
}

//Targets
void EnvironmentController::setTargets(uint16_t targetTemp, uint8_t targetRH, uint8_t targetFan){
  _targetTemp = constrain(targetTemp,MINTEMP,MAXTEMP);
  _targetRH = constrain(targetRH,MINRH,MAXRH);
  _targetFan = constrain(targetFan,MINFAN,MAXFAN);
}
void EnvironmentController::setTargetTemp(uint16_t temperature){
  _targetTemp = constrain(temperature,MINTEMP,MAXTEMP);
  _cHeater.setTarget(_targetTemp);
  _cFridge.setTarget(_targetTemp);
}
void EnvironmentController::setTargetRH(uint8_t RH){
  _targetRH = constrain(RH,MINRH,MAXRH);
}
void EnvironmentController::setTargetFan(uint8_t Fan){
  _targetFan = constrain(Fan,MINFAN,MAXFAN);
  _cFan.setTarget(_targetFan);
}
int16_t EnvironmentController::getTargetTemp(void){
  return _targetTemp;
}
uint8_t EnvironmentController::getTargetRH(void){
  return _targetRH;
}
uint8_t EnvironmentController::getTargetFan(void){
  return _targetFan;
}

//Control Points
void EnvironmentController::setTemperatureControlPoint(bool isLiquid){
  _temperatureControlPoint = isLiquid; //0: Air 1: Liquid
}

bool EnvironmentController::getTemperatureControlPoint(void){
  return _temperatureControlPoint;
}
