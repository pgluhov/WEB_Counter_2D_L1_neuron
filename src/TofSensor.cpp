#define DEBUG_distance 0

#include "TofSensor.h"
#include <Arduino.h>
#include <Wire.h>

int person_threshold = 1200; 
int door_threshold = 200;
int zoneX = 14; 
int zoneY =7;  
//uint8_t opticalCenters[2] = {68,188};
uint8_t opticalCenters[2] = {60,196};

TofSensor::TofSensor():
Sensor(),myTofSensor()
{}

void TofSensor::setPersonThreshold(int value){
  person_threshold = value;
}

void TofSensor::setDoorThreshold(int value){
  door_threshold = value;
}

void TofSensor::setZoneArea(int valueX, int valueY){
  zoneX = valueX; 
  zoneY = valueY; 
}

void TofSensor::setOpticalCenters(int valueZ1, int valueZ2){
  opticalCenters[0] = valueZ1; 
  opticalCenters[1] = valueZ2; 
}

int TofSensor::getDistanse_Z1(){
  return distance_zone1;
}

int TofSensor::getDistanse_Z2(){
  return distance_zone2;
}

int TofSensor::CallOffset(int distsnse){
  myTofSensor.calibrateOffset(distsnse);
  int Offset = myTofSensor.getOffset();
  Serial.println();
  Serial.print("Результат калибровки calibrateOffset=");
  Serial.print(Offset);  
  Serial.println();
  return Offset;
}

int TofSensor::CallXTalk(int distsnse){
  myTofSensor.calibrateXTalk(distsnse);
  int XTalk = myTofSensor.getXTalk();
  Serial.print("Результат калибровки calibrateXTalk=");  
  Serial.print(XTalk);  
  Serial.println();
  Serial.println();
  return XTalk;
}

bool TofSensor::setup(int SensOffset, int SensXTalk){
      //Wire.setClock(400000); 
      Wire.begin(21,22,400000); //SDA, SCL
      delay(100);
      if(myTofSensor.begin() != 0){
        delay(100);
        Serial.println("Sensor error");
        return false;        
      }
      
      myTofSensor.setOffset(SensOffset);    
      myTofSensor.setXTalk(SensXTalk);     
      myTofSensor.setDistanceModeLong();         // Set to 3,8M range 
      //myTofSensor.setDistanceModeShort();      // Set to 1,3M range 
      myTofSensor.setTimingBudgetInMs(20);       //Set the timing budget for a measurement
      //myTofSensor.setIntermeasurementPeriod(40); //Set time between measurements in ms         
      startMeasurement();
      return true;
  }

bool TofSensor::setup(){
      //Wire.setClock(400000); 
      Wire.begin(21,22,400000); //SDA, SCL
      delay(100);
      if(myTofSensor.begin() != 0){
        delay(100);
        Serial.println("Sensor error");
        return false;        
      } 
                  
      myTofSensor.setDistanceModeLong();        // Set to 3,8M range 
      //myTofSensor.setDistanceModeShort();     // Set to 1,3M range 
      myTofSensor.setTimingBudgetInMs(20);      //Set the timing budget for a measurement
      //myTofSensor.setIntermeasurementPeriod(40); //Set time between measurements in ms         
      startMeasurement();
      return true;
  }

void TofSensor::update(){
      if(myTofSensor.checkForDataReady()){
        getResult();
        if( (currentZone == 1) && !newDataAvailable){
          newDataAvailable = true;
        }        
        currentZone = !currentZone;
        startMeasurement();     
      }
}

void TofSensor::startMeasurement(){
  myTofSensor.stopRanging();
  myTofSensor.clearInterrupt();
  myTofSensor.setROI(zoneX,zoneY,opticalCenters[currentZone]);
  delay(5);
  myTofSensor.startRanging();
}

void TofSensor::getResult(){
  int distance = myTofSensor.getDistance();
  if(currentZone == 0){zone1 = personPresent(distance); // 0-1
    distance_zone1 = distance;
    #if (DEBUG_distance == 1)
    Serial.print("Zone1: ");Serial.print(distance);
    #endif    
  }
  else{  
    distance_zone2 = distance;  
    zone2 = personPresent(distance); // 0-1
    #if (DEBUG_distance == 1)
    Serial.print("  Zone2: ");Serial.println(distance);
    #endif
  }
}

bool TofSensor::personPresent(int distance){
  //return ( (door_threshold < distance) && (distance < person_threshold) );
  return distance < person_threshold ;
}

 /* TofSensor имеет приемную решетку, состоящую из 16x16 однофотонных диодов.
  * По умолчанию используются все из них, но вы можете уменьшить приемник до интересующей области (ROI).
  * Он должен быть размером не менее 4X4, но может быть больше и иметь неровные стороны.
  * Он определяется длинами сторон в направлениях x и y и его центром.
  * Чтобы установить центр, установите контактную площадку, которая находится справа и над точным центром области, которую вы хотите измерить, в качестве оптического центра.
  * Вот вызов функции: setROI(uint8_t x, uint8_t y, uint8_topticCenter);
  *
  ***   Вот таблица оптических центров   ***
  *
  * 128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248
  * 129,137,145,153,161,169,177,185,193,201,209,217,225,233,241,249
  * 130,138,146,154,162,170,178,186,194,202,210,218,226,234,242,250
  * 131,139,147,155,163,171,179,187,195,203,211,219,227,235,243,251
  * 132,140,148,156,164,172,180,188,196,204,212,220,228,236,244,252
  * 133,141,149,157,165,173,181,189,197,205,213,221,229,237,245,253
  * 134,142,150,158,166,174,182,190,198,206,214,222,230,238,246,254
  * 135,143,151,159,167,175,183,191,199,207,215,223,231,239,247,255
  
  * 127,119,111,103, 95, 87, 79, 71, 63, 55, 47, 39, 31, 23, 15, 7
  * 126,118,110,102, 94, 86, 78, 70, 62, 54, 46, 38, 30, 22, 14, 6
  * 125,117,109,101, 93, 85, 77, 69, 61, 53, 45, 37, 29, 21, 13, 5
  * 124,116,108,100, 92, 84, 76, 68, 60, 52, 44, 36, 28, 20, 12, 4
  * 123,115,107, 99, 91, 83, 75, 67, 59, 51, 43, 35, 27, 19, 11, 3
  * 122,114,106, 98, 90, 82, 74, 66, 58, 50, 42, 34, 26, 18, 10, 2
  * 121,113,105, 97, 89, 81, 73, 65, 57, 49, 41, 33, 25, 17, 9,  1
  * 120,112,104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8,  0 
  */
