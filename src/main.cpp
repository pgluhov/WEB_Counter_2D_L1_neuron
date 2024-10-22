
#include "TofSensor.h"
#include "Defines.h"
#include "FS_function.h"
#include "errorCodes.h"
#include "peopleCounter.h"
//#include "automate.h"
TofSensor myTofSensor;
PeopleCounter peopleCounter;

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4;
TaskHandle_t Task5;
TaskHandle_t Task6;
TaskHandle_t Task7; 
TaskHandle_t Task8;
TaskHandle_t Task9;
TaskHandle_t Task10;
TaskHandle_t Task11;
TaskHandle_t Task12;
TaskHandle_t Task13;
TaskHandle_t Task14;
TaskHandle_t Task15;
TaskHandle_t Task16;
TaskHandle_t Task17;
SemaphoreHandle_t wifi_mutex;
SemaphoreHandle_t sensor_mutex;
SemaphoreHandle_t flash_mutex;

void Task1code(void * pvParameters);
void Init_Task1();
void Task2code(void * pvParameters);
void Init_Task2();
void Task3code(void * pvParameters);
void Init_Task3();
void Task4code(void * pvParameters);
void Init_Task4();
void Task5code(void * pvParameters);
void Init_Task5();
void Task6code(void * pvParameters);
void Init_Task6();
void Task7code(void * pvParameters);
void Init_Task7();
void Task8code(void * pvParameters);
void Init_Task8();
void Task9code(void * pvParameters);
void Init_Task9();
void Task10code(void * pvParameters);
void Init_Task10();
void Task11code(void * pvParameters);
void Init_Task11();
void Task12code(void * pvParameters);
void Init_Task12();
void Task13code(void * pvParameters);
void Init_Task13();
void Task14code(void * pvParameters);
void Init_Task14();
void Task15code(void * pvParameters);
void Init_Task15();
void Task16code(void * pvParameters);
void Init_Task16();
void Task17code(void * pvParameters);
void Init_Task17();

QueueHandle_t QueueHandleUart; // Определить дескриптор очереди
const int QueueElementSizeUart = 5;
typedef struct{
  uint64_t x;     
  uint32_t key_1;  
  uint32_t key_3;
  uint16_t key_4;
  uint64_t y;
} message_uart;

