#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// # undef Pins_Arduino_h
// # include "pins_arduino.h"
#include <Arduino.h>

#include <unity.h>
#include <chrono>
#include <thread>
#include "LittleFS.h"
#include "MySerial.h"

#define RTC_DATA_ATTR
#define ACTIVE_REGION LORAMAC_REGION_AU915
#define Vext 3

extern uint32_t randr(uint32_t min, uint32_t max) {
   return min + (rand() % (max - min + 1));
}

#endif