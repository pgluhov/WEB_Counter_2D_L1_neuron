#ifndef TOF_SENSOR_H
#define TOF_SENSOR_H

#include "Sensor.h"
#include <SparkFun_VL53L1X.h>
#include <ComponentObject.h>
#include <RangeSensor.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>

class TofSensor : public Sensor{

public:
  TofSensor();
  bool setup(int SensOffset, int SensXTalk); 
  bool setup();
  void update() override; 
  void setPersonThreshold(int value); 
  void setDoorThreshold(int value); 
  void setZoneArea(int valueX, int valueY); 
  void setOpticalCenters(int valueZ1, int valueZ2);
  int getDistanse_Z1();
  int getDistanse_Z2();
  int CallOffset(int distsnse);
  int CallXTalk(int distsnse);

private:
  SFEVL53L1X myTofSensor;
  int currentZone = 0;
  int distance_zone1 = 0;
  int distance_zone2 = 0;
  void startMeasurement();
  void getResult();
  bool personPresent(int distance);
};

#endif
