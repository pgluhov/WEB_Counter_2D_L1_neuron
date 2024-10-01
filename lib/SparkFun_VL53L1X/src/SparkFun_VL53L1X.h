/*
  This is a library written for the VL53L1X I2C Distance sensor.

  Written by Andy England @ SparkFun Electronics, October 17th, 2017

  The sensor uses I2C to communicate, as well as a single (optional)
  interrupt line that is not currently supported in this driver.

  https://github.com/sparkfun/SparkFun_VL53L1X_Arduino_Library

  Do you like this library? Help support SparkFun. Buy a board!

  Development environment specifics:
  Arduino IDE 1.8.1

	==== MIT License ====	
	Copyright © 2022 SparkFun Electronics

	Permission is hereby granted, free of charge, to any person obtaining a copy of this 
	software and associated documentation files (the “Software”), to deal in the Software without restriction, 
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
	OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	=====================	
*/

#pragma once

#include "Arduino.h"
#include "Wire.h"
#include "st_src/RangeSensor.h"
#include "st_src/vl53l1_error_codes.h"
#include "st_src/vl53l1x_class.h"
#include "st_src/ComponentObject.h"
#include "st_src/RangeSensor.h"

#define DISTANCE_SHORT 	1
#define DISTANCE_LONG 	2
#define WINDOW_BELOW	0
#define WINDOW_ABOVE	1
#define WINDOW_OUT		2
#define WINDOW_IN		3

struct DetectionConfig  // Конфигурация обнаружения
{
	uint16_t distanceMode = DISTANCE_SHORT;	// distance mode : DISTANCE_SHORT (0) or DISTANCE_LONG (1)
	uint16_t windowMode = WINDOW_IN;		    // window mode : WINDOW_BELOW (0), WINDOW_ABOVE (1), WINDOW_OUT (2), WINDOW_IN (3)
	uint8_t IntOnNoTarget = 1;				      // = 1 (No longer used - just use 1)
	uint16_t thresholdHigh = 0;				      // (в мм) : пороговое значение, при превышении которого устройство инициирует прерывание, если Window = 1
	uint16_t thresholdLow = 0;				      // (в мм) : пороговое значение, при достижении которого устройство инициирует прерывание, если Window = 0
};

