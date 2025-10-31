#ifndef LoRaWANSERVICE_H
#define LoRaWANSERVICE_H

#include <Arduino.h>
#include <LoRaWan_APP.h>

#include "config.h"
// extern's LoRaWan_APP.h
// Não alterar os nomes das variáveis ou quebra o código

// OTAA para
// EC 9E 13 9C 00 00 B4 C6
uint8_t devEui[] = {0x22, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x23};
uint8_t appEui[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t appKey[] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88};
// ABP para
uint8_t nwkSKey[] = {0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda, 0x85};
uint8_t appSKey[] = {0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef, 0x67};
uint32_t devAddr = (uint32_t)0x007e6ae1;

// LoraWan channelsmask, default channels 0-7
uint16_t userChannelsMask[6] = {0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = TX_DUTY_CYCLE;

/*LoraWan region, select in arduino IDE tools
Abrir Tools -> LoRaWAN Region -> REGION_AU915
*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t loraWanClass = CLASS_C;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = true;
/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;
/*!
  Number of trials to transmit the frame, if the LoRaMAC layer did not
  receive an acknowledgment. The MAC performs a datarate adaptation,
  according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
  to the following table:

  Transmission nb | Data Rate
  ----------------|-----------
  1 (first)       | DR
  2               | DR
  3               | max(DR-1,0)
  4               | max(DR-1,0)
  5               | max(DR-2,0)
  6               | max(DR-2,0)
  7               | max(DR-3,0)
  8               | max(DR-3,0)

  Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
  the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;

/**
 * Handle any downlink acknowledgment
 * Called when a downlink is received
 */
RTC_DATA_ATTR uint32_t mensagens_sem_ack = 0;

class LoRaWANServiceClass
{
public:
    App *app = nullptr;

    LoRaWANServiceClass() { }

    void setup(App *app_instance)
    {
        app = app_instance;
#ifndef UNIT_TEST
        Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
#endif
    }

    void loop()
    {
        if(app == nullptr) {
            return;
        }

        switch (deviceState)
        {
        case DEVICE_STATE_INIT:
        {
            // Serial.println("DEVICE_STATE_INIT");
#if (LORAWAN_DEVEUI_AUTO)
            LoRaWAN.generateDeveuiByChipID();
#endif
            LoRaWAN.init(loraWanClass, loraWanRegion);
            // both set join DR and DR when ADR off
            LoRaWAN.setDefaultDR(3);
            break;
        }
        case DEVICE_STATE_JOIN:
        {
            // Serial.println("DEVICE_STATE_JOIN");
            LoRaWAN.join();
            break;
        }
        case DEVICE_STATE_SEND:
        {
            // Serial.println("DEVICE_STATE_SEND");
            appDataSize = 0;
            
            if(app->getPendingMessages() <= 0 || app->millisLastTime == 0 || (millis() - app->millisLastTime > (appTxDutyCycle + 15000))) {
                app->onTimer();
            }

            app->onSend();
            
            if (appDataSize > 0)
            {
                mensagens_sem_ack++;
                LoRaWAN.send();
            }

            deviceState = DEVICE_STATE_CYCLE;
            break;
        }
        case DEVICE_STATE_CYCLE:
        {
            // Serial.println("DEVICE_STATE_CYCLE");

            uint32_t dutyCycleTime = appTxDutyCycle;
            // Se recebeu ACK recentemente e há mais posições na fila, envia mais rápido
            if (mensagens_sem_ack <= 1 && app->getPendingMessages() > 1)
            {
                dutyCycleTime = 10000; // Envia próximo pacote em 10 segundos se houver mais posições na fila
                //descarregando_fila = true;
            }
            // Schedule next packet transmission
            txDutyCycleTime = dutyCycleTime + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
            LoRaWAN.cycle(txDutyCycleTime);

            deviceState = DEVICE_STATE_SLEEP;
            break;
        }
        case DEVICE_STATE_SLEEP:
        {
            // Será que isso aqui funciona direito mesmo? SÓ FUNCIONA EM CLASS_C
            if (app->millisLastTime == 0) app->millisLastTime = millis();
            else if (millis() - app->millisLastTime > (appTxDutyCycle + 15000))
            {
                // Serial.println("Tempo sem leituras GPS excedido, lendo nova posição...");
                app->onTimer();
            }
                        
            // Classe A irá colocar em deep sleep.
            // Classe C deixa ligado para receber downlinks.
            LoRaWAN.sleep(loraWanClass);
            break;
        }
        default:
        {
            // Serial.print("DEFAULT:");
            // Serial.println(deviceState);
            deviceState = DEVICE_STATE_INIT;
            break;
        }
        }
    }

    /**
     * appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
     * appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
     * if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
     * if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
     * for example, if use REGION_CN470,
     * the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
     */
    void writeAppData(unsigned char *puc, int size)
    {
        if (appDataSize + size > LORAWAN_APP_DATA_MAX_SIZE)
        {
            // Serial.println("tentou escrever appData mas excedeu LORAWAN_APP_DATA_MAX_SIZE!");
            return;
        }
        for (int i = 0; i < size; i++)
        {
            appData[appDataSize++] = puc[i];
        }
    }

    void printAppData()
    {
        for (int i = 0; i < appDataSize; i++)
        {
            if (appData[i] < 16)
                Serial.print("0");
            Serial.print(appData[i], HEX);
            Serial.print(" ");
        }
        Serial.print(" (");
        Serial.print(appDataSize);
        Serial.println(" bytes)");
    }

    uint32_t sendsWithoutAck()
    {
        return mensagens_sem_ack;
    }

    int infoJson(char * const str_buf, size_t str_buf_size)
    {
        int len_bytes = 0;
        len_bytes += snprintf(str_buf, str_buf_size, "{\"queued\":%lu,\"withoutAck\":%u",
            app != nullptr ? app->getPendingMessages() : 0,
            mensagens_sem_ack
        );
        
        len_bytes += snprintf(str_buf + len_bytes, str_buf_size - len_bytes, ",\"devEui\":\"");
        len_bytes += snprintf_hex(str_buf + len_bytes, str_buf_size - len_bytes, devEui, sizeof(devEui));
        
        len_bytes += snprintf(str_buf + len_bytes, str_buf_size - len_bytes, "\",\"appEui\":\"");
        len_bytes += snprintf_hex(str_buf + len_bytes, str_buf_size - len_bytes, appEui, sizeof(appEui));

        len_bytes += snprintf(str_buf + len_bytes, str_buf_size - len_bytes, "\",\"appKey\":\"");
        len_bytes += snprintf_hex(str_buf + len_bytes, str_buf_size - len_bytes, appKey, sizeof(appKey));
        len_bytes += snprintf(str_buf + len_bytes, str_buf_size - len_bytes, "\"}");
        
        return len_bytes;
    }
private:
    int snprintf_hex(char * const str_buf, size_t str_buf_size, const uint8_t *data, size_t data_size)
    {
        int offset = 0;
        for (size_t i = 0; i < data_size; i++)
        {
            offset += snprintf(str_buf + offset, str_buf_size - offset, "%02X", data[i]);
        }
        return offset;
    }
};

extern LoRaWANServiceClass LoRaWANService;

void downLinkAckHandle()
{
    mensagens_sem_ack = 0;

    if(LoRaWANService.app != nullptr) {
        LoRaWANService.app->onSendAck();
    }
}

#endif // LoRaWANSERVICE_H