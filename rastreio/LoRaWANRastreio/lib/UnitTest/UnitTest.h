#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#ifndef UNIT_TEST
#define UNIT_TEST
#endif

#include "ArduinoFake.h"
#include <unity.h>
#include "MySerial.h"
#include "Preferences.h"
#include "LittleFS.h"

#define RTC_DATA_ATTR
#define ACTIVE_REGION LORAMAC_REGION_AU915

using namespace fakeit;

extern void implArduinoMocks()
{
    When(Method(ArduinoFake(), millis)).AlwaysDo([]() {
        static unsigned long currentMillis = 0;
        currentMillis += 1; // increment by 1 ms each call
        return currentMillis;
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
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long, int))).AlwaysDo([](unsigned int val, int base) {
       return printf(base == HEX ? "%X" : base == OCT ? "%o" : "%u", val);
    });

    When(OverloadedMethod(ArduinoFake(Serial), println, size_t())).AlwaysDo([]() {
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
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned long, int))).AlwaysDo([](unsigned int val, int base) {
       return Serial.print(val, base) + Serial.println();
    });
}

extern uint32_t randr(uint32_t min, uint32_t max) {
   return min + (rand() % (max - min + 1));
}

#endif