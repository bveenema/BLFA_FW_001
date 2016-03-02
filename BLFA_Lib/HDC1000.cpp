/***************************************************
  This is a library for the HDC1000 Humidity & Temp Sensor

  Designed specifically to work with the HDC1008 sensor from Adafruit
  ----> https://www.adafruit.com/products/2635

  These sensors use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  Modified for Photon  needs application.h for types  RMB
 ****************************************************/
#include "HDC1000.h"


HDC1000::HDC1000() {
}


boolean HDC1000::begin(uint8_t addr) {
  _i2caddr = addr;

  Wire.begin();

  reset();
  if (read16(HDC1000_MANUFID) != 0x5449) return false;
  if (read16(HDC1000_DEVICEID) != 0x1000) return false;
  return true;
}



void HDC1000::reset(void) {
  // reset,combined temp/humidity measurement,and select 14 bit temp & humidity resolution
  // heater not needed for accurate humidity readings     RMB
  uint16_t config = HDC1000_CONFIG_RST | HDC1000_CONFIG_MODE | HDC1000_CONFIG_TRES_14 | HDC1000_CONFIG_HRES_14 ;

  Wire.beginTransmission(_i2caddr);
  Wire.beginTransmission(HDC1000_CONFIG);   // set pointer register to configuration register   RMB
  Wire.write(config>>8);        // now write out 2 bytes MSB first    RMB
  Wire.write(config&0xFF);
  Wire.endTransmission();
  delay(15);
}

void HDC1000::ReadTempHumidity(void) {
  // HDC1008 setup to measure both temperature and humidity in one sequential conversion
  // this is a different way to access data in ONE read
  // this sets internal private variables that can be accessed by Get() functions

  uint32_t rt,rh;  // working variables

  rt = read32(HDC1000_TEMP, 20);    // get temp and humidity reading together
  rh = rt;                          // save a copy for humidity processing

  // important to use ( ) around temp so private variable set and float cast done
  rt = rt >> 16;
  #ifdef Enable_Float_Report
    _temp = (float)rt;        // convert to temp first
    _temp /= 65536;
    _temp *= 165;
    _temp -= 40;
      //Serial.printlnf("Float Temp: %f",_temp);
  #endif

  _tempINT = rt*100*165;
  _tempINT /= 65536;
  _tempINT -=4000;
    //Serial.printlnf("Int Temp: %d",_tempINT);


  // important to use ( ) around humidity so private variable set and float cast done
  rh = rh & 0xFFFF;
  #ifdef Enable_Float_Report
    _humidity = (float)rh;   // now convert to humidity
    _humidity /= 65536;
    _humidity *= 100;
      //Serial.printlnf("Float Humidity: %f",_humidity);
  #endif

  _humidityINT = rh*10000;
  _humidityINT /= 65536;
    //Serial.printlnf("Int Humidity: %d",_humidityINT);
}


uint32_t HDC1000::GetTemperatureInt(void){
  return _tempINT;
}
uint32_t HDC1000::GetHumidityInt(void){
  return _humidityINT;
}

#ifdef Enable_Seperate_Reads
  uint32_t HDC1000::readTemperatureInt(void){
    uint32_t temperature = (read32(HDC1000_TEMP, 20) >> 16);
      temperature *= 16500;
      temperature /= 65536;
      temperature -=4000;
    return temperature;
  }
  uint32_t HDC1000::readHumidityInt(void){
    uint32_t humidity = (read32(HDC1000_TEMP, 20) & 0xFFFF);
      humidity *= 10000;
      humidity /= 65536;
    return humidity;
  }
#endif

#ifdef Enable_Float_Report
  #ifdef Enable_Seperate_Reads
    float HDC1000::readTemperature(void) {
      // does not set private variable
      float temp = (read32(HDC1000_TEMP, 20) >> 16);
        temp /= 65536;
        temp *= 165;
        temp -= 40;
      return temp;
    }
    float HDC1000::readHumidity(void) {
      // reads both temp and humidity but masks out temp in highest 16 bits
      // does not set private variable
      float hum = (read32(HDC1000_TEMP, 20) & 0xFFFF);
        hum /= 65536;
        hum *= 100;
      return hum;
    }
  #endif

  float HDC1000::GetTemperature(void) {
    // getter function to access private _temp variable
    return _temp ;
  }
  float HDC1000::GetHumidity(void) {
    // getter function to access private _humidity variable
    return _humidity ;
  }
#endif

// Add ability to test battery voltage, useful in remote monitoring, TRUE if <2.8V
// usually called after _temp/Humid reading  RMB
// Thanks to KFricke for micropython-hdc1008 example on GitHub
boolean HDC1000::batteryLOW(void)  {
  // set private variable, don't need delay to read Config register
  battLOW = (read16(HDC1000_CONFIG, 0));

  battLOW &= HDC1000_CONFIG_BATT;   // mask off other bits, bit 11 will be 1 if voltage < 2.8V

  if (battLOW > 0) return true;
  return  false;
}



/*********************************************************************/

uint16_t HDC1000::read16(uint8_t a, uint8_t d) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(a);
  Wire.endTransmission();
  delay(d);
  Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)2);
  uint16_t r = Wire.read();
  r <<= 8;
  r |= Wire.read();
  //Serial.println(r, HEX);
  return r;
}

uint32_t HDC1000::read32(uint8_t a, uint8_t d) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(a);
  Wire.endTransmission();
  // delay was hardcoded as 50, should use d   RMB
  delay(d);
  Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)4);
  uint32_t r = Wire.read();
  // assembles temp into highest 16 bits, _humidity into lowest 16 bits
  r <<= 8;
  r |= Wire.read();
  r <<= 8;
  r |= Wire.read();
  r <<= 8;
  r |= Wire.read();
  //Serial.println(r, HEX);
  return r;
}
