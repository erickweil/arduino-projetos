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
    bool setup()
    {
        if(!WiFi.mode(WIFI_STA)) {
            Serial.println("Erro ao configurar WiFi em modo STA");
            return false;
        }
#if defined(WIFI_HOSTNAME) && defined(ARDUINO_ARCH_ESP32)
        if(!WiFi.setHostname(WIFI_HOSTNAME)) {
            Serial.println("Erro ao configurar hostname WiFi");
            return false;
        }
        if(!MDNS.begin(WIFI_HOSTNAME)) {
            Serial.println("Erro ao iniciar mDNS");
            return false;
        }
#endif
        if(WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
            Serial.println("Erro ao iniciar conexão WiFi");
            // mas tudo bem, vamos tentar de novo no loop...
        }

        return true;
    }

    void loop()
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            // https://microcontrollerslab.com/reconnect-esp32-to-wifi-after-lost-connection/
            if (connectMillis == 0)
                connectMillis = millis();
            else if (millis() - connectMillis > 20000)
            {
                // Timeout de conexão WiFi, reiniciar tentativa
                WiFi.disconnect();
                WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
                connectMillis = millis();
            }
            return;
        }
    }

    String getIP()
    {
        switch (WiFi.status())
        {
        case WL_CONNECTED:
            return WiFi.localIP().toString();
        case WL_IDLE_STATUS:
            return "idle";
        case WL_NO_SSID_AVAIL:
            return "no ssid";
        case WL_SCAN_COMPLETED:
            return "scan";
        case WL_CONNECT_FAILED:
            return "failed";
        case WL_CONNECTION_LOST:
            return "lost";
        case WL_DISCONNECTED:
            return "disconnect";
        default:
            return "unknown";
        }
    }

private:
    uint32_t connectMillis = 0;
};

#endif // WIFI_SERVICE_H
