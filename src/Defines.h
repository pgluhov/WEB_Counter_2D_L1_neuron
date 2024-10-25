#ifndef DEFINES_H
#define DEFINES_H

#include <Arduino.h>
String DEVICE_NAME = "Vormatic 2D Counter (L1)"; // Имя девайса
String CURRENT_VERSION_SW = "3.15";              // Текущая версиия прошивки 
String VERSION_SW = "Software version 3.15";     // Текст для отображения

// ========== ДЕФАЙНЫ НАСТРОЕК ==========

#define ENABLE_DEBUGING 0 // Если 1 то отладка включена
#define ENABLE_DEBUG    0 // Если 1 то отладка включена
#define ENABLE_LOCKING  1 // Если 1 защита установлена
#define ENABLE_DEBUG_UART 0  // Если 1 то отладка обмена по uart включена 
#define ENABLE_DEBUG_TASK 0  // Если 1 то отладка клавиатуры включена
#define ENABLE_DEBUG_JSON 0
#define ENABLE_DEBUG_GET 0
#define ENABLE_DEBUG_UPDATE 0
#define ENABLE_DEBUG_DATA_WAITING 0
#define ENABLE_DEBUG_ACTION_WEB 0
#define ENABLE_PARSING 0
#define ENABLE_PARSING_GROUP 0
#define ENABLE_PARSING_PLOT 0

#define DISTANSE_CALLIBRATION_SENSOR 1600 // Расстояние от сенсора до калибровочного препятствия в миллиметрах

//-------------объявлять ДО ПОДКЛЮЧЕНИЯ БИБЛИОТЕКИ GyverPortal------------------
//#define GP_NO_MDNS          // убрать поддержку mDNS из библиотеки (вход по хосту в браузере)
//#define GP_NO_DNS           // убрать поддержку DNS из библиотеки (для режима работы как точка доступа)
//#define GP_NO_OTA           // убрать поддержку OTA обновления прошивки
#define GP_NO_UPLOAD          // убрать поддержку загрузки файлов на сервер
//#define GP_NO_DOWNLOAD      // убрать поддержку скачивания файлов с сервера

#define PERIOD_UPDATE_TIME 60 // Период обновления времени с NTP-сервера в минутах

String apiKey = "yEmAT7Al8j7F9";
#define INIT_KEY 40           // ключ первого запуска. 0-254, на выбор
#define MAX_COUNT_FILES 5     // Максимальное количество csv файлов в памяти устройства
#define MAX_DATA_WAITING 50   // Максимальное количество строк данных ожидающих записи


//--------номера IO-------------------
#define btn_reboot 14
#define shut_tof_pin 16
#define int_tof_pin 17


#define pwmBrightnessLed 200 // яркость свотодиодов 0-254
#define pwmFrequencyLed 1000 // частота ШИМ (в Гц)
#define pwmResolutionLed 8   // разрешение ШИМа (в битах) 
#define pwmChannelRed   1    // канал, на который назначим в дальнейшем ШИМ
#define pwmChannelGreen 2    // канал, на который назначим в дальнейшем ШИМ
#define pwmChannelBlue  3    // канал, на который назначим в дальнейшем ШИМ
#define led_red   26         // Пин IO подключенный к красному
#define led_green 32         // Пин IO подключенный к зеленому
#define led_blue  33         // Пин IO подключенный к синему
#define led_gnd   25         // Пин IO gnd для светодиода

#define SOUND 27          // Пин IO подключенный к зуммеру
#define pwmFrequency 1000 // частота ШИМ (в Гц)
#define pwmResolution 8   // разрешение ШИМа (в битах) 
#define pwmChannel    0   // канал, на который назначим в дальнейшем ШИМ 

#endif //DEFINES_H