#include <esp_now.h>
esp_now_peer_info_t peerInfo;
uint8_t MasterAddress[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; // MAC Address master 
bool F_RECIEVE_COMPLETE = 0;  // Пришли новые данные
#define LOST_PACKET_MAX_TX 5 
#define LOST_PACKET_MAX_RX 5
int count_lost_tx_1 = 0;
int count_lost_send_1 = 0;
 
typedef struct struct_rx_t {  // Структура для отправки данных на master  
  uint8_t   device_mac[6];    // mac slave 
  float      tempC_val[5];    // значение температуры 
  bool      stat_input[5];    // миссив состояния входов
  float        current[5];    // значение тока
} struct_rx_t; 
struct_rx_t  Buff_to_master;

typedef struct struct_tx_t{   // Структура для приема данных от master
  uint8_t        slave_id;   // номер slave (резерв)
  uint8_t   device_mac[6];   // mac slave   (резерв)
  float        current[5];   // значение тока 
} struct_tx_t; 
struct_tx_t  Buff_Rx;

#pragma pack(push, 1)   // используем принудительное выравнивание
struct struct_uart_tx{  // Структура отправки данных о устройстве на генератор ключей  
  uint32_t key_1;
  uint64_t id_devise;
  uint32_t key_2;
  uint16_t key_3; 
  byte crc;
};
#pragma pack(pop) 
struct_uart_tx  buff_tx_uart;

#pragma pack(push, 1)   // используем принудительное выравнивание
struct struct_uart_rx{  // Структура приема данных от генератора ключей
  uint64_t x;  
  uint32_t key_1;
  uint64_t y;
  uint32_t key_3;
  uint16_t key_4;
  byte crc;
}; 
#pragma pack(pop)
struct_uart_rx  buff_rx_uart;

#include <WiFi.h>
#include <ESP32httpUpdate.h>
bool F_update = 0;   // Флаг для начала обновления
char link_sw[150];   // Ссылка для скачивания прошивки
int count_err = 0;   // Количество ошибочных попыток обновления
int max_err = 2;     // Максимальное количество ошибок, после которого перезагрузка и ещё одна попытка
WiFiServer server(80);
#include <HTTPClient.h>
#include <ArduinoJson.h>
int countNetworkFound = 0; // количество найденных сетей WiFi
String networkFound[35];   // Названия найденных сетей WiFi
int activeNetwork = 0;
bool F_init_sta = 0;
bool F_added_event_sta = 0;
bool F_init_ap = 0;
bool F_added_event_ap = 0;
String Connected_WiFi = "xxx.xxx.xxx.xxx";

#include <ESP32Time.h>
ESP32Time rtc;
const char* ntpServer = "ntp.msk-ix.ru";
const int gmtOffset_sec = 10800;
const int daylightOffset_sec = 0;

#include <EEPROM.h>
#include <LittleFS.h> // подключить библиотеку файловой системы (до #include GyverPortal)
#include <GyverPortal.h>
GyverPortal portal(&LittleFS);  // передать ссылку на fs (SPIFFS/LittleFS)
#define PLOT_SIZE 170
int16_t arr[2][PLOT_SIZE];
uint32_t dates[PLOT_SIZE];
const char *names[] = {"вход", "выход",};

GPdate valDate;
GPtime valTime;
String valDataTime = "04.11.2023 12:00";
bool update_run_write = 0;
bool soundRun = 0;
int indoor_period_record = 0;  // счетчик периода 10 мин
int outdoor_period_record = 0; // счетчик периода 10 мин
uint64_t chip;                 // chip id
bool F_clean_day = 0;          // флаг очистки счетчика в 00:00
bool F_alert = 0;
bool F_alert_save = 0;
bool F_Call = 0;
bool F_exit = 0;
int distanse_zone1 = 0;
int distanse_zone2 = 0;

String ArrPachFileName[MAX_COUNT_FILES]; // Имена файлов в памяти
int FileIsActive = 0;                    // Номер активного файла из памяти
String arrDateTime[750];
int arrValue[750][4];
int count = 0;
int countDay = 0;
int tempVal_1 = 0;
int tempVal_2 = 0;
int countRowsFile = 0;

enum { // режим работы светодиода
  NORMAL_MODE,
  WAITING_MODE,
  SETTINGS_MODE,
  ERROR_MODE,
  UPDATE_MODE,
};

struct LoginPass {  
  int  initKey;           // ключ флаг первого запуска устройства
  bool F_tof_callibration;// 
  uint64_t ID_SERIAL1;    // первый ключ
  int  type_ui;           // Тип отображаемого интефейса (1 user) (2 admin) 
  char ssid_sta[50];      // Имя точки доступа для подключения в режиме STA
  char pass_sta[20];      // Пароль точки доступа для подключения в режиме STA
  char ssid_ap[50];       // Имя точки доступа в режиме AP
  char pass_ap[20];       // Пароль точки доступа в режиме AP
  char web_login[50];     // Логин для входа в WEB
  char web_password[20];  // Пароль для входа в WEB
  char ssid_hub_enow[50]; // Имя SSID к которой подключен HUB для вычисления канала ESP NOW
  int  Led_mode;          // режим работы светодиода  
  uint64_t ID_SERIAL2;    // второй ключ
  uint32_t key_3; 
  bool SW_STA;            // Вкл/Выкл режим WiFi STA
  bool SW_AP;             // Вкл/Выкл режим WiFi AP
  bool SW_ESP_NOW;        // Вкл/Выкл ESP NOW
  bool SW_Tx_BD;          // Вкл/Выкл сохранение в БД
  bool SW_Tx_SD;          // Вкл/Выкл сохранение на SD
  bool SW_Tx_API;         // Вкл/Выкл сохранение по API
  bool SW_CLEAN_DAY;      // Вкл/Выкл сброс суточного счетчика
  bool SW_SOUND;          // Вкл/Выкл звукового сопровождения
  bool SW_REVERS;         // Вкл/Выкл инверсию зон вход выход
  bool SW_REVERS_INV;     // Отображение направления счета вход - выход 
  char API_HOST[60];      // Адрес HOST API
  char DB_HOST[60];       // Учетные данные для записи в БД   
  char DB_NAME[50];       // Учетные данные для записи в БД   
  char DB_USER[20];       // Учетные данные для записи в БД   
  char DB_PASS[20];       // Учетные данные для записи в БД   
  char locate1[20];       // Место установки "помещение"
  char locate2[20];       // Место установки "адрес"
  char locate3[20];       // Место установки "дверь"        
  int Count_IN;           // Счетчик входа  
  int Count_OUT;          // Счетчик выхода 
  int Zone_MaxThreshold;  // Расстояние до объекта (больше него игнорируется)
  int Zone_MinThreshold;  // Расстояние до объекта (меньше него игнорируется)  
  int ZoneX;              // ширина зоны (4-15)
  int ZoneY;              // Высота зоны (4-8)  
  int Z1_ROI;             // Зона 1 оптический центр 
  int Z2_ROI;             // Зона 2 оптический центр 
  uint32_t key_1;     
  int MacAdressMaster[6]; // mac адрес мастера
  int ThisMacAdress[6];   // mac адрес этого устройства 
  char Text_This_Mac_Hex[40];
  char Text_This_Mac_Dec[40];
  uint16_t key_4;
  int SensorOffset;       // калибровочная константа сенсора
  int SensorXTalk;        // калибровочная константа сенсора
  int DistanseCallReal;   // дистанция от датчика до препятствия для калибраовки
  bool update_sw;         // флаг для обновления после перезагрузки, если произошли max_err неудачных попыток
  bool reboot_today;      // перезагрузка сегодня 1 была 0 не было
};
LoginPass secret;

struct data_waiting_t { // структура циклического буфера ожмдания отправки
  bool status_send_sd  = 1; // 0 - есть неотправленные данные      
  bool status_send_api = 1; // 0 - есть неотправленные данные
  bool status_send_bd  = 1; // 0 - есть неотправленные данные
  int status_code_api  = 0; // http code status (201,-1,-11,404...)
  int status_code_bd   = 0; // http code status (200,-1,-11,404...)
  
  String status_echo_api;  // "error" или "success"
  String machine_id;       // идентификатор устройства
  int counter_in;          // Количество за период входов
  int counter_out;         // Количество за период выходов
  String name;             // название помещения
  String address;          // адрес помещения
  String door;             // название двери
  String date_time;        // "2023-01-17 16:50" 
  String date_time_unux;   // 1695309804
};
data_waiting_t Data_Waiting[MAX_DATA_WAITING]; // Создайте структуру и получите указатель на нее
int counter_waiting = 0; // для формирования циклического буфера из структуры 
int num_string_now = -1; // номер строки для отправки или записи сейчас ("-1" это запись не требуется)

//-----------------------------------------Прототипы функций--------------------------------------------------------

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFi_AP_Start(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFi_AP_Connected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFi_AP_Disconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void INIT_STA_WiFi();
void INIT_AP_WiFi();
void IRAM_ATTR OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void INIT_SLAVE_ESP_NOW();
void boardInfo();
byte crc8_bytes(byte *buffer, byte size);
String displayFS();
String getValue(String data, char separator, int index);
void scanNetwork();
void save_data_period_in_queue();
void INIT_DEVAULT_SETTING();
int findValueIndex(int rowCount);
void DrowDatePlot(int index);
void readAndParseData(fs::FS &fs, const char * path);

//--------------------------------------Работа WEB интерфейса--------------------------------------------------------

void web_page_admin(){
  GP.BUILD_BEGIN(1100);
  GP.THEME(GP_DARK);
  GP.ALERT("alert");
  GP.UPDATE("Conn_WiFi,distanse_a,distanse_b");  // список имён компонентов на обновление
  //GP.RELOAD_CLICK("btnScan");
  GP.GRID_RESPONSIVE(700);   // позволяет "отключить" таблицу при ширине экрана меньше 700px
  GP.setReloadTimeout(10000); // задержка перезагрузки страницы для SELECT и BUTTON при включенном rel
  
  M_GRID(
     GP_MAKE_BLOCK_THIN_TAB(
      "Конфигурация системы",
      M_BOX(GP.FORM_BEGIN("/sysconf"););    
      M_BOX(GP.LABEL("Логин  WEB"); GP.TEXT("log_web", secret.web_login, secret.web_login); );   
      M_BOX(GP.LABEL("Пароль WEB"); GP.PASS("psw_web", "password", secret.web_password); ); 
      GP.BREAK();     
      GP.HR();
      GP.BREAK();
      M_BOX(GP.LABEL("Сбрасывать счетчик "); GP.SWITCH("swclean", secret.SW_CLEAN_DAY););
      M_BOX(GP.LABEL("Звуковое оповещение "); GP.SWITCH("swsound", secret.SW_SOUND););      
      M_BOX(GP.LABEL("Изменить направление");GP.SWITCH("sw_direction", secret.SW_REVERS););
      //M_BOX(GP.LABEL("Поменять вход-выход"); GP.SWITCH("swrevers", secret.SW_REVERS););
      M_BOX(GP.LABEL("Сохранять данные SD "); GP.SWITCH("swsd", secret.SW_Tx_SD););
      GP.BREAK();
      M_BOX(GP_CENTER, GP.SUBMIT("Сохранить"););
      M_BOX(GP.FORM_END(););   
    );
     GP_MAKE_BLOCK_THIN_TAB(      
      "Конфигурация WiFi",
      M_BOX(GP.FORM_BEGIN("/wificonf"););       
      //M_BOX(GP.BUTTON_MINI("btnScan", "Scan"); );
      M_BOX(GP.LABEL("Клиент"); GP.SELECT("wifi_sta", networkFound, activeNetwork); );
      M_BOX(GP.LABEL("Пароль"); GP.PASS("wifi_pswSta", "password", secret.pass_sta); GP.BUTTON_MINI("btnScan", "Scan","","","",0,1);); 
      M_BOX(GP.LABEL("Включить"); GP.SWITCH("swsta", secret.SW_STA););
      GP.HR(); 
      GP.BREAK();
      M_BOX(GP.LABEL("Точка"); GP.TEXT("wifi_ap", secret.ssid_ap, secret.ssid_ap); );   
      M_BOX(GP.LABEL("Пароль"); GP.PASS("wifi_pswAp", "password", secret.pass_ap); );
      M_BOX(GP.LABEL("Включить"); GP.SWITCH("swap", secret.SW_AP););
      GP.BREAK();     
      M_BOX(GP_CENTER, GP.SUBMIT("Сохранить"););
      M_BOX(GP.FORM_END(););
        
    );
     GP_MAKE_BLOCK_THIN_TAB(
      "Соединение с облаком",
      M_BOX(GP.FORM_BEGIN("/cloud");); 
      M_BOX(GP.LABEL("HOST GET"); GP.TEXT("hostSQL", secret.DB_HOST, secret.DB_HOST); );  
      M_BOX(GP.LABEL("Отправлять GET "); GP.SWITCH("swbd", secret.SW_Tx_BD););
      M_BOX(GP.LABEL("HOST API"); GP.TEXT("hostAPI", secret.API_HOST, secret.API_HOST); );
      M_BOX(GP.LABEL("Отправлять JSON "); GP.SWITCH("swapi", secret.SW_Tx_API);); 
      M_BOX(GP.LABEL("Название"); GP.TEXT("locate1", secret.locate1, secret.locate1); );
      M_BOX(GP.LABEL("Адрес"); GP.TEXT("locate2", secret.locate2, secret.locate2); );
      M_BOX(GP.LABEL("Вход"); GP.TEXT("locate3", secret.locate3, secret.locate3); ); 
      GP.BREAK();      
      M_BOX(GP_CENTER, GP.SUBMIT("Сохранить"););
      M_BOX(GP.FORM_END();); 
    );
  );
  M_GRID(
     GP_MAKE_BLOCK_THIN_TAB(
     "Соединение с HUB", 
      M_BOX(GP.FORM_BEGIN("/espnow"););  
      M_BOX(GP.LABEL("WiFi HUB"); GP.SELECT("ssid_hub", networkFound); );
      GP.BREAK();      
      GP.HR();  
      GP.BREAK(); 
      M_BOX(
          GP.NUMBER("mac1_0", "", secret.MacAdressMaster[0], "25%", false);
          GP.NUMBER("mac1_1", "", secret.MacAdressMaster[1], "25%", false);
          GP.NUMBER("mac1_2", "", secret.MacAdressMaster[2], "25%", false);                   
          );
      M_BOX(
          GP.NUMBER("mac1_3", "", secret.MacAdressMaster[3], "25%", false);
          GP.NUMBER("mac1_4", "", secret.MacAdressMaster[4], "25%", false);
          GP.NUMBER("mac1_5", "", secret.MacAdressMaster[5], "25%", false);          
          );      
      M_BOX(GP.LABEL("Включить"); GP.SWITCH("swespnow", secret.SW_ESP_NOW););
      GP.BREAK();
      GP.HR();  
      GP.BREAK();  
      GP.BREAK();   
      M_BOX(GP_CENTER, GP.SUBMIT("Сохранить"););
      M_BOX(GP.FORM_END(););
    );
    GP_MAKE_BLOCK_THIN_TAB(    
      "Настройка сенсора",
      M_BOX(GP.FORM_BEGIN("/zones"););
      //M_BOX(GP.LABEL("Расстояние калибровки"); GP.NUMBER("Distanse","",secret.DistanseCallReal););
      M_BOX(GP.LABEL("Расстояние мин"); GP.NUMBER("h_min","",secret.Zone_MinThreshold););
      M_BOX(GP.LABEL("Расстояние макс"); GP.NUMBER("h_max","",secret.Zone_MaxThreshold););
      M_BOX(GP.LABEL("Текущее значение A"); GP.NUMBER("distanse_a","",distanse_zone1););
      M_BOX(GP.LABEL("Текущее значение B"); GP.NUMBER("distanse_b","",distanse_zone2););   
      GP.BREAK(); 
      GP.BREAK();
      GP.BREAK(); 
      GP.BREAK(); 
      GP.BREAK(); 
      GP.BREAK(); 
      GP.BREAK();            
      M_BOX(GP_CENTER, GP.SUBMIT("Сохранить"););
      //M_BOX(GP_CENTER, GP.BUTTON("btnCallsens", "Запустить калибровку","","","",0,1);); 
      M_BOX(GP.FORM_END(););   
    );

    
   GP_MAKE_BLOCK_THIN_TAB(
      "Обновление ПО",
      GP.BREAK(); 
      GP.BREAK();      
      M_BOX(GP_CENTER, GP.OTA_FIRMWARE(); );
      GP.BREAK();  
      GP.BREAK();     
      GP.HR();
      GP.BREAK();
      if (secret.SW_STA == 0){GP.BREAK();}  
      if (secret.SW_STA == 1){GP_MAKE_BOX(GP_CENTER, GP.LABEL("IP connected "); GP.LABEL(Connected_WiFi,"Conn_WiFi"); );}    
      M_BOX(GP_CENTER, GP.LABEL("MAC"); GP.LABEL(String(secret.Text_This_Mac_Dec)););
      M_BOX(GP_CENTER, GP.LABEL("SN "); GP.LABEL(String(chip));); 
      M_BOX(GP_CENTER, GP.LABEL(VERSION_SW); );
      GP.BREAK();
      GP.BREAK();
      GP.HR();  
      GP.BREAK();
      GP.BUTTON("btnReboot", "Завершить настройку"); 
    );
  );
  GP.BUILD_END();
}

void web_page_user() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  //GP.THEME(GP_LIGHT);
  GP.UPDATE("countlb1,countlb2,datetime");   // список имён компонентов на обновление  


  if (portal.uri("/timedata")) {
    GP_MAKE_BLOCK_THIN_TAB(
   "Системные часы", 
   M_BOX(GP.FORM_BEGIN("/timedata");); 
   M_BOX(GP.LABEL("Дата"); GP.DATE("date", valDate); );
   M_BOX(GP.LABEL("Время"); GP.TIME("time", valTime); );
   M_BOX(GP_CENTER, GP.SUBMIT("Сохранить"););
   M_BOX(GP.FORM_END(););   
   );
   GP.BUTTON_LINK("/", "Назад");
  }

   else if (portal.uri("/config")) {
    GP_MAKE_BLOCK_THIN_TAB(
   "Переход к настройкам", 
   M_BOX(GP.FORM_BEGIN("/config");); 
   //M_BOX(GP.LABEL("Дата"); GP.DATE("date", valDate); );
   //M_BOX(GP.LABEL("Время"); GP.TIME("time", valTime); );
   M_BOX(GP_CENTER, GP.SUBMIT("Да, перезагрузить"););
   M_BOX(GP.FORM_END(););   
   );
   GP.BUTTON_LINK("/", "Назад");
   }

   else if (portal.uri("/plot")) {
   // Выборка данных 
   //Serial.print("График из файла ");
   //Serial.println(ArrPachFileName[FileIsActive]);

   char buffname[50];
   ArrPachFileName[FileIsActive].toCharArray(buffname, 50);    
   readAndParseData(LittleFS, buffname);

   GP.PLOT_STOCK_DARK<2, PLOT_SIZE>("graphics", names, dates,  arr);
   GP.BUTTON_LINK("/", "Назад");
   }  
    
    else{
    GP_MAKE_BLOCK_THIN_TAB(
    "Оперативные данные",    
    M_BOX(GP.LABEL("Систем. часы  "); GP.LABEL(String(valDataTime),"datetime"); ); 
    GP.BREAK();
    GP.HR();  
    GP.BREAK();  
    M_BOX( GP.LABEL("Вход"); GP.LABEL(String(secret.Count_IN),"countlb1"); );
    M_BOX( GP.LABEL("Выход "); GP.LABEL(String(secret.Count_OUT),"countlb2"); );  
    GP.BREAK(); 
    GP.HR();  
    //GP.BREAK();   
    M_BOX(GP.LABEL("Звуковое оповещение"); GP.SWITCH("swsound", secret.SW_SOUND););
    M_BOX(GP.LABEL("Сброс счетчика в 00:00 "); GP.SWITCH("swclean", secret.SW_CLEAN_DAY););
    ); 

    GP_MAKE_BLOCK_THIN_TAB(
    "Файлы статистики",  
    //GP.SEND(displayFS());  // файловый менеджер 
    int countFiles = 0;     
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
      if (!file.isDirectory()) {
        String nameFile = file.name();
        String pachFileName = "/";
        pachFileName += nameFile; 
        ArrPachFileName[countFiles] = pachFileName;
        if(secret.SW_AP == 0){M_BOX( GP.BUTTON_MINI_LINK("/plot", "Открыть",GP_GREEN,"",String(countFiles));  GP.BOLD(nameFile); GP.BUTTON_MINI_DOWNLOAD(pachFileName, "Скачать"););} // подключен к сети то строится график
        if(secret.SW_AP == 1){M_BOX( GP.BUTTON_MINI_LINK(pachFileName, "Открыть");  GP.BOLD(nameFile); GP.BUTTON_MINI_DOWNLOAD(pachFileName, "Скачать"););}               // Если в режиме точки доступа только открыть файл
     }
    file = root.openNextFile();
    countFiles ++;
    }    
    );   
    if (secret.SW_AP == 1){GP.BUTTON_LINK("/timedata", "Установить время");} // Отображать кнопку только если нет подключения к WiFi сети    
    
    GP_MAKE_BLOCK_THIN_TAB(
    "О системе",    
      M_BOX(GP_CENTER, GP.LABEL(DEVICE_NAME); );
      M_BOX(GP_CENTER, GP.LABEL(VERSION_SW); ); 
      GP.BREAK();                   
      M_BOX(GP_CENTER, GP.LABEL("MAC"); GP.LABEL(String(secret.Text_This_Mac_Dec)););
      M_BOX(GP_CENTER, GP.LABEL("SN "); GP.LABEL(String(chip));); 
      
    ); 
  }
  
  GP.BUILD_END();
}

