#include "KS103J2.h"

typedef struct{const uint32_t res; const int16_t temp;} res_t;

res_t RT[]{
  {55329,-1000},
  {49656,-800},
  {44626,-600},
  {40159,-400},
  {36187,-200},
  {34367,-100},
  {32650,0},
  {31029,100},
  {29498,200},
  {28052,300},
  {26685,400},
  {25392,500},
  {24170,600},
  {23013,700},
  {21918,800},
  {20882,900},
  {19901,1000},
  {18971,1100},
  {18090,1200},
  {17255,1300},
  {16463,1400},
  {15712,1500},
  {14999,1600},
  {14323,1700},
  {13681,1800},
  {13072,1900},
  {12493,2000},
  {11942,2100},
  {11419,2200},
  {10922,2300},
  {10450,2400},
  {10000,2500},
  {9572,2600},
  {9165,2700},
  {8777,2800},
  {8408,2900},
  {8057,3000},
  {7722,3100},
  {7402,3200},
  {7098,3300},
  {6808,3400},
  {6531,3500},
  {6267,3600},
  {6015,3700},
  {5775,3800},
  {5545,3900},
  {5326,4000},
  {5117,4100},
  {4917,4200},
  {4725,4300},
  {4543,4400},
  {4368,4500},
  {4201,4600},
  {4041,4700},
  {3888,4800},
  {3602,5000},
  {2986,5500},
  {2488,6000},
  {2083,6500},
  {1752,7000},
  {1480,7500},
  {1256,8000},
  {1070,8500},
  {916,9000},
  {787,9500},
  {679,10000},
  {266,13500},
};

KS103J2::KS103J2(uint8_t pin, uint16_t fixedRes, bool isFarenheit){
	_pin = pin;
	_fixedRes = fixedRes;
	_isFarenheit = isFarenheit;

	pinMode(_pin,INPUT);
}
void KS103J2::setUnit(bool isFarenheit){
	_isFarenheit = isFarenheit;
}

bool  KS103J2::getUnit(void){
  return _isFarenheit;
}

void  KS103J2::setResistance(uint16_t fixedRes){
	constrain(fixedRes,0,50000);
	_fixedRes = fixedRes;
}

int16_t KS103J2::read(void){
  int16_t temperature = 0;

	//Get Raw ADC
	uint16_t adcRaw = analogReadModeFilter(15);

	//Calculate Voltage (x100) =RAW/adcResolution*Vsupply
	uint32_t voltage = adcRaw*3300/4096;

	//Calculate Resistance RTop = ((VSupply*Rbottom)/VMidPoint) - RBottom
	uint32_t resistance = ((3300*_fixedRes)/voltage) - _fixedRes;

	//Lookup Temperature (Celcius) in table
  for( uint8_t i = 0; i < arraySize(RT); i++ )
  {
    if ( RT[i].res <= resistance && RT[i-1].res >= resistance )
    {
      double diffResistance = resistance - RT[i].res;
      double diffRT = RT[i-1].res - RT[i].res;

      temperature = RT[i].temp + ( RT[i-1].temp - RT[i].temp ) * diffResistance / diffRT;
    }
  }
  //Convert to Farenheit
	if(_isFarenheit){
    temperature = temperature*9/5+3200;
	}
	return temperature;

}


uint16_t KS103J2::analogReadModeFilter(uint8_t numReads){
	// read multiple values and sort them to take the mode
  uint16_t sortedValues[numReads];
  for(uint8_t i=0;i<numReads;i++){
    uint16_t value = analogRead(_pin);
    uint8_t j;
    if(value<sortedValues[0] || i==0){
      j=0; //insert at first position
    }
    else{
      for(j=1;j<i;j++){
         if(sortedValues[j-1]<=value && sortedValues[j]>=value){
           // j is insert position
           break;
         }
      }
    }
    for(uint8_t k=i;k>j;k--){
      // move all values higher than current reading up one position
      sortedValues[k]=sortedValues[k-1];
    }
    sortedValues[j]=value; //insert current reading
  }
  //return scaled mode of 10 values
  uint16_t ADCAverage = 0;
  for(int i=numReads/2-5;i<(numReads/2+5);i++){
    ADCAverage +=sortedValues[i];
  }
  ADCAverage = ADCAverage/10;

  return ADCAverage;
}
