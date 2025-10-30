#ifndef TEST_LORAWAN_SERVICE_H
#define TEST_LORAWAN_SERVICE_H

#include <UnitTest.h>
#include <cstring>

#include "LoRaWANService.h"
#include "LoRaWan_APP.h"

class TestApp final : public App
{
public:
    explicit TestApp(LoRaWANServiceClass *serviceInstance = nullptr)
        : service(serviceInstance)
    {
    }

    void attachService(LoRaWANServiceClass *serviceInstance)
    {
        service = serviceInstance;
    }

    void setPending(size_t value)
    {
        pendingMessages = value;
    }

    void onTimer() override
    {
        onTimerCalls++;
    }

    void onSend() override
    {
        onSendCalls++;
        if (service != nullptr)
        {
            unsigned char payload[2] = {0xAA, 0x55};
            service->writeAppData(payload, sizeof(payload));
        }
    }

    void onSendAck() override
    {
        onAckCalls++;
    }

    size_t getPendingMessages() override
    {
        return pendingMessages;
    }

    size_t onTimerCalls = 0;
    size_t onSendCalls = 0;
    size_t onAckCalls = 0;

private:
    LoRaWANServiceClass *service = nullptr;
    size_t pendingMessages = 0;
};

void test_lorawan_info_json()
{
    mensagens_sem_ack = 2;

    TestApp app;
    LoRaWANServiceClass service(&app);
    app.attachService(&service);
    app.setPending(7);

    char buffer[160];
    memset(buffer, 0, sizeof(buffer));

    int written = service.infoJson(buffer, sizeof(buffer));

    TEST_ASSERT_GREATER_THAN(0, written);
    TEST_ASSERT_LESS_THAN((int)sizeof(buffer), written + 1);

    const char *expected = "{\"queued\":7,\"withoutAck\":2,\"devEui\":\"2232330000000023\",\"appEui\":\"0000000000000000\",\"appKey\":\"88888888888888888888888888888888\"}";
    TEST_ASSERT_EQUAL_STRING(expected, buffer);

    mensagens_sem_ack = 0;
}

void test_lorawan_loop_send_flow()
{
    mensagens_sem_ack = 0;
    millis_last_time = 0;
    appDataSize = 0;
    deviceState = DEVICE_STATE_SEND;

    TestApp app;
    LoRaWANServiceClass service(&app);
    app.attachService(&service);
    app.setPending(3);

    service.loop();

    TEST_ASSERT_EQUAL_UINT32(1, app.onTimerCalls);
    TEST_ASSERT_EQUAL_UINT32(1, app.onSendCalls);
    TEST_ASSERT_EQUAL_UINT8(2, appDataSize);
    TEST_ASSERT_EQUAL_UINT32(1, mensagens_sem_ack);
    TEST_ASSERT_EQUAL_INT(DEVICE_STATE_CYCLE, deviceState);

    appDataSize = 0;
    mensagens_sem_ack = 0;
    deviceState = DEVICE_STATE_INIT;
    millis_last_time = 0;
}

void test_lorawan_service()
{
    RUN_TEST(test_lorawan_info_json);
    RUN_TEST(test_lorawan_loop_send_flow);
}

#endif