void action(){  

if (portal.form("/config")){     // Загрузиться в ADMIN UI         
    secret.type_ui = 2;      // выбираем admin интерфейс    
    EEPROM.put(0, secret);   // сохраняем
    EEPROM.commit();         // записываем
    WiFi.disconnect();       // отключаем WiFi
    Init_Task15();           // перезагружаемся
  } 
       
if (portal.form("/sysconf")) {   // Параметры системы           
    portal.copyStr("log_web", secret.web_login, sizeof(secret.web_login));    
    portal.copyStr("psw_web", secret.web_password, sizeof(secret.web_password)); 
    portal.copyBool("swclean", secret.SW_CLEAN_DAY);
    portal.copyBool("swsound", secret.SW_SOUND);    
    portal.copyBool("sw_direction", secret.SW_REVERS);
    portal.copyBool("swsd", secret.SW_Tx_SD); 
    F_alert_save = 1;       
    EEPROM.put(0, secret);                     
    EEPROM.commit();                                    
    } 
     
if (portal.form("/wificonf")) {  // Параметры WiFi
    activeNetwork = 0;             
    portal.copyInt("wifi_sta", activeNetwork); // копируем себе    
    networkFound[activeNetwork].toCharArray(secret.ssid_sta, 50);
    //networkFound[activeNetwork].toCharArray(secret.ssid_selected, 50);
    portal.copyStr("wifi_pswSta", secret.pass_sta, sizeof(secret.pass_sta)); 
    portal.copyBool("swsta", secret.SW_STA);
    portal.copyStr("wifi_ap", secret.ssid_ap, sizeof(secret.ssid_ap)); 
    portal.copyStr("wifi_pswAp", secret.pass_ap, sizeof(secret.pass_ap));      
    portal.copyBool("swap", secret.SW_AP);
    if (secret.SW_STA == 1 && F_init_sta==0){INIT_STA_WiFi(); /*Serial.println("INIT_STA_WiFi()")*/;}
    if (secret.SW_STA == 0){secret.SW_AP = 1; WiFi.disconnect(); F_init_sta=0; Connected_WiFi = "xxx.xxx.xxx.xxx"; /*Serial.println("WiFi.disconnect()")*/;}
    F_alert_save = 1;
    EEPROM.put(0, secret);                      
    EEPROM.commit(); 
    }    

if (portal.form("/cloud")) {     // Параметры соединения с БД и API            
    portal.copyStr("hostSQL", secret.DB_HOST, sizeof(secret.DB_HOST));
    portal.copyBool("swbd", secret.SW_Tx_BD);
    portal.copyStr("hostAPI", secret.API_HOST, sizeof(secret.API_HOST));    
    portal.copyBool("swapi", secret.SW_Tx_API);
    portal.copyStr("locate1", secret.locate1, sizeof(secret.locate1));    
    portal.copyStr("locate2", secret.locate2, sizeof(secret.locate2));
    portal.copyStr("locate3", secret.locate3, sizeof(secret.locate3)); 
    #if (ENABLE_DEBUG_ACTION_WEB == 1)
     Serial.println(); 
     Serial.print("secret.SW_Tx_BD: ");
     Serial.println(secret.SW_Tx_BD);
     Serial.print("secret.SW_Tx_API: ");
     Serial.println(secret.SW_Tx_API);
     Serial.print("secret.DB_HOST: ");
     Serial.println(secret.DB_HOST);
     Serial.print("secret.API_HOST: ");
     Serial.println(secret.API_HOST);
     Serial.println();
    #endif
    F_alert_save = 1;
    EEPROM.put(0, secret);                      
    EEPROM.commit();                           
    }

if (portal.form("/espnow")) {    // Параметры ESP NOW  
    int num_ssid = 0;             
    portal.copyInt("ssid_hub", num_ssid); // копируем себе    
    networkFound[num_ssid].toCharArray(secret.ssid_hub_enow, 50);            
    portal.copyInt("mac1_0", secret.MacAdressMaster[0]); 
    portal.copyInt("mac1_1", secret.MacAdressMaster[1]);
    portal.copyInt("mac1_2", secret.MacAdressMaster[2]);
    portal.copyInt("mac1_3", secret.MacAdressMaster[3]);
    portal.copyInt("mac1_4", secret.MacAdressMaster[4]);
    portal.copyInt("mac1_5", secret.MacAdressMaster[5]);
    portal.copyBool("swespnow", secret.SW_ESP_NOW); 
    F_alert_save = 1;       
    EEPROM.put(0, secret); // сохраняем
    EEPROM.commit();       // записываем           
    }

if (portal.form("/zones")) {     // Параметры зон TOF сенсора             
    portal.copyInt("h_min", secret.Zone_MinThreshold);    
    portal.copyInt("h_max", secret.Zone_MaxThreshold);
    portal.copyInt("Distanse", secret.DistanseCallReal);
    //portal.copyInt("x_size", secret.ZoneX);
    //portal.copyInt("y_size", secret.ZoneY);
    //portal.copyInt("roi_a", secret.Z1_ROI);
    //portal.copyInt("roi_b", secret.Z2_ROI); 
    F_alert_save = 1;        
    EEPROM.put(0, secret);                     
    EEPROM.commit(); 
    //Init_Task14();
    }

if (portal.form("/timedata")) {  // Установка времени из WEB интерфейса              
    portal.copyDate("date", valDate);    // копируем себе
    portal.copyTime("time", valTime);    // копируем себе
    String T_time = valTime.encode();
    String T_date = valDate.encode();
    //Serial.println("Кнопка Time");
    //Serial.println(T_time); 
    //Serial.println(T_date);
    String tmp = "";
    tmp = getValue(T_time, ':', 0);
    int hh = tmp.toInt();    
    tmp = getValue(T_time, ':', 1);
    int mn = tmp.toInt();
    tmp = getValue(T_time, ':', 2); 
    int ss = tmp.toInt(); 
    
    tmp = getValue(T_date, '-', 0);
    int yyyy = tmp.toInt();     
    tmp = getValue(T_date, '-', 1);
    int mm = tmp.toInt();
    tmp = getValue(T_date, '-', 2);
    int dd = tmp.toInt(); 
    rtc.setTime(ss, mn, hh, dd, mm, yyyy);

    secret.Led_mode = NORMAL_MODE;
    F_alert_save = 1;
                    
    //Serial.println(T_time); // 04:05:00
    //Serial.println(T_date); // 2023-01-18    
    //Serial.println(getValue(T_time, ':', 0)); // 04
    //Serial.println(getValue(T_time, ':', 1)); // 05
    //Serial.println(getValue(T_time, ':', 2)); // 00    
    //Serial.println(getValue(T_date, '-', 0)); // 2023
    //Serial.println(getValue(T_date, '-', 1)); // 01
    //Serial.println(getValue(T_date, '-', 2)); // 18 
    //rtc.setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30
    }

if (portal.click("swsound")){    // Установка вкл/выкл звука
    portal.copyBool("swsound", secret.SW_SOUND);
    EEPROM.put(0, secret);  // сохраняем
    EEPROM.commit();        // записываем
    //Serial.print("Переключатель звука ");              
    //Serial.println(secret.SW_SOUND);      
  }

if (portal.click("swclean")){    // Установка вкл/выкл звука
    portal.copyBool("swclean", secret.SW_CLEAN_DAY);
    EEPROM.put(0, secret);  // сохраняем
    EEPROM.commit();        // записываем
    //Serial.print("Переключатель сброс суточного счетчика ");              
    //Serial.println(secret.SW_CLEAN_DAY);      
  }

if (portal.click("swespnow")){   // Установка вкл/выкл ESP NOW
    portal.copyBool("swespnow", secret.SW_ESP_NOW);
    EEPROM.put(0, secret);  // сохраняем
    EEPROM.commit();        // записываем
    //Serial.print("Переключатель ESP NOW ");              
    //Serial.println(secret.SW_ESP_NOW);      
  }

if (portal.click("btnScan")){    // Сканирование WiFi сетей
    //Serial.print("Сканирование...");
    F_alert = 1;
    secret.SW_STA = 0;
    WiFi.disconnect(); 
    F_init_sta=0; 
    Connected_WiFi = "xxx.xxx.xxx.xxx"; 
    //Serial.println("WiFi.disconnect()");
    EEPROM.put(0, secret);  // сохраняем
    EEPROM.commit();        // записываем
    Init_Task13();
    //scanNetwork();
    //EEPROM.put(0, secret);   // сохраняем
    //EEPROM.commit();         // записываем
    //WiFi.disconnect();       // отключаем WiFi
    //ESP.restart();           // перезагружаемся
  }

if (portal.click("btnCallsens")){  // Колибровка сенсора
    //vTaskSuspend(Task1);
    //vTaskSuspend(Task2);
    //Serial.println("Остановка основных задач по опросу сенсора");
    //Serial.print("Калибровка сенсора запущена на расстоянии ");
    F_Call = 1;
    //portal.copyInt("Distanse", secret.DistanseCallReal); 
    //Serial.println(secret.DistanseCallReal);   
    Init_Task14();    
  }

if (F_alert && portal.update("alert")){
      F_alert = 0;
      portal.answer("Сканирование WiFi сетей занимает до 10 секунд. По окончании WEB страница перезагрузится");
}

if (F_exit && portal.update("alert")){
      F_exit = 0;
      portal.answer("Конфигурация завершена. Применяются параметры и устройство будет перезагружено.");
}

if (F_alert_save && portal.update("alert")){
      F_alert_save = 0;
      portal.answer("Сохранение данных успешно");
}

if (F_Call && portal.update("alert")){
      F_Call = 0;      
      String text = "Запущен процесс калибровки сенсора с препядствием на расстоянии ";
      //portal.copyInt("Distanse", secret.DistanseCallReal);
      text += String(secret.DistanseCallReal);
      text += " мм. По окончании устройство будет перезагружено.";
      portal.answer(text);
}
  
if (portal.click("btnReboot")){  // Настройка закончена загрузиться в USER UI
    F_exit = 1; 
    secret.type_ui = 1;      // выбираем user интерфейс
    EEPROM.put(0, secret);   // сохраняем
    EEPROM.commit();         // записываем
    WiFi.disconnect();       // отключаем WiFi
    Init_Task15();           // перезагружаемся
  }

if (portal.click("0")){    
   FileIsActive = 0; 
  }
if (portal.click("1")){    
   FileIsActive = 1; 
  } 
if (portal.click("2")){    
   FileIsActive = 2; 
  }
if (portal.click("3")){    
   FileIsActive = 3; 
  } 
if (portal.click("4")){    
   FileIsActive = 4; 
  }
     
if (portal.update()){    
    portal.updateInt("countlb1", secret.Count_IN);
    portal.updateInt("countlb2", secret.Count_OUT);    
    portal.updateString("datetime", valDataTime); 
    portal.updateString("Conn_WiFi", Connected_WiFi);
    portal.updateInt("distanse_a", distanse_zone1);
    portal.updateInt("distanse_b", distanse_zone2);
  }
}

//-----------------------------------------Задачи FreeRTOS-----------------------------------------------------------

/*
Задача  Ядро  Приоритет Описание
Task1     0       5     Функция работы с TOF сенсором
Task2     0       4     Функция загрузки данных с tof в основную программу
Task3     1       4     Функция записи лога в файл, БД, API
Task4     1       5     Синхронизация даты времени с NTP-сервера
Task5     0       3     Таймер для записи лога в файл и БД
Task6     0       2     Отслеживание кнопки сброса
Task7     1       2     Отправка данных ESP NOW
Task8     1       2     Watch Dog обмена данными ESP NOW
Task9     1       2     Функция для работы зуммера
Task10    1       2     Функция для работы светодиода
Task11    1       2     Отслеживание состояния буфера неотправленный/незаписанных данных
Task12    1       1     Обновление строковой переменной времени для Web интерфейса
Task13    0       2     Сканирование WiFi
Task14    0       0     Калибровка TOF сенсора
Task15    0       1     Перезагрузка контроллера
Task16    0       1     Обработка приема данных от Serial
Task17    1       1     Отложенная задача для обновления прошивки с сервера
*/

void Task1code( void * pvParameters ){  // Функция работы с TOF сенсором
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task1code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
  pinMode(shut_tof_pin, OUTPUT);
  digitalWrite(shut_tof_pin, HIGH);
  pinMode(int_tof_pin, INPUT);
  myTofSensor.setPersonThreshold(secret.Zone_MaxThreshold);    // Расстояние до головы человека
  myTofSensor.setDoorThreshold(secret.Zone_MinThreshold);      // Расстояние до начала двери
  myTofSensor.setZoneArea(secret.ZoneX, secret.ZoneY);         // Размер зон
  myTofSensor.setOpticalCenters(secret.Z1_ROI, secret.Z2_ROI); // Центра зон  
  bool stat;
  if(secret.F_tof_callibration==1){stat = myTofSensor.setup(secret.SensorOffset,secret.SensorXTalk);} // Инициализация сенсора с заданной калибровкой
  if(secret.F_tof_callibration==0){stat = myTofSensor.setup();} // Инициализация сенсора без калибровки  
  if (stat == false){secret.Led_mode = ERROR_MODE;}
  peopleCounter.setSensor(&myTofSensor);
  vTaskDelay(5000/portTICK_PERIOD_MS);
   
  for(;;){
  xSemaphoreTake(sensor_mutex, portMAX_DELAY);
  myTofSensor.update();
  xSemaphoreGive(sensor_mutex);
  peopleCounter.update();
  vTaskDelay(1/portTICK_PERIOD_MS);    
  }
}

