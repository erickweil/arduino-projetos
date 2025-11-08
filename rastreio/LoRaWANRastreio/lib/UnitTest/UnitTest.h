#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#ifndef UNIT_TEST
#define UNIT_TEST
#endif

#undef Pins_Arduino_h
#include "pins_arduino.h"
#include "ArduinoFake.h"

#include <unity.h>
#include <chrono>
#include <thread>
#include "MySerial.h"
#include "Preferences.h"
#include "LittleFS.h"

#define RTC_DATA_ATTR
#define ACTIVE_REGION LORAMAC_REGION_AU915

using std::min;
using std::max;

using namespace fakeit;

extern void implArduinoMocks()
{
    When(Method(ArduinoFake(), millis)).AlwaysDo([]() {
        // millis by std
    static auto start_time = std::chrono::high_resolution_clock::now();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    return static_cast<unsigned long>(duration.count());
    });

    When(Method(ArduinoFake(), delay)).AlwaysDo([](unsigned long ms) {
         // std::this_thread::sleep_for(std::chrono::milliseconds(ms));
         fflush(stdout);
         std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    });

    When(Method(ArduinoFake(), pinMode)).AlwaysDo([](uint8_t pin, uint8_t mode) {
        // mock vazio — retorno void
    });

    When(Method(ArduinoFake(), digitalWrite)).AlwaysDo([](uint8_t pin, uint8_t val) {
        // mock vazio — retorno void
    });

    When(OverloadedMethod(ArduinoFake(Serial), begin, void(unsigned long))).AlwaysDo([](unsigned long baud_rate) {
    });
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char *))).AlwaysDo([](const char *str) {
       return printf("%s", str);
    });
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char))).AlwaysDo([](const char str) {
       return printf("%c", str);
    });
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned char, int))).AlwaysDo([](const unsigned char val, int base) {
       return printf(base == HEX ? "%X" : base == OCT ? "%o" : "%d", val);
    });
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(int, int))).AlwaysDo([](unsigned int val, int base) {
       return printf(base == HEX ? "%X" : base == OCT ? "%o" : "%d", val);
    });
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned int, int))).AlwaysDo([](unsigned int val, int base) {
       return printf(base == HEX ? "%X" : base == OCT ? "%o" : "%u", val);
    });
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(long, int))).AlwaysDo([](unsigned int val, int base) {
       return printf(base == HEX ? "%X" : base == OCT ? "%o" : "%d", val);
    });
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long, int))).AlwaysDo([](unsigned long val, int base) {
       return printf(base == HEX ? "%X" : base == OCT ? "%o" : "%lu", val);
    });

    When(OverloadedMethod(ArduinoFake(Serial), println, size_t())).AlwaysDo([]() {
       fflush(stdout);
       return printf("\n");
    });
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *))).AlwaysDo([](const char *str) {
       return Serial.print(str) + Serial.println();
    });
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(char))).AlwaysDo([](const char str) {
       return Serial.print(str) + Serial.println();
    });
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned char, int))).AlwaysDo([](const unsigned char val, int base) {
       return Serial.print(val, base) + Serial.println();
    });
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(int, int))).AlwaysDo([](unsigned int val, int base) {
      return Serial.print(val, base) + Serial.println();
    });
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned int, int))).AlwaysDo([](unsigned int val, int base) {
      return Serial.print(val, base) + Serial.println();
    });
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(long, int))).AlwaysDo([](unsigned int val, int base) {
       return Serial.print(val, base) + Serial.println();
    });
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned long, int))).AlwaysDo([](unsigned long val, int base) {
       return Serial.print(val, base) + Serial.println();
    });
}

extern uint32_t randr(uint32_t min, uint32_t max) {
   return min + (rand() % (max - min + 1));
}

extern void configTzTime(const char* tz, const char* server1, const char* server2 = nullptr, const char* server3 = nullptr) {
    
}

class ESP32Class {
public:
    void restart() {
        // For unit tests, we won't actually restart, just exit the program
        Serial.println("ESP.restart() called");
        exit(0);
    }

   unsigned int getFreeHeap() {
      return 1024 * 1024; // 1 MB for testing
   }

   unsigned int getHeapSize() {
      return 2 * 1024 * 1024; // 2 MB for testing
   }
};
ESP32Class ESP;

#endif