#include <UnitTest.h>

#include "PositionQueue.h"

#include "GpsModule.h"

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