void Init_Task1(){
  xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task1code,   /* Функция задачи. */
               "Task1",     /* Ее имя. */
               10000,       /* Размер стека функции */
               NULL,        /* Параметры */
               5,           /* Приоритет */
               &Task1,      /* Дескриптор задачи для отслеживания */
               0);          /* Указываем пин для данного ядра */                  
  delay(10);  
  }

void Task2code(void * pvParameters) {  // Функция загрузки данных с tof в основную программу
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task2code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
   
  for(;;){ 
    
  int in = 0;
  int out = 0;    
  if(secret.SW_REVERS == 0){in = peopleCounter.getcounter_indoor(); out = peopleCounter.getcounter_outdoor();}     
  if(secret.SW_REVERS == 1){out = peopleCounter.getcounter_indoor(); in = peopleCounter.getcounter_outdoor();}

  distanse_zone1 = myTofSensor.getDistanse_Z1();
  distanse_zone2 = myTofSensor.getDistanse_Z2();
      
  if(in > secret.Count_IN){
    int in_period = in - secret.Count_IN; 
    indoor_period_record += in_period;
    if (secret.SW_SOUND == 1){soundRun = 1;} // Звук при детекции входа
    //Serial.println(in_period);       
    };
  if(out > secret.Count_OUT){
    int out_period = out - secret.Count_OUT; 
    outdoor_period_record += out_period;
    //if (secret.SW_SOUND == 1){soundRun = 1;} // Звук при детекции выхода 
    //Serial.println(out_period);   
    };  
  secret.Count_IN = in;
  secret.Count_OUT = out;
  vTaskDelay(500/portTICK_PERIOD_MS);    
  }
}

void Init_Task2(){
  xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task2code,   /* Функция задачи. */
               "Task2",     /* Ее имя. */
               4096,        /* Размер стека функции */
               NULL,        /* Параметры */
               4,           /* Приоритет */
               &Task2,      /* Дескриптор задачи для отслеживания */
               0);          /* Указываем пин для данного ядра */                  
  delay(10);  
  }

void Task3code( void * pvParameters ){  // Функция записи лога в файл, БД, API
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task3code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
  
  int httpResponseCode = 0;
  
  
  for(;;){      

  if(update_run_write == 1)
    { 
    if(secret.SW_AP == 0 && secret.SW_Tx_BD == 1 && Data_Waiting[num_string_now].status_send_bd==0){
      Data_Waiting[num_string_now].status_send_bd=1;
      xSemaphoreTake(wifi_mutex, portMAX_DELAY);

      String server = String(secret.DB_HOST); 
      String chipidstr = Data_Waiting[num_string_now].machine_id; 
      String rtc_Time = Data_Waiting[num_string_now].date_time;
      String rtc_time = "";
      String rtc_date = "";
        
      rtc_date = getValue(rtc_Time, ' ', 0);
      rtc_date += "%20";
      rtc_time = getValue(rtc_Time, ' ', 1);           
      rtc_Time = rtc_date;
      rtc_Time += rtc_time;     
      
      #if (ENABLE_DEBUG_GET == 1)
       Serial.print("Коррекция date_time: ");
       Serial.println(rtc_Time);           
      #endif 
      
      String httpRequestData = server;         
      httpRequestData += "?machine_id="+chipidstr+"&counter_in=0"+String(Data_Waiting[num_string_now].counter_in)+"&counter_out=0"+String(Data_Waiting[num_string_now].counter_out)+"&date_time="+rtc_Time;
            
      #if (ENABLE_DEBUG_GET == 1)
       Serial.print("httpRequestData ");
       Serial.println(httpRequestData);           
      #endif
      
      HTTPClient http;
      httpResponseCode = 0;      
      http.begin(httpRequestData);  
      httpResponseCode = http.GET(); // Отправляем запрос
      Data_Waiting[num_string_now].status_code_bd = httpResponseCode;
             
      #if (ENABLE_DEBUG_GET == 1)       
        if (httpResponseCode > 0) {                   
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);                  
        } else {          
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);                
        } 
        #endif       
        http.end();  // Освобождаем память
        xSemaphoreGive(wifi_mutex);          
     } 

    if (secret.SW_AP == 0 && secret.SW_Tx_API == 1 && Data_Waiting[num_string_now].status_send_api==0) {  
      Data_Waiting[num_string_now].status_send_api=1;
      xSemaphoreTake(wifi_mutex, portMAX_DELAY);
      
      char ServerName[sizeof(secret.API_HOST)];
      String server = String(secret.API_HOST); 
      server.toCharArray(ServerName, sizeof(secret.API_HOST)); 
      
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient http;
      http.setTimeout(1000);
      http.setConnectTimeout(1000);
      
      char buffer[400];      
      JsonDocument  DocumentJson;  

      /*Структура отправки данных:
      {
        "machine_id": 88268765765608,
        "counter_in": "0",
        "counter_out": "2", 
        "name":   "доп.информация поле не обязательное",  // до 20 знаков
        "address": "доп.информация поле не обязательное", // до 20 знаков
        "door":   "доп.информация поле не обязательное",  // до 20 знаков
        "date_time": "2023-01-17 16:50" 
      }*/  
      
      DocumentJson["machine_id"]  = Data_Waiting[num_string_now].machine_id;
      DocumentJson["counter_in"]  = Data_Waiting[num_string_now].counter_in;
      DocumentJson["counter_out"] = Data_Waiting[num_string_now].counter_out;
      DocumentJson["name"] = Data_Waiting[num_string_now].name;
      DocumentJson["address"] = Data_Waiting[num_string_now].address;
      DocumentJson["door"] = Data_Waiting[num_string_now].door;
      DocumentJson["date_time"] = Data_Waiting[num_string_now].date_time;
      serializeJson(DocumentJson, buffer); // Преобразование DocumentJson в массив символов buffer[400]

      #if (ENABLE_DEBUG_JSON == 1)
      Serial.println(); 
      Serial.print("ServerName: ");
      Serial.println(ServerName);      
      Serial.println(); 
      Serial.print("buffer: ");
      Serial.println(buffer);
      Serial.println();
      #endif
      
      http.begin(client, ServerName); 
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(buffer);
      Data_Waiting[num_string_now].status_code_api = httpResponseCode;  

      #if (ENABLE_DEBUG_JSON == 1) 
      serializeJsonPretty(DocumentJson, Serial);
      Serial.println(); 
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      #endif
       
      if(httpResponseCode == HTTP_CODE_CREATED) {
            String ResponseText = http.getString();
            #if (ENABLE_DEBUG_JSON == 1) 
            Serial.println(ResponseText);
            #endif 
                    
            /* Ожидаемый ответ:
               httpResponseCode равен  HTTP_CODE_CREATED (код 201) 
                 {
                   "status": "success"                  
                  }
                */ 
            
            JsonDocument Json;
            DeserializationError error = deserializeJson(Json, ResponseText);
            if (error) {
              Data_Waiting[num_string_now].status_echo_api = "error";
              #if (ENABLE_DEBUG_JSON == 1)
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              #endif
              }
            if (!error) {             
            const char* status_api = Json["status"];             
            Data_Waiting[num_string_now].status_echo_api = status_api;            
            #if (ENABLE_DEBUG_JSON == 1)
            Serial.print("status: ");
            Serial.println(status_api);
            Serial.println();
            #endif
            } 
      }
      else{ 
           #if (ENABLE_DEBUG_JSON == 1)
           Serial.print("HTTP error: ");      
           Serial.println(http.errorToString(httpResponseCode)); 
           #endif
           }                      
        http.end();  // Освобождаем память
        xSemaphoreGive(wifi_mutex);        
      }  
             
    if(secret.SW_Tx_SD == 1 && Data_Waiting[num_string_now].status_send_sd==0){
      Data_Waiting[num_string_now].status_send_sd=1;
      listDir(LittleFS, "/", 1);
      //Serial.print("Файлов в папке: ");
      //Serial.println(file_is_dir);       
      char nameBuff[50];
      char buff[100];
      
      String name_file = "/";
      if(rtc.getMonth() < 9){name_file += "0"; name_file += String(1 + rtc.getMonth());} 
      if(rtc.getMonth() >= 9){name_file += String(1 + rtc.getMonth());}
      name_file += ".";
      name_file += rtc.getYear();
      name_file  += ".txt"; 
      
      String str_to_csv = rtc.getDateTimePortal();     
      str_to_csv += ";";
      str_to_csv += String(Data_Waiting[num_string_now].counter_in);
      str_to_csv += ";";
      str_to_csv += String(Data_Waiting[num_string_now].counter_out);
      str_to_csv += ";"; 
      str_to_csv += "\n";   
      str_to_csv.toCharArray(buff, 100); 
      name_file.toCharArray(nameBuff, 50);    
      //Serial.println(nameBuff);
      
      if(appendFile(LittleFS, nameBuff, buff) == -1){
        vTaskDelay(100/portTICK_PERIOD_MS);
        writeFile(LittleFS, nameBuff, "");             
        }; 
        
      if(file_count > MAX_COUNT_FILES){ // удалить первый файл, если их кол-во больше MAX_COUNT_FILES
        char buffname[20];
        String FILE = "/";
        FILE += nameCount.name01;
        FILE.toCharArray(buffname, 100);
        deleteFile(LittleFS, buffname);}       
    } 
          
   num_string_now = -1;
   update_run_write = 0;
     
   if(F_clean_day == 1){ // сброс счетчика в 00:00
    F_clean_day = 0; 
    secret.Count_IN = 0; 
    secret.Count_OUT = 0;      
    peopleCounter.setcounter_indoor(0); 
    peopleCounter.setcounter_outdoor(0);
    }         
   }    
  vTaskDelay(5000/portTICK_PERIOD_MS);
 }   
}

void Init_Task3(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task3code,   /* Функция задачи. */
               "Task3",     /* Ее имя. */
               20000,       /* Размер стека функции */
               NULL,        /* Параметры */
               4,           /* Приоритет */
               &Task3,      /* Дескриптор задачи для отслеживания */
               1);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

void Task4code( void * pvParameters ){  // Синхронизация даты времени с NTP-сервера
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task4code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
  
  bool F_time_update_failed = 1;      

  for(;;){    
    xSemaphoreTake(wifi_mutex, portMAX_DELAY);
    
    if (WiFi.status() == WL_CONNECTED && secret.SW_AP == 0 && secret.type_ui == 1){
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);      
      struct tm timeinfo;
      
       if(!getLocalTime(&timeinfo)){
        //Serial.println("Failed to obtain time"); 
        secret.Led_mode = WAITING_MODE;
        F_time_update_failed = 1;}
        
       if(getLocalTime(&timeinfo)){
       //Serial.println("Time updated"); 
       secret.Led_mode = NORMAL_MODE;
       F_time_update_failed = 0;}
       
      }
      
    xSemaphoreGive(wifi_mutex);    

  if (F_time_update_failed == 0){  
    int period_update_time = PERIOD_UPDATE_TIME * 60000;
    vTaskDelay(period_update_time/portTICK_PERIOD_MS);}
    
  if (F_time_update_failed == 1){    
    vTaskDelay(5000/portTICK_PERIOD_MS);}
  } 
}

void Init_Task4(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task4code,   /* Функция задачи. */
               "Task4",     /* Ее имя. */
               4096,        /* Размер стека функции */
               NULL,        /* Параметры */
               5,           /* Приоритет */
               &Task4,      /* Дескриптор задачи для отслеживания */
               1);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

void Task5code( void * pvParameters ){  // Таймер для записи лога в файл и БД  
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task5code running on core ");
  Serial.println(xPortGetCoreID());
  #endif

  vTaskDelay(61000/portTICK_PERIOD_MS); 
  
  for(;;){         
    int year = rtc.getYear();
    if (secret.type_ui == 1 && year != 1970){ // Если активен user интерфейс и установлено время более 1970 года    
    int minutes = rtc.getMinute();
    switch (minutes) {
      case  0: save_data_period_in_queue(); 
               if (rtc.getHour(true)==0){                  
                  if(secret.SW_CLEAN_DAY == 1){F_clean_day = 1;} // очистить счетчик человеков в 00:00 если установлен флаг сброса счетчика
                  F_update = 1;  // Разрешить проверить обновление на сервере
                  secret.reboot_today = 0; // в полночь сбросить статус перезагрузки
                  EEPROM.put(0, secret); 
                  EEPROM.commit();                                                     
                  }
               vTaskDelay(61000/portTICK_PERIOD_MS); 
               break;      
      case 20: save_data_period_in_queue(); vTaskDelay(61000/portTICK_PERIOD_MS); break;      
      case 40: save_data_period_in_queue(); vTaskDelay(61000/portTICK_PERIOD_MS); break;
      //case 5: save_data_period_in_queue(); F_update=1; vTaskDelay(61000/portTICK_PERIOD_MS); break;    // тест  
      //case 59: save_data_period_in_queue(); F_update=1; vTaskDelay(61000/portTICK_PERIOD_MS); break;    // тест  
      }
    }   
    vTaskDelay(10000/portTICK_PERIOD_MS);
  }
}  

