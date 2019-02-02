# Rise N' Shine IoT

The updated RiseNShine aims to make use of the FreeRTOS-ESP8266 SDK to fulfill new software requirements. These new software requirements enhances the product by providing a covenient user interface to allow debugging or monitoring. 

A personal project that uses MSP430G2553 and ESP8266 (ESP-01 module). This project showcases my skill in real-time system design and firmware design. A write up of the project can be found [here](https://jayveevelayo.com/project/2018/11/02/risenshine)

## Built With

* [ESP8266 FreeRTOS SDK v3.1](https://github.com/espressif/ESP8266_RTOS_SDK/tree/release/v3.1) - SDK tools for development
* [Xtensa Toolchain](https://dl.espressif.com/dl/xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz) - Toolchain for linux 64bit (also found in the above git)
* [Arduino IDE for ESP8266](https://github.com/esp8266/Arduino) - The compiler and flasher for the ESP8266
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson) - Used to parse JSON objects from web API
* [Sunset / Sunrse API](http://sunrise-sunset.org/api)
* [Current time API](http://worldclockapi.com/)
