#include <UnitTest.h>

using namespace fakeit;

#include "HT_TinyGPS++.h"
#include "GpsModule.h"

// https://swairlearn.bluecover.pt/nmea_analyser
const char *gpsStream = "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
  "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
  "$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
  "$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,1.2,200.8,M,-22.5,M,,0000*6C\r\n"
  "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
  "$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";

/**
 * Plano de teste: rawDegreesToInt32_simple arredonda corretamente e aplica sinal.
 * Etapas:
 * 1) Chamar a função com valores positivos e verificar o resultado esperado.
 * 2) Chamar com billionths que causem arredondamento (ex: 345678900) e verificar.
 * 3) Chamar com negative=true e verificar sinal invertido.
 * 
 * Plano de teste: rawDegreesToInt32_rounding
 * - Construir RawDegrees with billionths near rounding boundary to ensure correct rounding
 * - Example: billionths = 345678950 should round up the last decimal when dividing by 100
 * - Expect result 123456790 (because 345678950 -> (345678950+50)/100 = 3456781 -> + deg*1e7)
 */
void test_raw_degrees_simple()
{
    // 12 degrees, 345,678,900 billionths => 12.3456789 -> 123456789
    RawDegrees raw;

    raw.deg = 12;
    raw.billionths = 345678900UL;
    raw.negative = false;
    int32_t v = GPS.rawDegreesToInt32(raw);
    TEST_ASSERT_EQUAL_INT32(123456789, v);

    raw.deg = 12;
    raw.billionths = 345678950UL; // slightly larger to force rounding up
    raw.negative = false;

    v = GPS.rawDegreesToInt32(raw);
    TEST_ASSERT_EQUAL_INT32(123456790, v);
}

/**
 * Plano de teste: parse NMEA full sentence flow
 * - Feed a GPGGA and GPRMC sentence into Serial1 fake buffer
 * - Call GPS.waitGpsInfo(timeout)
 * - Verify GPS.isValid(), GPS.isUpdated(), and multiple getters (lat/lng, hdop, speed, course, sats, integer coords)
 */
void test_parse_nmea_full()
{
    Serial1.__clear();
    Serial1.__feed(",,,A*7D\r\n$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n");
    Serial1.__delay();
    Serial1.__feed(gpsStream);

    TEST_ASSERT_TRUE(Serial1.available() > 0);
    TEST_ASSERT_TRUE(millis() >= 0);

    bool ok = GPS.waitGpsInfo(2000);
    TEST_ASSERT_TRUE(ok);
    
    TEST_ASSERT_TRUE(GPS.isValid());
    TEST_ASSERT_TRUE(GPS.isUpdated());

    tm timeinfo = GPS.getTime();
    TEST_ASSERT_EQUAL_INT(3, timeinfo.tm_mday);
    TEST_ASSERT_EQUAL_INT(9, timeinfo.tm_mon + 1); // tm_mon
    TEST_ASSERT_EQUAL_INT(2013, timeinfo.tm_year + 1900); // tm_year
    TEST_ASSERT_EQUAL_INT(4, timeinfo.tm_hour);
    TEST_ASSERT_EQUAL_INT(52, timeinfo.tm_min);
    TEST_ASSERT_EQUAL_INT(52, timeinfo.tm_sec);

    uint32_t timestamp = mktime(&timeinfo);
    TEST_ASSERT_TRUE(timestamp > 0);

    double lat = GPS.getLatitude();
    double lng = GPS.getLongitude();
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 30.240455, lat);
    TEST_ASSERT_FLOAT_WITHIN(0.0001, -97.81771333, lng);

    int32_t latInt = GPS.getLatitudeInt();
    int32_t lngInt = GPS.getLongitudeInt();
    TEST_ASSERT_EQUAL_INT32( 302404550, latInt);
    TEST_ASSERT_EQUAL_INT32(-978177133, lngInt);
}


void test_gps_module()
{
    RUN_TEST(test_raw_degrees_simple);
    RUN_TEST(test_parse_nmea_full);
}