void Init_Task5(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (3)
               Task5code,   /* Функция задачи. */
               "Task5",     /* Ее имя. */
               4096,        /* Размер стека функции */
               NULL,        /* Параметры */
               3,           /* Приоритет */
               &Task5,      /* Дескриптор задачи для отслеживания */
               0);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

void Task6code( void * pvParameters ){  // Отслеживание кнопки сброса
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task6code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
  
  pinMode(btn_reboot, INPUT_PULLUP); // Настраиваем на вход
  int count_reboot = 0;              // если кнопка нажата 5 секунд сбрасываем до заводских  
  bool status_input = 1;
   
  for(;;){     
        
    status_input = digitalRead(btn_reboot);
    if ( status_input == 0 ){count_reboot++;}
    if ( status_input == 1 ){count_reboot = 0;}

  if (count_reboot >= 5){
    //Serial.println("DEFAULT SETTING REBOOT...");    
    int key = secret.initKey;    
    secret.initKey = key+1;        
    EEPROM.put(0, secret);  // сохраняем
    EEPROM.commit();        // записываем       
    ESP.restart();   
    }      
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }  
}

void Init_Task6(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task6code,   /* Функция задачи. */
               "Task6",     /* Ее имя. */
               4096,        /* Размер стека функции */
               NULL,        /* Параметры */
               2,           /* Приоритет */
               &Task6,      /* Дескриптор задачи для отслеживания */
               0);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

void Task7code(void* pvParameters) {   // Отправка данных ESP NOW
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task7code running on core ");
  Serial.println(xPortGetCoreID());
  #endif    

  for (;;) {  

    #if (ENABLE_DEBUG == 1) // Иммитация работы для отладки
    Buff_to_master.tempC_val[0] = random(-20,30);
    Buff_to_master.tempC_val[1] = random(-20,30);
    Buff_to_master.tempC_val[2] = random(-20,30);
    Buff_to_master.tempC_val[3] = random(-20,30);
    Buff_to_master.tempC_val[4] = random(-20,30);
    Buff_to_master.stat_input[0] = random(0,2);
    Buff_to_master.stat_input[1] = random(0,2);
    Buff_to_master.stat_input[2] = random(0,2);
    Buff_to_master.stat_input[3] = random(0,2);
    Buff_to_master.stat_input[4] = random(0,2);
    #endif   
       
    esp_err_t result1 = esp_now_send(MasterAddress, (uint8_t *) &Buff_to_master, sizeof(Buff_to_master)); 
     
    if (result1 == ESP_OK) {count_lost_send_1 = 0;}
    if (result1 != ESP_OK) {count_lost_send_1++;}
    
    #if (ENABLE_DEBUG == 1)
    if (result1 == ESP_OK) {Serial.println("Sent MASTER...");}
    else { Serial.println("Error sending the data master");}
    #endif     
  
  vTaskDelay(2000/portTICK_PERIOD_MS);           
  }
}

void Init_Task7() {
  xTaskCreatePinnedToCore(
    Task7code, /* Функция задачи. */
    "Task7",   /* Ее имя. */
    4096,      /* Размер стека функции */
    NULL,      /* Параметры */
    2,         /* Приоритет */
    &Task7,    /* Дескриптор задачи для отслеживания */
    1);        /* Указываем пин для данного ядра */
  delay(10);
}

void Task8code(void* pvParameters) {   // Watch Dog обмена данными ESP NOW 
  #if (ENABLE_DEBUG_TASK == 1) 
  Serial.print("Task8code running on core ");
  Serial.println(xPortGetCoreID());  
  #endif

  for (;;) {

    #if (ENABLE_DEBUG == 1)
     Serial.println();
     Serial.println("Watch Dog ESP NOW");
    #endif   
      
           
    if (count_lost_tx_1 >= LOST_PACKET_MAX_TX){
      count_lost_tx_1 = LOST_PACKET_MAX_TX;
      #if (ENABLE_DEBUG == 1) 
      Serial.println("ERROR TRANSMIT MASTER "); 
      #endif
      } 
              
   if (count_lost_send_1 >= LOST_PACKET_MAX_TX){
      count_lost_send_1 = LOST_PACKET_MAX_TX; 
      #if (ENABLE_DEBUG == 1)
      Serial.println("ERROR TRANSMIT ESP NOW 1");
      #endif 
      } 
                        
    #if (ENABLE_DEBUG == 1)
     Serial.println();
    #endif          
    
    vTaskDelay(2000/portTICK_PERIOD_MS);    
  }
}

void Init_Task8() {
  xTaskCreatePinnedToCore(
    Task8code, /* Функция задачи. */
    "Task8",   /* Ее имя. */
    4096,      /* Размер стека функции */
    NULL,      /* Параметры */
    2,         /* Приоритет */
    &Task8,    /* Дескриптор задачи для отслеживания */
    1);        /* Указываем пин для данного ядра */
  delay(10);
}

void Task9code(void* pvParameters) {   // Функция для работы зуммера  
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task9code running on core ");
  Serial.println(xPortGetCoreID());   
  #endif
  
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution); // задаём настройки ШИМ-канала:                                            
  ledcAttachPin(SOUND, pwmChannel);                   // подключаем ШИМ-канал к пину 
  ledcWrite(pwmChannel, 0);                           // Начальное скважности ШИМ-канала
  
  for (;;) {  

  if(soundRun == 1){  
    ledcWrite(pwmChannel, 127);     
    vTaskDelay(50/portTICK_PERIOD_MS);
    ledcWrite(pwmChannel, 0);
    vTaskDelay(200/portTICK_PERIOD_MS);
    ledcWrite(pwmChannel, 127);     
    vTaskDelay(50/portTICK_PERIOD_MS);
    ledcWrite(pwmChannel, 0);
    soundRun = 0;
    }
  vTaskDelay(100/portTICK_PERIOD_MS);        
  }
}

void Init_Task9() {
  xTaskCreatePinnedToCore(
    Task9code, /* Функция задачи. */
    "Task9",   /* Ее имя. */
    4096,      /* Размер стека функции */
    NULL,      /* Параметры */
    2,         /* Приоритет */
    &Task9,    /* Дескриптор задачи для отслеживания */
    1);        /* Указываем пин для данного ядра */
  delay(10);
}

