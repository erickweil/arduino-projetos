#include <UnitTest.h>
#include "MySerial.cpp"

#include "PositionQueue.h"
#include "PositionQueue.cpp"

#include "GpsModule.h"
#include "GpsModule.cpp"

#include "LoRaWan_APP.h"
#include "LoRaWan_APP.cpp"

#include "LoRaWANService.h"

#include "test_position_queue.h"
#include "test_gps_module.h"

void setUp(void) {
    ArduinoFakeReset();
    implArduinoMocks();
}

void tearDown(void) {
    // clean stuff up here
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    test_position_queue();
    test_gps_module();
    return UNITY_END();
}