class SFEVL53L1X
{
	public:
	//Constructs our Distance sensor
	SFEVL53L1X(); //По умолчанию используется Wire. Установите для обоих контактов значение -1 (не определено).
	SFEVL53L1X(TwoWire &i2cPort, int shutdownPin = -1, int interruptPin = -1);
	~SFEVL53L1X();
	bool init();  //Устаревшая версия begin
	bool begin(); //Инициализация датчика
	bool begin(TwoWire &i2cPort); // Инициализация датчика
	bool checkID(); //Проверяет идентификатор датчика, возвращает значение true, если идентификатор правильный
	void sensorOn(); //Переключает вывод выключения для включения и выключения датчика
    void sensorOff(); //Переключает вывод выключения для включения и выключения датчика
	VL53L1X_Version_t getSoftwareVersion(); //Get's the current ST software version
	void setI2CAddress(uint8_t addr); //Set the I2C address
	int getI2CAddress(); //Get the I2C address
	void clearInterrupt(); // Clear the interrupt flag
	void setInterruptPolarityHigh(); //Установите полярность активного прерывания на высокую
	void setInterruptPolarityLow();  //Установите полярность активного прерывания на низкую
	uint8_t getInterruptPolarity();  //получить текущую полярность прерывания
	void startRanging(); //Начинает выполнять измерения
	void startOneshotRanging(); //Начать прицельную стрельбу одним выстрелом
	void stopRanging(); //Прекращает выполнение измерений
	bool checkForDataReady(); //Проверяет, готовы ли данные
	void setTimingBudgetInMs(uint16_t timingBudget); //Установите временной бюджет для измерения
	uint16_t getTimingBudgetInMs(); //Получите бюджет времени для измерения
	void setDistanceModeLong();  //Set to 4M range
	void setDistanceModeShort(); //Set to 1.3M range
	uint8_t getDistanceMode();   //Переключает режим расстояния, возвращает 1 для короткой дистанции и 2 для длинной
	void setIntermeasurementPeriod(uint16_t intermeasurement); //Установите время между измерениями в мс
	uint16_t getIntermeasurementPeriod(); //Получить время между измерениями в мс
	bool checkBootState();  //Проверьте, был ли инициализирован VL53L1X
	uint16_t getSensorID(); //Get the sensor ID
	uint16_t getDistance(); //Возвращает расстояние
	uint16_t getSignalPerSpad();  //Возвращает среднюю скорость передачи сигнала на SPAD (чувствительные панели, которые обнаруживают свет, в VL53L1X имеют матрицу 16x16 таких датчиков) в килокалориях/SPAD, или количество килограммов в секунду на SPAD.
	uint16_t getAmbientPerSpad(); //Возвращает значение окружающего шума, если сигнал не измеряется в kcps/SPAD.
	uint16_t getSignalRate(); //Возвращает скорость передачи сигнала в килокалориях в секунду. Все значения SPAD вместе взятые.
	uint16_t getSpadNb();     //Возвращает текущее количество включенных SPADs
	uint16_t getAmbientRate();//Возвращает общую скорость обмена в тыс. кбит/с. Все SPAD вместе взятые.
	uint8_t getRangeStatus(); //Возвращает состояние диапазона, которое может быть любым из следующих. 0 = ошибки нет, 1 = сбой сигнала, 2 = сигма-сбой, 7 = завершенный целевой сбой
	void setOffset(int16_t offset); //Вручную установите смещение в мм
	int16_t getOffset(); //Получить текущее смещение в мм
	void setXTalk(uint16_t xTalk);  //Вручную установите значение перекрестных помех в количестве отсчетов в секунду (cps), которое представляет собой помехи от любого окна перед вашим датчиком.
	uint16_t getXTalk(); //Возвращает текущее значение перекрестных помех в cps.
	void setDistanceThreshold(uint16_t lowThresh, uint16_t hiThresh, uint8_t window); //Установите границы для прерывания. низкий порог и максимальное значение - это границы вашего прерывания, в то время как window решает, когда должно сработать прерывание. Параметры для window показаны ниже.
	//0: Прерывание срабатывает при измерении расстояния ниже нижнего предела.
	//1: Прерывание срабатывает при измерении расстояния выше нижнего предела.
	//2: Прерывание срабатывает при измерении расстояния за пределами границ.
	//3: Прерывание срабатывает при измерении расстояния внутри границ.
	uint16_t getDistanceThresholdWindow(); //Возвращает параметр окна порога расстояния
	uint16_t getDistanceThresholdLow();    //Возвращает нижнюю границу в мм.
	uint16_t getDistanceThresholdHigh();   //Возвращает верхнюю границу в мм
	/**Table of Optical Centers**
	*
	* 128,136,144,152,160,168,176,184,  192,200,208,216,224,232,240,248
	* 129,137,145,153,161,169,177,185,  193,201,209,217,225,233,241,249
	* 130,138,146,154,162,170,178,186,  194,202,210,218,226,234,242,250
	* 131,139,147,155,163,171,179,187,  195,203,211,219,227,235,243,251
	* 132,140,148,156,164,172,180,188,  196,204,212,220,228,236,244,252
	* 133,141,149,157,165,173,181,189,  197,205,213,221,229,237,245,253
	* 134,142,150,158,166,174,182,190,  198,206,214,222,230,238,246,254
	* 135,143,151,159,167,175,183,191,  199,207,215,223,231,239,247,255
	
	* 127,119,111,103, 95, 87, 79, 71,  63, 55, 47, 39, 31, 23, 15, 7
	* 126,118,110,102, 94, 86, 78, 70,  62, 54, 46, 38, 30, 22, 14, 6
	* 125,117,109,101, 93, 85, 77, 69,  61, 53, 45, 37, 29, 21, 13, 5
	* 124,116,108,100, 92, 84, 76, 68,  60, 52, 44, 36, 28, 20, 12, 4
	* 123,115,107, 99, 91, 83, 75, 67,  59, 51, 43, 35, 27, 19, 11, 3
	* 122,114,106, 98, 90, 82, 74, 66,  58, 50, 42, 34, 26, 18, 10, 2
	* 121,113,105, 97, 89, 81, 73, 65,  57, 49, 41, 33, 25, 17, 9, 1
	* 120,112,104, 96, 88, 80, 72, 64,  56, 48, 40, 32, 24, 16, 8, 0 Pin 1
	*
	* Чтобы задать центр, установите площадку справа и выше точного центра области, которую вы хотите измерить в качестве оптического центра*/
	void setROI(uint8_t x, uint8_t y, uint8_t opticalCenter); //Установите высоту и ширину ROI (интересующей области) в единицах измерения, наименьший возможный параметр равен 4. Установите оптический центр в соответствии с приведенной выше таблицей
	uint16_t getROIX(); //Returns the width of the ROI in SPADs
	uint16_t getROIY(); //Returns the height of the ROI in SPADs
	void setSignalThreshold(uint16_t signalThreshold); //Программирует необходимый порог для запуска измерения. Default is 1024 kcps.
	uint16_t getSignalThreshold(); //Возвращает пороговое значение сигнала в kcps
	void setSigmaThreshold(uint16_t sigmaThreshold); //Программирует новый сигма-порог в мм. (default=15 mm)
	uint16_t getSigmaThreshold();  //Возвращает текущее значение сигма-порога.
	void startTemperatureUpdate(); //Выполняет повторную калибровку датчика при изменении температуры. Выполняйте это действие каждый раз, когда температура изменяется более чем на 8°C
	void calibrateOffset(uint16_t targetDistanceInMm); //Автоматическая калибровка смещения осуществляется путем размещения объекта на известном расстоянии от датчика и передачи этого известного расстояния в функцию.
	void calibrateXTalk(uint16_t targetDistanceInMm);  //Автоматическая калибровка перекрестных помех осуществляется путем размещения объекта на известном расстоянии от датчика и передачи этого известного расстояния в функцию.
	bool setThresholdConfig(DetectionConfig* config);  //Устанавливает структуру конфигурации порогового значения
	bool getThresholdConfig(DetectionConfig* config);  //Возвращает текущие пороговые настройки. В случае ошибки возвращает значение false, в противном случае значение true. Сохраняет результаты в виде указателя на struct аргумент.

	private:
	TwoWire *_i2cPort;
	int _shutdownPin;
	int _interruptPin;
	int _i2cAddress = 0x52;
	VL53L1X* _device;
};
