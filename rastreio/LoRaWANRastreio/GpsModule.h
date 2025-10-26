#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include <time.h>
#include "HT_TinyGPS++.h"

#define DEBUG_SERIAL 1

class GPSClass
{
public:
    void setup();
    bool waitGpsInfo(uint32_t timeoutMs = 10000);
    bool isValid() const;
    bool isUpdated() const;

    tm getTime() const;
    double getLatitude() const;
    double getLongitude() const;
    int32_t rawDegreesToInt32(const RawDegrees &raw);
    int32_t getLatitudeInt() const;
    int32_t getLongitudeInt() const;
    float getHdop() const;
    float getSpeedKmph() const;
    float getCourseDeg() const;
    uint32_t getSatellites() const;
};

extern GPSClass GPS;
#endif