void Task10code(void* pvParameters) {   // Функция для работы светодиода
  #if (ENABLE_DEBUG_TASK == 1)  
  Serial.print("Task10code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
  
  ledcSetup(pwmChannelRed, pwmFrequencyLed, pwmResolutionLed);   // задаём настройки ШИМ-канала:                                            
  ledcAttachPin(led_red, pwmChannelRed);                         // подключаем ШИМ-канал к пину 
  ledcWrite(pwmChannelRed, 0);                                   // Начальное скважности ШИМ-канала 
  ledcSetup(pwmChannelGreen, pwmFrequencyLed, pwmResolutionLed); // задаём настройки ШИМ-канала:                                            
  ledcAttachPin(led_green, pwmChannelGreen);                     // подключаем ШИМ-канал к пину 
  ledcWrite(pwmChannelGreen, 0);                                 // Начальное скважности ШИМ-канала 
  ledcSetup(pwmChannelBlue, pwmFrequencyLed, pwmResolutionLed);  // задаём настройки ШИМ-канала:                                            
  ledcAttachPin(led_blue, pwmChannelBlue);                       // подключаем ШИМ-канал к пину 
  ledcWrite(pwmChannelBlue, 0);                                  // Начальное скважности ШИМ-канала  
  pinMode(led_gnd, OUTPUT);                                      // Настраиваем на выход
  digitalWrite(led_gnd, LOW);                                    // Соединил с gnd   
  
  for (;;) { 
    if (secret.Led_mode == NORMAL_MODE){
      vTaskDelay(1000/portTICK_PERIOD_MS);
      ledcWrite(pwmChannelGreen, pwmBrightnessLed);      
      vTaskDelay(1000/portTICK_PERIOD_MS);
      ledcWrite(pwmChannelGreen, 0);           
    }
    if (secret.Led_mode == WAITING_MODE){
      vTaskDelay(100/portTICK_PERIOD_MS);      
      ledcWrite(pwmChannelGreen, pwmBrightnessLed);
      vTaskDelay(100/portTICK_PERIOD_MS);      
      ledcWrite(pwmChannelGreen, 0);      
    }
    if (secret.Led_mode == SETTINGS_MODE){
      vTaskDelay(1000/portTICK_PERIOD_MS);
      ledcWrite(pwmChannelBlue, pwmBrightnessLed);      
      vTaskDelay(1000/portTICK_PERIOD_MS);
      ledcWrite(pwmChannelBlue, 0);          
    }
    if (secret.Led_mode == ERROR_MODE){
      vTaskDelay(100/portTICK_PERIOD_MS);
      ledcWrite(pwmChannelRed, pwmBrightnessLed);      
      vTaskDelay(100/portTICK_PERIOD_MS);
      ledcWrite(pwmChannelRed, 0);           
    }    
  }
}

void Init_Task10() {
  xTaskCreatePinnedToCore(
    Task10code, /* Функция задачи. */
    "Task10",   /* Ее имя. */
    4096,      /* Размер стека функции */
    NULL,      /* Параметры */
    2,         /* Приоритет */
    &Task10,    /* Дескриптор задачи для отслеживания */
    1);        /* Указываем пин для данного ядра */
  delay(10);
}

void Task11code(void* pvParameters) {   // Отслеживание состояния буфера неотправленный/незаписанных данных
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task11code running on core ");
  Serial.println(xPortGetCoreID());
  #endif

  for (;;) {    
    if (secret.SW_Tx_SD  == 0) {for (int i = 0; i < MAX_DATA_WAITING; i++) {Data_Waiting[i].status_send_sd  = 1;}}  // очистка очереди, если выключили
    if (secret.SW_Tx_BD  == 0) {for (int i = 0; i < MAX_DATA_WAITING; i++) {Data_Waiting[i].status_send_bd  = 1; Data_Waiting[i].status_code_bd  = 0;}} // очистка очереди, если выключили
    if (secret.SW_Tx_API == 0) {for (int i = 0; i < MAX_DATA_WAITING; i++) {Data_Waiting[i].status_send_api = 1; Data_Waiting[i].status_code_api = 0;}} // очистка очереди, если выключили             

    
    for (int i = 0; i < MAX_DATA_WAITING; i++) {  // первый приоритет работы мониторинга для отправки свежих данных
      if (Data_Waiting[i].status_send_sd==0 || Data_Waiting[i].status_send_bd==0 || Data_Waiting[i].status_send_api==0){
        num_string_now = i; 
        update_run_write = 1;
         #if (ENABLE_DEBUG_DATA_WAITING == 1)
         Serial.println();
         Serial.println("Detecting new data"); 
         Serial.print("counter_waiting  ");
         Serial.println(counter_waiting);
         Serial.print("status_send_sd  ");
         Serial.println(Data_Waiting[num_string_now].status_send_sd);
         Serial.print("status_send_bd  ");
         Serial.println(Data_Waiting[num_string_now].status_send_bd);
         Serial.print("status_send_api ");
         Serial.println(Data_Waiting[num_string_now].status_send_api);
         Serial.println();
         #endif        
        break;
        }
    }

    #if (ENABLE_DEBUG_DATA_WAITING == 1)
    if (num_string_now == -1){Serial.println("Checking data");}
    #endif
    
    if (num_string_now == -1){ // если нет данных первого приоритета  
    for (int i = 0; i < MAX_DATA_WAITING; i++) {  // второй приоритет работы мониторинга API
       if (Data_Waiting[i].status_code_api!=201 && Data_Waiting[i].status_code_api!=0){
         num_string_now = i; 
         update_run_write = 1;
         Data_Waiting[i].status_send_api = 0;
         Data_Waiting[i].status_send_bd  = 1;
         //portal.log.println("ERROR SEND API");
         #if (ENABLE_DEBUG_DATA_WAITING == 1)
         Serial.println("ERROR SEND API");
         #endif
         break;
         }    
      }  
    } 
        
    if (num_string_now == -1){ // если нет данных первого приоритета  
    for (int i = 0; i < MAX_DATA_WAITING; i++) {  // второй приоритет работы мониторинга BD
       if (Data_Waiting[i].status_code_bd!=200 && Data_Waiting[i].status_code_bd!=0){
         num_string_now = i; 
         update_run_write = 1;
         Data_Waiting[i].status_send_api = 1;
         Data_Waiting[i].status_send_bd  = 0;
         //portal.log.println("ERROR SEND BD");
         #if (ENABLE_DEBUG_DATA_WAITING == 1)
         Serial.println("ERROR SEND BD");
         #endif
         break;
         }    
      }  
    }    
    vTaskDelay(30000/portTICK_PERIOD_MS);
  }
}

void Init_Task11() {
  xTaskCreatePinnedToCore(
    Task11code, /* Функция задачи. */
    "Task11",   /* Ее имя. */
    2048,      /* Размер стека функции */
    NULL,      /* Параметры */
    2,         /* Приоритет */
    &Task11,    /* Дескриптор задачи для отслеживания */
    1);        /* Указываем пин для данного ядра */
  delay(10);
}

void Task12code( void * pvParameters ){ // Обновление строковой переменной времени для Web интерфейса 
  #if (ENABLE_DEBUG_TASK == 1) 
  Serial.print("Task12code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
  
  for(;;){ 

    valDataTime = rtc.getDateTimePortal();
    vTaskDelay(5000/portTICK_PERIOD_MS);
  }
} 

void Init_Task12(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task12code,  /* Функция задачи. */
               "Task12",    /* Ее имя. */
               4096,        /* Размер стека функции */
               NULL,        /* Параметры */
               1,           /* Приоритет */
               &Task12,     /* Дескриптор задачи для отслеживания */
               1);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

void Task13code( void * pvParameters ){ // Сканирование WiFi 
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task13code running on core ");
  Serial.println(xPortGetCoreID());
  #endif

    vTaskDelay(3000/portTICK_PERIOD_MS);
    scanNetwork();
    
 vTaskDelete(NULL);
} 

void Init_Task13(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task13code,  /* Функция задачи. */
               "Task13",    /* Ее имя. */
               10000,        /* Размер стека функции */
               NULL,        /* Параметры */
               2,           /* Приоритет */
               &Task13,     /* Дескриптор задачи для отслеживания */
               0);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

void Task14code( void * pvParameters ){ // Калибровка TOF сенсора 
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task14code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
  
    xSemaphoreTake(sensor_mutex, portMAX_DELAY);
    vTaskDelay(2000/portTICK_PERIOD_MS);        
    secret.SensorOffset = myTofSensor.CallOffset(secret.DistanseCallReal);
    if( secret.SensorOffset < 0){secret.Led_mode = ERROR_MODE;}
    secret.SensorXTalk = myTofSensor.CallXTalk(secret.DistanseCallReal);
    if( secret.SensorXTalk < 0){secret.Led_mode = ERROR_MODE;}
    //secret.F_tof_callibration = 1;
    EEPROM.put(0, secret);                     
    EEPROM.commit();   
    vTaskDelay(10/portTICK_PERIOD_MS);
    xSemaphoreGive(sensor_mutex);
    ESP.restart();
    
 vTaskDelete(NULL);
} 

void Init_Task14(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task14code,  /* Функция задачи. */
               "Task14",    /* Ее имя. */
               10000,        /* Размер стека функции */
               NULL,        /* Параметры */
               0,           /* Приоритет */
               &Task14,     /* Дескриптор задачи для отслеживания */
               0);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

void Task15code( void * pvParameters ){ // Перезагрузка контроллера 
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task15code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
    
    vTaskDelay(3000/portTICK_PERIOD_MS);
    ESP.restart();
    
 vTaskDelete(NULL);
} 

void Init_Task15(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task15code,  /* Функция задачи. */
               "Task15",    /* Ее имя. */
               10000,        /* Размер стека функции */
               NULL,        /* Параметры */
               1,           /* Приоритет */
               &Task15,     /* Дескриптор задачи для отслеживания */
               0);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

void Task16code(void* pvParameters) {  // Обработка приема данных от Serial
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task3code running on core ");
  Serial.println(xPortGetCoreID()); 
  #endif 
  
  message_uart message;
  
    for (;;) { 
      if(QueueHandleUart != NULL){ // Проверка работоспособности просто для того, чтобы убедиться, что очередь действительно существует
        int ret = xQueueReceive(QueueHandleUart, &message, portMAX_DELAY);
      if(ret == pdPASS){ 
        secret.key_3 = message.key_3; 
        secret.ID_SERIAL1 = message.x;
        secret.key_1 = message.key_1;
        secret.ID_SERIAL2 = message.y;
        secret.key_4 = message.key_4;
        int key = secret.initKey;    
        secret.initKey = key+1; 
        EEPROM.put(0, secret);   // сохраняем
        EEPROM.commit();         // записываем
        
        
        buff_tx_uart.key_1 = random(1294967295, 3294967295);
        buff_tx_uart.id_devise = ESP.getEfuseMac();
        buff_tx_uart.key_2 = random(1294967295, 3294967295);
        buff_tx_uart.key_3 = random(20695, 59895);        
        buff_tx_uart.crc = crc8_bytes((byte*)&buff_tx_uart, sizeof(buff_tx_uart) - 1);
        Serial.write((byte*)&buff_tx_uart, sizeof(buff_tx_uart));                
   
      }
    }
  }  
}

void Init_Task16() {
  xTaskCreatePinnedToCore(
    Task16code, /* Функция задачи. */
    "Task16",   /* Ее имя. */
    4096,       /* Размер стека функции */
    NULL,       /* Параметры */
    1,          /* Приоритет */
    &Task16,    /* Дескриптор задачи для отслеживания */
    0);         /* Указываем пин для данного ядра */
  delay(50);
}

void Task17code( void * pvParameters ){ // Отложенная задача для обновления прошивки с сервера
  #if (ENABLE_DEBUG_TASK == 1)
  Serial.print("Task17code running on core ");
  Serial.println(xPortGetCoreID());
  #endif
    
    vTaskDelay(3000/portTICK_PERIOD_MS);
    long wait_ms = random(60000, 1800000);
    //Serial.print("задержка обновления: ");
    //Serial.print(wait_ms/1000);
    //Serial.println(" сек");    
    vTaskDelay(wait_ms/portTICK_PERIOD_MS);       

    if( WiFi.status() == WL_CONNECTED ){
      xSemaphoreTake(wifi_mutex, portMAX_DELAY); // Заблокировать WiFi для других задач
      t_httpUpdate_return ret = ESPhttpUpdate.update(link_sw);        
        switch(ret) {
            case HTTP_UPDATE_FAILED:
                //Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                count_err++;
                if(count_err <= max_err){F_update = 1;} // повторяем попытку обновления
                if(count_err > max_err) {count_err = 0; // Достигнуто максимальное количество попыток перезагружаем контроллер
                  if(secret.reboot_today == 0){         // повторить попытку обновления после перезагрузки если сегодня ещё не перезагружались
                    secret.update_sw = 1;                 
                    secret.reboot_today = 1;
                    EEPROM.put(0, secret); 
                    EEPROM.commit();
                    ESP.restart();
                    }
                  }
                //Serial.println();
                //Serial.println("не получается обновиться");
                //Serial.print("count_err ");
                //Serial.println(count_err);
                break;
            case HTTP_UPDATE_NO_UPDATES:
                //Serial.println("HTTP_UPDATE_NO_UPDATES");
                break;
            case HTTP_UPDATE_OK:
                //Serial.println("HTTP_UPDATE_OK");
                if(secret.update_sw == 1){
                  secret.update_sw = 0; 
                  EEPROM.put(0, secret); 
                  EEPROM.commit(); 
                  }
                break;
        }
    xSemaphoreGive(wifi_mutex); // Разблокировать WiFi для других задач   
    }

    if( WiFi.status() != WL_CONNECTED ){F_update = 0;} // если нет соединения отменить обновление 

 vTaskDelete(NULL);
} 

void Init_Task17(){
xTaskCreatePinnedToCore( //создаем задачу, которая будет выполняться на ядре 0 с максимальным приоритетом (1)
               Task17code,  /* Функция задачи. */
               "Task17",    /* Ее имя. */
               20000,       /* Размер стека функции */
               NULL,        /* Параметры */
               1,           /* Приоритет */
               &Task17,     /* Дескриптор задачи для отслеживания */
               1);          /* Указываем пин для данного ядра */                  
  delay(10); 
}

//---------------------------------------Работа WiFi-----------------------------------------------------------------

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("Connected to STA successfully!");   
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.print("WiFi connected");
  //Serial.print(" IP address: ");
  Connected_WiFi  = WiFi.localIP().toString();   
  //Serial.println(Connected_WiFi);  
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("Disconnected from WiFi access point");
  //Serial.print("WiFi lost connection. Reason: ");
  //Serial.println(info.wifi_sta_disconnected.reason);
  //Serial.println("Trying to Reconnect");
  Connected_WiFi = "xxx.xxx.xxx.xxx";
  if (secret.SW_STA == 1){WiFi.begin(secret.ssid_sta, secret.pass_sta);}
}

void WiFi_AP_Start(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("ESP32 soft-AP start");  
}

void WiFi_AP_Connected(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("Connected to AP successfully!");  
}

void WiFi_AP_Disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  //Serial.println("Disconnected from WiFi AP");
}

void INIT_STA_WiFi(){
  if (F_init_sta == 0){
    F_init_sta = 1;  
    WiFi.begin(secret.ssid_sta, secret.pass_sta); 
    if (F_added_event_sta == 0){ // создать евенты только один раз
      F_added_event_sta = 1;
      WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED); 
      WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP); 
      WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    }
  }
}

void INIT_AP_WiFi(){ 
  if (F_init_ap == 0){
    F_init_ap = 1;   
    IPAddress local_IP(192,168,0,100); // IP в режиме точки доступа
    IPAddress gateway(192,168,0,1);
    IPAddress subnet(255,255,255,0);
    //Serial.print("Setting soft-AP configuration ... ");                                   //  "Задаем настройки программной точки доступа ... "
    //Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");   //  "Готово" : "Задать настройки не удалось"                                                    
    WiFi.softAP(secret.ssid_ap, secret.pass_ap);
    if (F_added_event_ap == 0){ // создать евенты только один раз
      F_added_event_ap = 1;
      WiFi.onEvent(WiFi_AP_Start, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_START);  
      WiFi.onEvent(WiFi_AP_Connected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED); 
      WiFi.onEvent(WiFi_AP_Disconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
    }
  }
}

//---------------------------------------Работа ESP_NOW--------------------------------------------------------------

void IRAM_ATTR OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { // Callback when data is sent
  
  if (memcmp (mac_addr, MasterAddress, 6) == 0){  // ноль, если одинаковы
    if (status == 0){count_lost_tx_1 = 0;     
    }
    if (status != 0){count_lost_tx_1++;     
    }
    } 
  #if (ENABLE_DEBUG == 1)
   Serial.println();
  #endif   
}

void IRAM_ATTR OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) { // Callback when data is received
  memcpy(&Buff_Rx, incomingData, sizeof(Buff_Rx));
  
  F_RECIEVE_COMPLETE = 1;
  //for (int i=0; i<ELNUMBER; i++){
  //  Buff_to_master.current[i] = Buff_Rx.current[i];
  //  eload[i].curr_val = Buff_Rx.current[i];
  //}

}

void IRAM_ATTR serialEvent(){    
  if (Serial.readBytes((byte*)&buff_rx_uart, sizeof(buff_rx_uart))) {
  byte crc = crc8_bytes((byte*)&buff_rx_uart, sizeof(buff_rx_uart));

  uint64_t chip = ESP.getEfuseMac();  
  chip = chip/2;
  uint64_t id = secret.ID_SERIAL1 + secret.ID_SERIAL2;  
  
  if (crc == 0 && id != chip) {
    #if (ENABLE_DEBUG_UART == 1)
    Serial.println("CRC PASSED");
    #endif       
      message_uart message; 
      message.x = buff_rx_uart.x;
      message.key_1 = buff_rx_uart.key_1;
      message.y = buff_rx_uart.y;
      message.key_3 = buff_rx_uart.key_3;      
      message.key_4 = buff_rx_uart.key_4;     
      if(QueueHandleUart != NULL && uxQueueSpacesAvailable(QueueHandleUart) > 0){ // проверьте, существует ли очередь И есть ли в ней свободное место
        int ret = xQueueSend(QueueHandleUart, (void*) &message, 0);
        if(ret == pdTRUE){
          #if (ENABLE_DEBUG_UART == 1)
          Serial.println("serialEvent Отправлены данные в очередь ");      
          #endif    
          }
        else if(ret == errQUEUE_FULL){/*Serial.println("Не удалось отправить данные в очередь из serialEvent()");*/
        } 
      }
      else if (QueueHandleUart != NULL && uxQueueSpacesAvailable(QueueHandleUart) == 0){/*Serial.println("Очередь отсутствует или нет свободного места");*/}
      } 
   else {
      //Serial.println("CRC ERROR");
     }
   }  
}

