#ifdef UNIT_TEST
#include "LoRaWANRastreio.ino"
#include <chrono>
#include <thread>

int main() 
{
    ArduinoFakeReset();
    implArduinoMocks();
    if(!LittleFS.begin(false)){
        Serial.println("LittleFS Mount Failed");
    }
    Serial.println("Iniciando rodando native... Setup:");
    setup();
    // delay 1 sec (because yes)
    delay(1000);

    // ArduinoFake tem memory leak. Utilizar https://github.com/bxparks/EpoxyDuino

    //while (true)
    //{
        // delay millis (because yes)
        //delay(20);
        //loop();
        // if(millis() % 7777 == 0) {
        //     Serial.println("Looping native...");
        // }
    //}
    return 0;
}
#endif