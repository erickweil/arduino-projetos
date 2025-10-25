#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include <time.h>

#define DEBUG_SERIAL 1

class GPSClass
{
public:
    void begin(HardwareSerial &serialPort, uint32_t baudRate, SerialConfig config, int rxPin, int txPin);
    bool waitGpsInfo(uint32_t timeoutMs = 10000);
    bool isValid() const;
    bool isUpdated() const;

    tm getTime() const;
    double getLatitude() const;
    double getLongitude() const;
    int32_t getLatitudeInt() const;
    int32_t getLongitudeInt() const;
    float getHdop() const;
    float getSpeedKmph() const;
    float getCourseDeg() const;
    uint32_t getSatellites() const;
};

extern GPSClass GPS;
#endif
