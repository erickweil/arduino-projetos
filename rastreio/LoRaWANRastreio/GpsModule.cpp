#include "HT_TinyGPS++.h"
#include "GpsModule.h"
GPSClass GPS;


// GPS principal usado pelo firmware
static TinyGPSPlus TinyGPS;

void GPSClass::begin(HardwareSerial &serialPort, uint32_t baudRate, SerialConfig config, int rxPin, int txPin)
{
    serialPort.begin(115200, SERIAL_8N1, 33, 34);
}

/**
 * Processa os dados do GPS até que não haja mais linhas.
 * Caso demore mais de 15 segundos sem chegar nenhum dado ou em uma linha pela metade desiste.
 */
bool GPSClass::waitGpsInfo(uint32_t timeoutMs)
{
    uint32_t start = millis();
    int c = -1;
    // Flush pois o buffer irá conter linhas parciais inválidas
    //while (Serial1.available() > 0 && Serial1.read() > 0)
    //    ;
    if(Serial1.available() > 0) {
        while(c != '\n' || Serial1.available() > 0) {
            if (Serial1.available() > 0) {
                c = Serial1.read();
            }

            if ((millis() - start) > 10000)
            {
                Serial.println("Timeout: Nenhum dado GPS recebido após 10 segundos...");
                return false;
            }
        }

        c = -1;
    }

#if DEBUG_SERIAL
    Serial.println("<GPS>");
#endif
    while (true)
    {
        // Lê cada uma das linhas NMEA https://gpsd.gitlab.io/gpsd/NMEA.html
        while (Serial1.available() > 0)
        {
            c = Serial1.read();
#if DEBUG_SERIAL
            Serial.print((char)c);
#endif
            TinyGPS.encode(c);
        }

        // Se parou de ler em um fim de linha, encerrou os dados.
        if (c == '\n')
        {
#if DEBUG_SERIAL
            Serial.println("</GPS>");
#endif
            break;
        }

        // Timeout
        if ((millis() - start) > 10000)
        {
#if DEBUG_SERIAL
            Serial.println("</GPS>");
#endif
            Serial.println("Timeout: Nenhum dado GPS recebido após 10 segundos...");
            return false;
        }
    }

    if (!TinyGPS.time.isValid())
    {
        Serial.println("Tempo inválido, tentando novamente...");
        return false;
    }

    return true;
}

bool GPSClass::isValid() const
{
    return TinyGPS.location.isValid();
}

bool GPSClass::isUpdated() const
{
    return TinyGPS.location.isUpdated();
}

/**
 * https://berthub.eu/articles/posts/how-to-get-a-unix-epoch-from-a-utc-date-time-string/
 * https://en.cppreference.com/w/c/chrono/tm
 */
tm GPSClass::getTime() const
{
    return {
        .tm_sec = TinyGPS.time.second(),       //  seconds after the minute – [​0​, 61](until C99)[​0​, 60](since C99)[note 1]
        .tm_min = TinyGPS.time.minute(),       //  minutes after the hour – [​0​, 59]
        .tm_hour = TinyGPS.time.hour(),        //  hours since midnight – [​0​, 23]
        .tm_mday = TinyGPS.date.day(),         //  day of the month – [1, 31]
        .tm_mon = TinyGPS.date.month() - 1,    //  months since January – [​0​, 11]
        .tm_year = TinyGPS.date.year() - 1900, //  years since 1900
        .tm_isdst = 0                      //  Daylight Saving Time flag. The value is positive if DST is in effect, zero if not and negative if no information is available
    };
}

double GPSClass::getLatitude() const
{
    return TinyGPS.location.lat();
}

double GPSClass::getLongitude() const
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
int32_t rawDegreesToInt32(const RawDegrees &raw)
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

int32_t GPSClass::getLatitudeInt() const
{
    return rawDegreesToInt32(TinyGPS.location.rawLat());
}

int32_t GPSClass::getLongitudeInt() const
{
    return rawDegreesToInt32(TinyGPS.location.rawLng());
}

float GPSClass::getHdop() const
{
    return TinyGPS.hdop.hdop();
}

float GPSClass::getSpeedKmph() const
{
    return TinyGPS.speed.kmph();
}

float GPSClass::getCourseDeg() const
{
    return TinyGPS.course.deg();
}

uint32_t GPSClass::getSatellites() const
{
    return TinyGPS.satellites.value();
}
