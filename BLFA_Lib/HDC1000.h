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

  Sensor datasheet: http://www.ti.com/lit/ds/symlink/hdc1008.pdf

 ****************************************************/
#ifndef HDC1008_h
#define HDC1008_h

#if defined(SPARK)
#include "application.h"
#else
#include "Arduino.h"
#endif

//Values can be reported as either uint32_t or float
//If float reporting is required, define Enable_Float_Report
//Otherwise do not define to save code space
//#define Enable_Float_Report

// Define Enable_Seperate_Reads if you intend to use the individual
// "readTemperature[Int]()" and "readHumidity[Int]()" methods
//#define Enable_Seperate_Reads

#define HDC1000_TEMP          0x00
#define HDC1000_HUMID         0x01
#define HDC1000_CONFIG        0x02
#define HDC1000_CONFIG_RST    (1 << 15)
#define HDC1000_CONFIG_HEAT   (1 << 13)
#define HDC1000_CONFIG_MODE   (1 << 12)
#define HDC1000_CONFIG_BATT   (1 << 11)
#define HDC1000_CONFIG_TRES_14  0
#define HDC1000_CONFIG_TRES_11  (1 << 10)
#define HDC1000_CONFIG_HRES_14  0
#define HDC1000_CONFIG_HRES_11  (1 << 8)
#define HDC1000_CONFIG_HRES_8   (1 << 9)

#define HDC1000_SERIAL1       0xFB
#define HDC1000_SERIAL2       0xFC
#define HDC1000_SERIAL3       0xFD
#define HDC1000_MANUFID       0xFE
#define HDC1000_DEVICEID      0xFF



class HDC1000 {
 public:
  HDC1000();
  bool begin(uint8_t addr);

  #ifdef Enable_Float_Report
    #ifdef Enable_Seperate_Reads
      float readTemperature(void);
      float readHumidity(void);
    #endif
    float GetTemperature(void);
    float GetHumidity(void);
  #endif

  #ifdef Enable_Seperate_Reads
    uint32_t readTemperatureInt(void);
    uint32_t readHumidityInt(void);
  #endif
  uint32_t GetTemperatureInt(void);            // added getter function to access private temp
  uint32_t GetHumidityInt(void);              // added getter function to access private humidity

  void reset(void);
  void ReadTempHumidity(void);    // added one conversion/one read version, updates private variables   RMB
  bool batteryLOW(void);            // added testing of battery voltage, useful for remote monitoring

  uint16_t read16(uint8_t a, uint8_t d=0);
  uint32_t read32(uint8_t a, uint8_t d=0);

 private:
  bool readData(void);   // not being used
  #ifdef Enable_Float_Report
    float _humidity, _temp;
  #endif
  uint32_t _humidityINT, _tempINT;
  uint16_t battLOW;
  uint8_t _i2caddr;
};

#endif