void INIT_SLAVE_ESP_NOW(){
  
  WiFi.mode(WIFI_STA); 
  
  for (int i=0; i<6; i++){MasterAddress[i] = (byte)secret.MacAdressMaster[i];}  
    
  if (esp_now_init() != ESP_OK) {  
    //Serial.println("Error initializing ESP-NOW"); return;
    }
  if (esp_now_init() == ESP_OK) {   // Init ESP-NOW
    //Serial.print("WiFi.macAddress ");
    //Serial.println(WiFi.macAddress());
    //Serial.println("Success initializing ESP-NOW");
    }  
  esp_now_register_send_cb(OnDataSent); 

  peerInfo.channel = 0;  
  peerInfo.encrypt = false;  
  memcpy(peerInfo.peer_addr, MasterAddress, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK){/*Serial.println("Failed to add peer 1");*/}
    if (esp_now_add_peer(&peerInfo) == ESP_OK){/*Serial.println("Added peer 1 ");*/} 
  
  esp_now_register_recv_cb(OnDataRecv); // Register for a callback function that will be called when data is received
   
}

//---------------------------------------Разные функции--------------------------------------------------------------

void boardInfo(){
  Serial.println(); 
  Serial.println("****************************************");  
  chip = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).  
  Serial.printf("MCU chip ID:\t\t%04X", (uint16_t)(chip >> 32)); //print High 2 bytes
  Serial.printf("%08X\n", (uint32_t)chip); //print Low 4bytes. 
  Serial.printf("Flash chip frequency:\t%d (Hz)\n", ESP.getFlashChipSpeed());
  Serial.printf("Flash chip size:\t\t%d (bytes)\n", ESP.getFlashChipSize());
  Serial.printf("Free heap size:\t\t%d (bytes)\n", ESP.getFreeHeap());
  Serial.println("****************************************");
  Serial.println();  
  Serial.flush();
}

byte crc8_bytes(byte *buffer, byte size) {
  byte crc = 0;
  for (byte i = 0; i < size; i++) {
    byte data = buffer[i];
    for (int j = 8; j > 0; j--) {
      crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
      data >>= 1;
    }
  }
  return crc;
}
  
String displayFS() {  // вывести содержимое SPIFFS ссылками
//  <input type='button' class='miniButton' style='' value='Открыть' onclick='location.href="/12.2023.txt";'>
//  <a style='text-decoration:none;' href='/12.2023.txt' download><input type='button' value='Скачать' class='miniButton' style=''></a>
  
  String str;
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      str += file.name();
      str += F("&nbsp;<a href='/");
      str += file.name();
      str += F("'>[download]</a><br>\n");
    }
    file = root.openNextFile();
  }
  return str;
}

String getValue(String data, char separator, int index) { //Парсер строк по разделителю
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void scanNetwork(){   // WiFi.scanNetworks заполняем структуру networkFound найденными сетями 
  countNetworkFound = WiFi.scanNetworks(); // WiFi.scanNetworks will return the number of networks found
  Serial.println();
  if (countNetworkFound == 0) {Serial.println("no networks found");} 
  else {
    Serial.print(countNetworkFound);
    Serial.println(" networks found");
    networkFound[0] = String(secret.ssid_sta);
    for (int i = 0; i < countNetworkFound; ++i) {  // Print SSID and RSSI for each network found     
      Serial.print(i);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i)); 
      networkFound[i+1] = WiFi.SSID(i);
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));      
      Serial.print(")");
      Serial.print(" channel ");
      Serial.print(WiFi.channel(i));
      Serial.print(" ");  
      Serial.print("encryptionType ");
            switch (WiFi.encryptionType(i))
            {
            case WIFI_AUTH_OPEN:
                Serial.print("open");
                break;
            case WIFI_AUTH_WEP:
                Serial.print("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                Serial.print("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                Serial.print("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                Serial.print("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                Serial.print("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                Serial.print("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                Serial.print("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                Serial.print("WAPI");
                break;
            default:
                Serial.print("unknown");
            }
            Serial.println();    
      delay(10);
    }
  }
  Serial.println();
}

void save_data_period_in_queue(){
  Data_Waiting[counter_waiting].machine_id = String(chip);
  Data_Waiting[counter_waiting].counter_in = indoor_period_record;
  Data_Waiting[counter_waiting].counter_out = outdoor_period_record;
  Data_Waiting[counter_waiting].date_time = rtc.getDateTimeSQL();
  Data_Waiting[counter_waiting].name = secret.locate1; 
  Data_Waiting[counter_waiting].address = secret.locate2;  
  Data_Waiting[counter_waiting].door = secret.locate3;  
  
  Data_Waiting[counter_waiting].status_echo_api = ""; // очистка строки 
  Data_Waiting[counter_waiting].status_code_api = 0;  // очистка строки 
  Data_Waiting[counter_waiting].status_code_bd = 0;   // очистка строки         
  if (secret.SW_Tx_SD  == 1){Data_Waiting[counter_waiting].status_send_sd  = 0;}     
  if (secret.SW_Tx_API == 1){Data_Waiting[counter_waiting].status_send_api = 0;}   
  if (secret.SW_Tx_BD  == 1){Data_Waiting[counter_waiting].status_send_bd  = 0;}   
  
  indoor_period_record = 0;
  outdoor_period_record = 0;
  counter_waiting ++;
  if(counter_waiting == MAX_DATA_WAITING){counter_waiting = 0;}
}

void INIT_DEVAULT_SETTING(){
    Serial.println("СБРОС ДО ЗАВОДСКИХ НАСТРОЕК");
    secret.initKey = INIT_KEY;
    secret.type_ui = 2;    
    secret.Led_mode = WAITING_MODE;
    secret.F_tof_callibration = 0;
    String hostapi = "https://api.vormatic/machineWorkAdd";
    String hostdb = "https://vormatic.ru"; 
    String namedb = "WebCounter";
    String userdb = "CounterData";
    String passdb = "root";
    String locate1 = "Offise";
    String locate2 = "indoor";
    String locate3 = "1";
    String ssidAp = "Vormatic 2D Counter";
    String passAp = "12345678"; 
    String ssidSta = "not selected";
    String passSta = "12345678";     
    String webLogin = "admin";
    String webPassword = "admin";  
    hostapi.toCharArray(secret.API_HOST, sizeof(secret.API_HOST));  
    hostdb.toCharArray(secret.DB_HOST, sizeof(secret.DB_HOST));
    namedb.toCharArray(secret.DB_NAME, 50);
    userdb.toCharArray(secret.DB_USER, 20);
    passdb.toCharArray(secret.DB_PASS, 20);
    locate1.toCharArray(secret.locate1, 20);
    locate2.toCharArray(secret.locate2, 20);
    locate3.toCharArray(secret.locate3, 20);
    ssidAp.toCharArray(secret.ssid_ap, 50);
    passAp.toCharArray(secret.pass_ap, 20);
    webLogin.toCharArray(secret.web_login, 50);
    webPassword.toCharArray(secret.web_password, 20);
    ssidSta.toCharArray(secret.ssid_sta, 50);
    passSta.toCharArray(secret.pass_sta, 20);
    secret.Zone_MaxThreshold = 1800;   
    secret.Zone_MinThreshold = 200;
    secret.ZoneX = 16;
    secret.ZoneY = 8;       
    secret.Z1_ROI = 60;
    secret.Z2_ROI = 195;
    secret.SensorOffset = 1839;
    secret.SensorXTalk = 4074;  
    secret.DistanseCallReal = 2000;         
    secret.SW_Tx_BD = 0;
    secret.SW_Tx_SD = 1;
    secret.SW_Tx_API = 0;
    secret.SW_ESP_NOW = 0;
    secret.SW_STA = 0;
    secret.SW_AP = 1;
    secret.SW_CLEAN_DAY = 1;
    secret.SW_SOUND = 1;
    secret.SW_REVERS = 0;
    secret.SW_REVERS_INV = 1;   
    for(int i=0; i<6; i++){secret.MacAdressMaster[i] = 0;}
    //Serial.println();  
    //Serial.println("Конвертация MAC");
    String thasMacHex = WiFi.macAddress();
    thasMacHex.toCharArray(secret.Text_This_Mac_Hex, 40);
    //Serial.println(secret.Text_This_Mac_Hex);    
    String ArrThisMac[6];
    char MacPos[6][3];
    uint8_t ArrMacInt[6];
    for (int i=0; i<6; i++){     
      ArrThisMac[i] = getValue(thasMacHex, ':', i);      
      ArrThisMac[i].toCharArray(MacPos[i], 3);      
      ArrMacInt[i] = strtol(MacPos[i], NULL, 16);
      } 
    //for(int i=0; i<6; i++){Serial.print(ArrMacInt[i]);Serial.print(" ");}
    //Serial.println();    
    String TextDec = String(ArrMacInt[0]);
    TextDec += ":";
    TextDec += String(ArrMacInt[1]);
    TextDec += ":";
    TextDec += String(ArrMacInt[2]);
    TextDec += ":";
    TextDec += String(ArrMacInt[3]);
    TextDec += ":";
    TextDec += String(ArrMacInt[4]);
    TextDec += ":";
    TextDec += String(ArrMacInt[5]);
    TextDec.toCharArray(secret.Text_This_Mac_Dec, 40);  
    secret.update_sw = 0;    
    secret.reboot_today = 0;      
    EEPROM.put(0, secret);   // сохраняем
    EEPROM.commit();         // записываем
}

int findValueIndex(int rowCount){
   countRowsFile = rowCount;
   int index = rowCount - PLOT_SIZE;
   if (index < 0){return 0;}
   return arrValue[index][3];  // номер строки с которой начинать 
}

void DrowDatePlot(int index){

  String date = arrDateTime[index].substring(0, arrDateTime[index].indexOf(' '));
  arrDateTime[index].remove(0, arrDateTime[index].indexOf(' ') + 1);
  String time = arrDateTime[index].substring(0, arrDateTime[index].indexOf(' '));
  String day = date.substring(0, date.indexOf('.'));
  date.remove(0, date.indexOf('.') + 1);
  String mout = date.substring(0, date.indexOf('.'));
  date.remove(0, date.indexOf('.') + 1);
  String yars = date;
  String h = time.substring(0, time.indexOf(':'));
  time.remove(0, time.indexOf(':') + 1);
  String m = time.substring(0, time.indexOf(':'));  

  #if (ENABLE_PARSING_PLOT == 1)
    Serial.print("index: ");
    Serial.println(index);
    Serial.print("arrDateTime[index]: ");
    Serial.println(arrDateTime[index]);
    Serial.print("day: ");
    Serial.println(day);
    Serial.print("mout: ");
    Serial.println(mout);
    Serial.print("yars: ");
    Serial.println(yars);
    Serial.print("h: ");
    Serial.println(h);
    Serial.print("m: ");
    Serial.println(m);
    Serial.print("s: ");
    Serial.println(0);
  #endif

  dates[PLOT_SIZE - 1] = GPunix(yars.toInt(), mout.toInt(), day.toInt(), h.toInt(), m.toInt(), 0, gmtOffset_sec/3600); 
  uint32_t prevDates = dates[PLOT_SIZE - 1];

if(index != 0){
  for (int i = 0; i < PLOT_SIZE; i++) {
      GPaddInt(arrValue[index+i][0], arr[0], PLOT_SIZE);      
      GPaddInt(arrValue[index+i][1], arr[1], PLOT_SIZE);      
      //if (i == 0) {GPaddUnixS(3600, dates, PLOT_SIZE);}
      if (i != 0) {
        String date = arrDateTime[index+i].substring(0, arrDateTime[index+i].indexOf(' '));
        arrDateTime[index+i].remove(0, arrDateTime[index+i].indexOf(' ') + 1);
        String time = arrDateTime[index+i].substring(0, arrDateTime[index+i].indexOf(' '));
        String day = date.substring(0, date.indexOf('.'));
        date.remove(0, date.indexOf('.') + 1);
        String mout = date.substring(0, date.indexOf('.'));
        date.remove(0, date.indexOf('.') + 1);
        String yars = date;
        String h = time.substring(0, time.indexOf(':'));
        time.remove(0, time.indexOf(':') + 1);
        String m = time.substring(0, time.indexOf(':'));
        
      uint32_t nextDates = GPunix(yars.toInt(), mout.toInt(), day.toInt(), h.toInt(), m.toInt(), 0, gmtOffset_sec/3600);
      int stepDate = nextDates - prevDates; 
      prevDates = nextDates; 

    #if (ENABLE_PARSING_PLOT == 1)     
        Serial.print("arrValue[index+i][0]: ");
        Serial.println(arrValue[index+i][0]);
        Serial.print("arrValue[index+i][1]: ");
        Serial.println(arrValue[index+i][1]);
        Serial.print("Dates ");
        Serial.println(nextDates);     
        Serial.print("stepDate: ");
        Serial.println(stepDate);
    #endif

    GPaddUnixS(stepDate, dates, PLOT_SIZE);      
    }
  }
} 

if(index == 0){
  for (int i = 0; i < PLOT_SIZE; i++) {
    if (i < countRowsFile) {
      GPaddInt(arrValue[index+i][0], arr[0], PLOT_SIZE);      
      GPaddInt(arrValue[index+i][1], arr[1], PLOT_SIZE);       
      if (i != 0) {
        String date = arrDateTime[index+i].substring(0, arrDateTime[index+i].indexOf(' '));
        arrDateTime[index+i].remove(0, arrDateTime[index+i].indexOf(' ') + 1);
        String time = arrDateTime[index+i].substring(0, arrDateTime[index+i].indexOf(' '));
        String day = date.substring(0, date.indexOf('.'));
        date.remove(0, date.indexOf('.') + 1);
        String mout = date.substring(0, date.indexOf('.'));
        date.remove(0, date.indexOf('.') + 1);
        String yars = date;
        String h = time.substring(0, time.indexOf(':'));
        time.remove(0, time.indexOf(':') + 1);
        String m = time.substring(0, time.indexOf(':'));
        
      uint32_t nextDates = GPunix(yars.toInt(), mout.toInt(), day.toInt(), h.toInt(), m.toInt(), 0, gmtOffset_sec/3600);
      int stepDate = nextDates - prevDates; 
      prevDates = nextDates; 

    #if (ENABLE_PARSING_PLOT == 1)     
        Serial.print("arrValue[index+i][0]: ");
        Serial.println(arrValue[index+i][0]);
        Serial.print("arrValue[index+i][1]: ");
        Serial.println(arrValue[index+i][1]);
        Serial.print("Dates ");
        Serial.println(nextDates);     
        Serial.print("stepDate: ");
        Serial.println(stepDate);
    #endif
    GPaddUnixS(stepDate, dates, PLOT_SIZE);      
    }
   } 
  if (i >= countRowsFile) {
    GPaddInt(0, arr[0], PLOT_SIZE);      
    GPaddInt(0, arr[1], PLOT_SIZE);
    GPaddUnixS(3600, dates, PLOT_SIZE);
   } 
  }
 }
}

void readAndParseData(fs::FS &fs, const char * path){
   
    for (int i = 0; i < 2; i++) { for (int j = 0; j < PLOT_SIZE; j++) { arr[i][j] = 0;}}  // очистка массива
    for (int i = 0; i < PLOT_SIZE; i++) {dates[i] = 0;}  // очистка массива

    xSemaphoreTake(flash_mutex, portMAX_DELAY);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
      xSemaphoreGive(flash_mutex);
      //Serial.println("- failed to open file for reading");
      return;
    }
    while(file.available()){
       String line = file.readStringUntil(0x0A);

       #if (ENABLE_PARSING_GROUP == 1)
        //Serial.println(line);
       #endif 
       #if (ENABLE_PARSING == 1)
        //Serial.println(line);
       #endif 

      String dateTime = line.substring(0, line.indexOf(';'));
      arrDateTime[count] = dateTime;
      line.remove(0, line.indexOf(';') + 1);
      String value1 = line.substring(0, line.indexOf(';'));
      line.remove(0, line.indexOf(';') + 1);
      String value2 = line;
      tempVal_1 += value1.toInt();
      tempVal_2 += value2.toInt();

      String date = dateTime.substring(0, dateTime.indexOf(' '));
      dateTime.remove(0, dateTime.indexOf(' ') + 1);
      String time = dateTime.substring(0, dateTime.indexOf(' '));
      String day = date.substring(0, date.indexOf('.'));
      date.remove(0, date.indexOf('.') + 1);
      String mout = date.substring(0, date.indexOf('.'));
      date.remove(0, date.indexOf('.') + 1);
      String yars = date;
      String h = time.substring(0, time.indexOf(':'));
      time.remove(0, time.indexOf(':') + 1);
      String m = time.substring(0, time.indexOf(':'));       


      if (m.toInt()==0){
        arrValue[count][0] = tempVal_1;
        arrValue[count][1] = tempVal_2;
        arrValue[count][2] = 0;          // счетчик сутки
        arrValue[count][3] = count;      // счетчик  количества строк          
        if (m.toInt()==0 && h.toInt()==0){arrValue[count][2] = 1; countDay++;} // полночь флаг начала дня
        
      #if (ENABLE_PARSING_GROUP == 1)  
         
        Serial.print("arrDateTime[count]: ");
        Serial.println(arrDateTime[count]);    
        Serial.print("arrValue[count][0]: ");
        Serial.println(arrValue[count][0]);
        Serial.print("arrValue[count][1]: ");
        Serial.println(arrValue[count][1]);
        Serial.print("arrValue[count][2]: ");
        Serial.println(arrValue[count][2]);
        Serial.print("countHours: ");
        Serial.println(arrValue[count][3]);
        Serial.print("countDay: ");
        Serial.println(countDay);
        Serial.println();
      #endif

        tempVal_1 = 0;
        tempVal_2 = 0;
        count++;

      }

        
      #if (ENABLE_PARSING == 1)
        Serial.print("day: ");
        Serial.println(day);
        Serial.print("mout: ");
        Serial.println(mout);
        Serial.print("yars: ");
        Serial.println(yars);
        Serial.print("h: ");
        Serial.println(h);
        Serial.print("m: ");
        Serial.println(m);
        Serial.print("s: ");
        Serial.println(0);
        Serial.print("Value 1: ");
        Serial.println(value1);
        Serial.print("Value 2: ");
        Serial.println(value2);
      #endif
       
    }
    file.close();  

    xSemaphoreGive(flash_mutex);  

    int result = findValueIndex(count);
    DrowDatePlot(result);   
    
    count = 0;  // сброс счетчика

}

