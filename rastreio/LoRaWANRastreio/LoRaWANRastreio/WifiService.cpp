#include "WifiService.h"

WifiServiceClass WifiService;

void WifiServiceClass::setup()
{
    WiFi.mode(WIFI_STA);
#if defined(WIFI_HOSTNAME) && defined(ARDUINO_ARCH_ESP32)
    WiFi.setHostname(WIFI_HOSTNAME);
    MDNS.begin(WIFI_HOSTNAME);
#endif
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void WifiServiceClass::loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        // https://microcontrollerslab.com/reconnect-esp32-to-wifi-after-lost-connection/
        if(connectMillis == 0) connectMillis = millis();
        else if(millis() - connectMillis > 20000) {
            // Timeout de conexão WiFi, reiniciar tentativa
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            connectMillis = millis();
        }
        return;
    }
}

String WifiServiceClass::getIP()
{
    switch (WiFi.status())
    {
        case WL_CONNECTED: return WiFi.localIP().toString();
        case WL_IDLE_STATUS: return "idle";
        case WL_NO_SSID_AVAIL: return "no ssid";
        case WL_SCAN_COMPLETED: return "scan";
        case WL_CONNECT_FAILED: return "failed";
        case WL_CONNECTION_LOST: return "lost";
        case WL_DISCONNECTED: return "disconnect";
        default: return "unknown";
    }
}