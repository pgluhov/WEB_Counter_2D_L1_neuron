# Обновление прошивки ESP32 HTTP (OTA)

ESP-клон https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266httpUpdate (большая часть работы проделана Маркусом Саттлером).

# TL;DR

Это всего лишь быстрый порт ESP8266httpUpdate для ESP32.

Можно собрать с помощью фреймворка Arduino для ESP32.

Осталось решить несколько вопросов:

* Некоторые заголовки устарели (в любом случае будут изменены для ESP32)
* Загрузка в SPIFFS с расшифровкой AES-256
* Не поддерживает ESP-IDF.