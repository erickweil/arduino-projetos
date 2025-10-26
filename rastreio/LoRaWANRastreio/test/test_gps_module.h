#include <UnitTest.h>

using namespace fakeit;

#include "HT_TinyGPS++.h"
#include "GpsModule.h"

/**
 * Plano de teste: rawDegreesToInt32_simple arredonda corretamente e aplica sinal.
 * Etapas:
 * 1) Chamar a função com valores positivos e verificar o resultado esperado.
 * 2) Chamar com billionths que causem arredondamento (ex: 345678900) e verificar.
 * 3) Chamar com negative=true e verificar sinal invertido.
 */
void test_raw_degrees_simple_positive()
{
    // 12 degrees, 345,678,900 billionths => 12.3456789 -> 123456789
    RawDegrees raw;

    raw.deg = 12;
    raw.billionths = 345678900UL;
    raw.negative = false;
    int32_t v = GPS.rawDegreesToInt32(raw);
    TEST_ASSERT_EQUAL_INT32(123456789, v);
}

void test_gps_module()
{
    RUN_TEST(test_raw_degrees_simple_positive);
}