void setup() {
  Serial.setTimeout(5);   
  Serial.begin(115200);
  delay(10); 
  boardInfo();
      
  EEPROM.begin(4096);    // инициализация EEPROM 
  EEPROM.get(0, secret); // читаем данные из памяти
  delay(10);  
  
  #if (ENABLE_LOCKING == 1)  
  uint64_t chip = ESP.getEfuseMac();  
  chip = chip/2;
  uint64_t id = secret.ID_SERIAL1 + secret.ID_SERIAL2;
  if(id != chip || secret.ID_SERIAL1 == secret.ID_SERIAL2){ 
    Serial.println();   
    Serial.println("НЕ ЛИЦЕНЗИОННАЯ КОПИЯ"); 
    QueueHandleUart = xQueueCreate(QueueElementSizeUart, sizeof(message_uart)); // Создайте очередь, которая будет содержать <Размер элемента очереди> количество элементов, каждый из которых имеет размер `message_t`, и передайте адрес в <QueueHandleKeyboard>.
    if(QueueHandleUart == NULL){  // Проверьте, была ли успешно создана очередь
      //Serial.println("QueueHandleUart could not be created. Halt.");
      while(1) delay(1000);   // Halt at this point as is not possible to continue
      }
  Init_Task16();
  secret.Led_mode = ERROR_MODE;
  Init_Task10();
  Init_Task6();
  while(1){
    if(Serial.available() != 0){serialEvent();}
    delay(100);}       
  } 
  #endif
  
  if (secret.initKey != INIT_KEY){INIT_DEVAULT_SETTING();} // первый запуск устройства или сброс на базовые настройки

  wifi_mutex = xSemaphoreCreateMutex();
  sensor_mutex = xSemaphoreCreateMutex();
  flash_mutex = xSemaphoreCreateMutex();  
  
  if (secret.type_ui == 2){WiFi.disconnect(true); scanNetwork();}
  
  if (secret.SW_STA == 1){INIT_STA_WiFi();}
  if (secret.SW_AP == 1) {INIT_AP_WiFi();}
  if (secret.SW_AP == 0 && secret.SW_STA == 0 && secret.SW_ESP_NOW == 1){
    INIT_SLAVE_ESP_NOW();
    Init_Task7();  // Отправка данных мастеру
    Init_Task8();  // Watch Dog обмена данными ESP NOW
    }
          
  if (secret.type_ui == 1){portal.attachBuild(web_page_user); secret.Led_mode = WAITING_MODE;}
  if (secret.type_ui == 2){portal.attachBuild(web_page_admin); secret.Led_mode = SETTINGS_MODE;}  
  if (!LittleFS.begin()) {Serial.println("FS Error"); secret.Led_mode = ERROR_MODE;}
  portal.attach(action);    
  portal.start();   
  portal.enableAuth(secret.web_login, secret.web_password); // включить авторизацию
  portal.downloadAuto(true);                                // автоскачивание
  portal.enableOTA();                                       // без пароля   
 
  Init_Task1();   // Функция работы с TOF сенсором
  Init_Task2();   // Функция загрузки данных с tof в основную программу
  Init_Task11();  // Отслеживание состояния буфера неотправленный/незаписанных данных
  Init_Task3();   // Функция записи лога в файл, БД, API 
  Init_Task4();   // Синхронизация даты времени
  Init_Task5();   // Таймер для записи лога в файл, БД, API
  Init_Task6();   // Отслеживание кнопки сброса
  Init_Task9();   // Функция для работы зуммера
  Init_Task10();  // Функция для работы светодиода 
  Init_Task12();  // Обновление строковой переменной времени для Web интерфейса  

  if(secret.update_sw == 1){F_update = 1; delay(5000);} 
}

void loop() {

  if (secret.SW_ESP_NOW != 1){
    xSemaphoreTake(wifi_mutex, portMAX_DELAY);
      portal.tick();
    xSemaphoreGive(wifi_mutex);
  }
  
  if(WiFi.status() == WL_CONNECTED && WiFi.softAPgetStationNum()==0 && F_update==1){ // Если есть подключение к сети и есть разрешение на проверку
    xSemaphoreTake(wifi_mutex, portMAX_DELAY); // Заблокировать WiFi для других задач    
    char ServerName[100];
    int httpResponseCode = 0;
    String ResponseText = "";
    float rev_server;
    float rev_current;
    String server = "https://api.pg-corp.nohost.me/site/rest-api-update/items/read.php?name=";
    server += DEVICE_NAME;    
    server.toCharArray(ServerName, 100); 
      
    httpResponseCode = 0;      
    HTTPClient http;     
    http.begin(ServerName);
    httpResponseCode = http.GET(); // Отправляем запрос 
    if (httpResponseCode>0){ResponseText = http.getString();}
    else {/*Serial.print("Error code: ");  Serial.println(httpResponseCode);*/}
    http.end();  // Освобождаем память  

    JsonDocument Json;      
    DeserializationError error = deserializeJson(Json, ResponseText);      
      if (error) {
        //Serial.print(F("deserializeJson() failed: "));
        //Serial.println(error.f_str());
        delay(5000);
        count_err++;
        if(count_err > max_err){count_err = 0; F_update = 0;} // сбрасываем счетчик попыток и ждем следующего запроса
        }
      if (!error) {  
        F_update  = 0;          
        String api_version = Json["items"][0]["version"];
        String api_link = Json["items"][0]["link"];
        //Serial.print("version "); 
        //Serial.println(api_version);
        //Serial.print("link "); 
        //Serial.println(api_link);
        rev_server = api_version.toFloat();         // версия на сервере
        rev_current = CURRENT_VERSION_SW.toFloat(); // текущая версия
        if(rev_server > rev_current){               // на сервере более новая прошивка
          //Serial.println("Запускаем обновление"); 
          api_link.toCharArray(link_sw, sizeof(link_sw)); 
          Init_Task17();   // Запустить задачу на обновление прошивки  
          }     
        }
    xSemaphoreGive(wifi_mutex); // Разблокировать WiFi для других задач
  }     
} 
