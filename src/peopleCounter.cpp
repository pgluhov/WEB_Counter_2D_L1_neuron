#include "Arduino.h"
#include "peopleCounter.h"
//#include "errorCodes.h"
#include "automate.h"

#define RESULT_OK 0
#define RESULT_FAIL -1
#define PERSON_TOO_FAST -2

#define DEBUG_COUNTER 1

enum zoneNames{ZONE1 = 0, ZONE2 = 1};
enum events{NONE = 0,ENTERED = 1,LEFT = 2};
enum globalEvents{EnteredZ1 = 1, LeftZ1 = 2, EnteredZ2 = 3, LeftZ2 = 4};

int counter_outdoor = 0;
int counter_indoor = 0;

PeopleCounter::PeopleCounter(){}

void PeopleCounter::setSensor(Sensor* sensor){
  mySensor = sensor;
}
  
int PeopleCounter::update(){
  
  if(mySensor -> dataAvailable()){

    int Z1,Z2,inside;
    
    Z1 = mySensor -> getZone1();
    Z2 = mySensor -> getZone2();
    inside = Z1 || Z2;

    if( !prevInside && inside ){       
       int event = getGlobalEvent(Z1,Z2);
       if(event < 0){
        return event;
       }
      entry = event;
    }
    else if(prevInside && !inside){
      int event = getGlobalEvent(Z1,Z2);
       if(event < 0){
        return event;
       }
      exit = event;
      int result = evaluate(entry,exit);
      count += result;
    }

    prevInside = inside;
    prevZ1 = Z1;
    prevZ2 = Z2;
    return RESULT_OK;
 }
 return RESULT_FAIL;
}

//int PeopleCounter::getCount(){
//    return count;
//  }

int PeopleCounter::getSingleZoneEvent(bool prevState,bool currentState){
  if(prevState == currentState){
    return NONE;
  }
  else if(!prevState && currentState){
    return ENTERED;
  }
  else if(prevState && !currentState){
    return LEFT;
  }
  return RESULT_FAIL;
}

int PeopleCounter::getGlobalEvent(bool zone1, bool zone2){
  int Z1Event = getSingleZoneEvent(prevZ1,zone1);
  int Z2Event = getSingleZoneEvent(prevZ2,zone2);

  if(Z1Event && Z2Event){ //если два события произошли одновременно, человек движется слишком быстро для датчика (не может определить направление)
    //Serial.println("Движется слишком быстро");
    return PERSON_TOO_FAST; 
  }

  if(Z1Event == ENTERED){
    #if (DEBUG_COUNTER ==1)
    Serial.println("Вошел в зону 1");
    #endif
    return EnteredZ1;
  }
  else if(Z1Event == LEFT){
    #if (DEBUG_COUNTER ==1)
    Serial.println("Вошел в зону 1");
    #endif
    return 2;
  }

  if(Z2Event == ENTERED){
    #if (DEBUG_COUNTER ==1)
    Serial.println("Вошел в зону 2");
    #endif
    return EnteredZ2;
  }
  else if(Z2Event == LEFT){
    #if (DEBUG_COUNTER ==1)
    Serial.println("Вошел в зону 2");
    #endif
    return 4;
  }
  return RESULT_FAIL;
}

int PeopleCounter::evaluate(int entry, int exit){
  #if (DEBUG_COUNTER ==1)
  Serial.print("Оценивание: ");
  #endif
  
  if( ( entry == EnteredZ1 ) && ( exit == LeftZ2 ) ){
    #if (DEBUG_COUNTER ==1)
    Serial.println("+1");
    #endif    
    counter_indoor++;
    return +1;    
    //return +1; // оригинал
  }
  else if( ( entry == EnteredZ2 ) && ( exit == LeftZ1 ) ){
    #if (DEBUG_COUNTER ==1)
    Serial.println("-1");
    #endif
    counter_outdoor++; 
    return -1;
    //return -1; // оригинал 
  }
 #if (DEBUG_COUNTER ==1)
  Serial.println("not");
  #endif
  return 0; 
}

int PeopleCounter::getcounter_outdoor(){ 
  return counter_outdoor; 
}

int PeopleCounter::getcounter_indoor(){ 
  return counter_indoor; 
}

void PeopleCounter::setcounter_outdoor(int value){ 
  counter_outdoor = value; 
}

void PeopleCounter::setcounter_indoor(int value){ 
  counter_indoor = value; 
}
