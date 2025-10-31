#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include <time.h>
// # include <TinyGPSPlus.h>
#include "HT_TinyGPS++.h"
#include "config.h"

// GPS principal usado pelo firmware
static TinyGPSPlus TinyGPS;

class GPSClass
{
public:
    void setup()
    {
        Serial1.begin(115200, SERIAL_8N1, 33, 34);
    }

    bool loop()
    {
        // A FAZER: Detectar que o buffer encheu demais e tem linhas inválidas, e fazer flush nessa situação.
        // flushGps(0)

        if(!waitGpsInfo(0, false)) 
            return false;

        return true;
    }

    bool flushGps(uint32_t timeoutMs)
    {
        if (Serial1.available() <= 0)
        {
            return true;
        }

        uint32_t start = millis();
        int c = -1;
        // while (Serial1.available() > 0 && Serial1.read() > 0)
        //    ;
        while (c != '\n' || Serial1.available() > 0)
        {
            if (Serial1.available() > 0)
            {
                c = Serial1.read();
            }

            if (timeoutMs == 0 || (millis() - start) > timeoutMs)
            {
                if (timeoutMs != 0) {
                    Serial.print("Timeout: Nenhum dado GPS recebido após ms:"); Serial.println(timeoutMs);
                }

                return false;
            }
        }

        return true;        
    }

    /**
     * Processa os dados do GPS até que não haja mais linhas.
     * Caso demore mais de 10 segundos sem chegar nenhum dado ou em uma linha pela metade desiste.
     */
    bool waitGpsInfo(uint32_t timeoutMs = 10000, bool flush = true)
    {
        uint32_t start = millis();
        int c = -1;

        // Flush pois o buffer pode conter linhas parciais inválidas
        if(flush) 
        {
            if(!flushGps(timeoutMs)) 
            {
                return false;
            }
        }

        while (true)
        {
            // Lê cada uma das linhas NMEA https://gpsd.gitlab.io/gpsd/NMEA.html
            while (Serial1.available() > 0)
            {
                c = Serial1.read();
//# if DEBUG_SERIAL
                //Serial.print((char)c);
//# endif
                TinyGPS.encode(c);
            }

            // Se parou de ler em um fim de linha, encerrou os dados.
            if (c == '\n')
            {
                break;
            }

            // Timeout
            if (timeoutMs == 0 || (millis() - start) > timeoutMs)
            {
                if (timeoutMs != 0) {
                    Serial.print("Timeout: Nenhum dado GPS recebido após ms:"); Serial.println(timeoutMs);
                }

                return false;
            }
        }

        return TinyGPS.time.isValid() && TinyGPS.location.isValid();
    }

    bool isUpdated() const
    {
        return TinyGPS.location.isUpdated();
    }

    uint32_t getLocationAge()
    {
        return TinyGPS.location.age();
    }

    /**
     * https://berthub.eu/articles/posts/how-to-get-a-unix-epoch-from-a-utc-date-time-string/
     * https://en.cppreference.com/w/c/chrono/tm
     */
    tm getTime()
    {
        return {
            .tm_sec = TinyGPS.time.second(),       //  seconds after the minute – [​0​, 61](until C99)[​0​, 60](since C99)[note 1]
            .tm_min = TinyGPS.time.minute(),       //  minutes after the hour – [​0​, 59]
            .tm_hour = TinyGPS.time.hour(),        //  hours since midnight – [​0​, 23]
            .tm_mday = TinyGPS.date.day(),         //  day of the month – [1, 31]
            .tm_mon = TinyGPS.date.month() - 1,    //  months since January – [​0​, 11]
            .tm_year = TinyGPS.date.year() - 1900, //  years since 1900
            .tm_isdst = 0                          //  Daylight Saving Time flag. The value is positive if DST is in effect, zero if not and negative if no information is available
        };
    }

    double getLatitude()
    {
        return TinyGPS.location.lat();
    }

    double getLongitude()
    {
        return TinyGPS.location.lng();
    }

    /**
     * https://forum.arduino.cc/t/storing-latitute-and-longitude-using-a-double/670691/17
     * long integer format in degrees*(ten million) +/- 1 cm precision.
     *
     * Converte a estrutura RawDegrees (usada pelo TinyGPS++) para um int32_t
     * com 7 casas decimais de precisão (escala de 1e7).
     * @param raw A estrutura RawDegrees (ex: GPS.location.rawLat()).
     * @return A coordenada como um int32_t (ex: -12.3456789 se torna -123456789).
     */
    int32_t rawDegreesToInt32(const RawDegrees &raw) const
    {
        // Fator de escala: 10,000,000 para 7 casas decimais
        // 1. 12 graus -> 120,000,000
        int32_t result = (int32_t)raw.deg * 10000000L;

        // Adicionamos 50 para arredondamento correto antes da divisão inteira
        // Ex: 345,678,900 bilionésimos -> (345678900 + 50) / 100 = 3,456,789
        //     120,000,000 + 3,456,789 = 123,456,789
        result += (int32_t)((raw.billionths + 50UL) / 100UL);

        // 4. Aplica o sinal negativo se necessário
        if (raw.negative)
        {
            result = -result;
        }

        return result;
    }

    int32_t getLatitudeInt()
    {
        return rawDegreesToInt32(TinyGPS.location.rawLat());
    }

    int32_t getLongitudeInt()
    {
        return rawDegreesToInt32(TinyGPS.location.rawLng());
    }

    float getHdop()
    {
        return TinyGPS.hdop.hdop();
    }

    float getSpeedKmph()
    {
        return TinyGPS.speed.kmph();
    }

    float getCourseDeg()
    {
        return TinyGPS.course.deg();
    }

    uint32_t getSatellites()
    {
        return TinyGPS.satellites.value();
    }

    // https://math.stackexchange.com/questions/110173/how-to-calculate-relative-degree-changes-in-0-to-360
    static float absCourseDiff(float lastCourse, float course) {
        float diff = abs(lastCourse - course);
        if(diff > 180.0f) {
            return 360.0f - diff;
        } else {
            return diff;
        }
    }
};

#endif
