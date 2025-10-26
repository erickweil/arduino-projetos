#include <UnitTest.h>

using namespace fakeit;

#include "PositionQueue.h"
#include "PositionQueue.cpp"

#include "GpsModule.h"
#include "GpsModule.cpp"

#include "test_position_queue.h"
#include "test_gps_module.h"

void setUp(void) {
    ArduinoFakeReset();
    PositionQueue.resetForUnitTest();
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