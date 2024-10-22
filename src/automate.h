#ifndef AUTOMATE_H
#define AUTOMATE_H
#include <Arduino.h>

enum Zone {NO_ZONE, UPPER_ZONE, LOWER_ZONE, BOTH_ZONES};

enum State {
    START,
    FROM_UPPER_TO_BOTH,  // Состояние движения сверху вниз
    FROM_BOTH_TO_LOWER,  // Состояние движения через обе зоны вниз
    FROM_LOWER_TO_BOTH,  // Состояние движения снизу вверх
    FROM_BOTH_TO_UPPER   // Состояние движения через обе зоны вверх
};

State currentState  = START;
int outdoor = 0;
int indoor  = 0;

void updateState(Zone zone) {
    switch (currentState) {
        case START:            
            if (zone == UPPER_ZONE) { // Отслеживаем вход в зону
                currentState = FROM_UPPER_TO_BOTH;
            }
            if (zone == LOWER_ZONE) {
                currentState = FROM_LOWER_TO_BOTH;
            }
            break;
        case FROM_UPPER_TO_BOTH:  // Переход из верхней зоны через обе зоны вниз            
            if (zone == BOTH_ZONES) {
                currentState = FROM_BOTH_TO_LOWER;
            } else if (zone == NO_ZONE) {               
                currentState = START;  // Если объект покинул верхнюю зону без перехода в BOTH_ZONES
            }
            break;
        case FROM_BOTH_TO_LOWER: // Завершение пути, если был в BOTH_ZONES и двигается в LOWER_ZONE            
            if (zone == LOWER_ZONE) {                
                outdoor++;
                Serial.println("counter_outdoor++");
                currentState = START;
            } else if (zone == NO_ZONE) {// Если переход в LOWER_ZONE не был зарегистрирован                
                currentState = START;
            }
            break;
        case FROM_LOWER_TO_BOTH: // Переход из нижней зоны через обе зоны верх            
            if (zone == BOTH_ZONES) {
                currentState = FROM_BOTH_TO_UPPER;
            } else if (zone == NO_ZONE) {                
                currentState = START; // Если объект покинул нижнюю зону без перехода в BOTH_ZONES
            }
            break;
        case FROM_BOTH_TO_UPPER: // Завершение пути, если был в BOTH_ZONES и двигается в UPPER_ZONE            
            if (zone == UPPER_ZONE) {                
                indoor++;
                Serial.println("counter_indoor++");
                currentState = START;
            } else if (zone == NO_ZONE) {// Если переход в UPPER_ZONE не был зарегистрирован                
                currentState = START;
            }
            break;
    }
}

#endif //AUTOMATE_H