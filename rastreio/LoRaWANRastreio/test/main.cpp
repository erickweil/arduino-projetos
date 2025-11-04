#include <UnitTest.h>

#include "Position.h"
#include "PositionQueue.h"
#include "PositionQueueLittleFS.h"

#include "GpsModule.h"

#include "test_position_queue.h"
#include "test_gps_module.h"

void setUp(void) {
    ArduinoFakeReset();
    implArduinoMocks();
    if(!LittleFS.begin(true)){
        Serial.println("LittleFS Mount Failed");
    }
}

void tearDown(void) {
    // clean stuff up here
}

extern void testittleFSMock() {
    File file = LittleFS.open("/testfile.txt", FILE_WRITE);
    TEST_ASSERT_TRUE((bool)file);

    const char *data = "Hello, LittleFS!\0";
    size_t written = file.write((const uint8_t *)data, strlen(data)+1);
    TEST_ASSERT_EQUAL(strlen(data)+1, written);
    file.close();

    file = LittleFS.open("/testfile.txt", FILE_READ);
    TEST_ASSERT_TRUE((bool)file);

    char buffer[64];
    size_t readBytes = file.read((uint8_t *)buffer, sizeof(buffer)-1);
    TEST_ASSERT_EQUAL(strlen(data)+1, readBytes);
    TEST_ASSERT_EQUAL_STRING(data, buffer);
    file.close();
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(testittleFSMock);
    test_position_queue();
    test_gps_module();
    return UNITY_END();
}