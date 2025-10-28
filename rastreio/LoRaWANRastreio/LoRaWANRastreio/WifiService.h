#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <time.h>

#include "config.h"

class WifiServiceClass
{
public:
    WifiServiceClass() {}
    void setup();
    void loop();
    String getIP();

private:
    uint32_t connectMillis = 0;
};

extern WifiServiceClass WifiService;

#endif // WIFI_SERVICE_H
