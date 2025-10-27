/**
    Código a ser utilizado no Heltec HITIT-Tracker (Wireless Tracker v1.2)
    Links:
    - https://heltec.org/project/wireless-tracker/
    - https://docs.heltec.org/en/node/esp32/wireless_tracker/index.html
    - https://resource.heltec.cn/download/Wireless_Tracker/
    - https://br.mouser.com/datasheet/3/1574/1/esp32-s3_datasheet_en.pdf (Datasheet processador ESP32-S3FN8)
*/
#include "Arduino.h"

#include "LoRaWan_APP.h"
#include "HT_st7735.h"
#include <time.h>

// Configurações do projeto (LoRaWAN, etc)
#include "config.h"

// Comunicação LoRaWAN
#include "LoRaWANService.h"

// GPS principal usado pelo firmware
#include "GpsModule.h"

// Fila de posições armazenadas
#include "PositionQueue.h"

HT_st7735 st7735;

class MyApp: public App
{
public:
    // Chamado periodicamente para registrar uma nova leitura
    void onTimer() override
    {
        // pinMode(Vext, OUTPUT);
        // digitalWrite(Vext, HIGH);

        // ~2 min Fix GPS
        char str_buf[15];
        for (int i = 0; i < 10; i++)
        {
            bool gps_result = GPS.waitGpsInfo();
            bool gps_valid = GPS.isValid() && GPS.isUpdated();

            st7735.st7735_fill_screen(ST7735_BLACK);
            // A FAZER: parar de usar classe String
            //char buf[30]; // Buffer para formatar strings
            //snprintf(buf, 30, "Na fila: %d", numero_posicoes());
            snprintf(str_buf, sizeof(str_buf), "%d ...%d", PositionQueue.getStart(), PositionQueue.size());
            st7735.st7735_write_str(0, 0, str_buf);

            tm t = GPS.getTime();
            snprintf(str_buf, sizeof(str_buf), "%02d:%02d:%02d %s", 
                t.tm_hour, t.tm_min, t.tm_sec, gps_valid ? "FIX" : ".", i);
            st7735.st7735_write_str(0, 20, str_buf);

            snprintf(str_buf, sizeof(str_buf), "%.7f", GPS.getLatitude());
            st7735.st7735_write_str(0, 40, str_buf);
            snprintf(str_buf, sizeof(str_buf), "%.7f", GPS.getLongitude());
            st7735.st7735_write_str(0, 60, str_buf);

            if (gps_result && gps_valid)
                break;

            // Wait 1 sec before trying again...
            delay(1000);
        }

        Posicao pos = {0, 0, 0, 0, 0, 0, 0};
        tm t = GPS.getTime();
        /** Obter unix timestamp (seconds since 1970-01-01) válido a partir dos valores do GPS
            https://pubs.opengroup.org/onlinepubs/009696799/functions/mktime.html */
        pos.timestamp = (uint32_t)mktime(&t);

        // Converter latitude e longitude para formato inteiro em degrees*(ten million)
        pos.lat = GPS.getLatitudeInt();
        pos.lng = GPS.getLongitudeInt();

        pos.course = (uint8_t)((GPS.getCourseDeg() * 255.0f / 360.0f) + 0.5f);
        pos.speed = (uint8_t)(min(GPS.getSpeedKmph() + 0.5f, 255.0f));
        pos.hdop = (uint8_t)(min(GPS.getHdop() * 10, 255.0f) + 0.5f);
        pos.sats = (uint8_t)(min(GPS.getSatellites(), (uint32_t)255));

        // A FAZER: em modo CLASS_A, desligar Vext após um tempo sem uso, para economizar bateria
        // digitalWrite(Vext, LOW);

        PositionQueue.enqueue(pos);
    }
    // Chamado quando deve enviar dados
    void onSend() override
    {
        const uint32_t PAYLOAD_SIZE = sizeof(Posicao);                          // dados da posição
        const uint32_t MAX_PAYLOAD_SIZE = LORAWAN_APP_DATA_MAX_SIZE - (32 + 4); // Margem de segurança (A FAZER: decidir melhor esse valor)
        const uint32_t BULK_PAYLOAD_MAX = MAX_PAYLOAD_SIZE / PAYLOAD_SIZE;      // Quantas posições cabem no payload

        uint32_t posicoes_faltando = PositionQueue.size();
        if (posicoes_faltando <= 0)
        {
            Serial.println("Nenhuma posição para enviar.");
            return;
        }

        // Quantas posições cabem no payload
        // [4 bytes contador] + N * [16 bytes posições]
        uint32_t posicoes_para_enviar = LoRaWANService.sendsWithoutAck() == 0 ? min(posicoes_faltando, BULK_PAYLOAD_MAX) : 1;
        Serial.print("Preparando envio de ");
        Serial.print(posicoes_para_enviar);
        Serial.println(" posições...");

        PositionQueue.resetSend();
        uint32_t inicio = (uint32_t)PositionQueue.getStart();
        LoRaWANService.writeAppData((unsigned char *)(&inicio), sizeof(inicio));

        Posicao pos;
        for (uint32_t i = 0; i < posicoes_para_enviar; i++)
        {
            //Posicao *pos = desenfileirar_posicao_envio();
            if (!PositionQueue.dequeueForSend(pos))
            {
                Serial.println("Erro ao obter posição da fila para envio.");
                break;
            }
            LoRaWANService.writeAppData((unsigned char *)(&pos), sizeof(Posicao));
        }

        Serial.print("Enviando posição Nº ");
        Serial.print(inicio);
        Serial.print(": ");
#if DEBUG_SERIAL
        LoRaWANService.printAppData();
#endif
    }
    // Chamado para confirmar que enviou a última mensagem
    void onSendAck() override
    {
        Serial.println("ACK RECEBIDO! O pacote foi confirmado pelo servidor.");

        char str_buf[15];
        snprintf(str_buf, sizeof(str_buf), "ACK: %d-%d", PositionQueue.getStart(), PositionQueue.getSendIndex()-1);
        st7735.st7735_write_str(0, 0, str_buf);
        //st7735.st7735_write_str(0, 0, (String) "ACK:" + (String)posicoesInicio + " ..." + (String)numero_posicoes()); // Escreve no display

        // Remove a posição da fila
        Serial.print("Marcando posições de Nº ");
        Serial.print(PositionQueue.getStart());
        Serial.print(" a ");
        Serial.print(PositionQueue.getSendIndex()-1);
        Serial.println(" como enviadas.");
        //posicoesInicio = posicoesEnvio;
        if (!PositionQueue.commitSend())
        {
            Serial.println("Aviso: commitSend chamado sem janela ativa.");
        }
    }
    // Retorna quantas mensagens estão pendentes para envio
    size_t getPendingMessages() override
    {
        return PositionQueue.size();
    }
};

MyApp MyAppInstance;

void setup()
{
    Serial.begin(115200);
    Serial.print("Inicializando ");

    // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
    // Configuração Timezone. Deve ser UTC pois o GPS fornece tempo em UTC
    configTzTime("UTC0", nullptr);

    // Enable Output 3.3V, power supply for built-in TFT and GNS
    Serial.print("1... ");
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, HIGH);

    // Start serial for GNS
    Serial.print("2... ");
    //Serial1.begin(115200, SERIAL_8N1, 33, 34);
    GPS.setup();

    // Limpa tela e exibe boas vindas
    Serial.print("3... ");
    delay(100);
    st7735.st7735_init();
    st7735.st7735_fill_screen(ST7735_BLACK);
    delay(100);
    st7735.st7735_write_str(0, 0,  "|Rastreio    |");
    st7735.st7735_write_str(0, 20, "|     LoRaWAN|");
    st7735.st7735_write_str(0, 40, "|------------|");
    st7735.st7735_write_str(0, 60, "|Erick L Weil|");

    Serial.print("4... ");
    LoRaWANService.setup();
    LoRaWANService.app = &MyAppInstance;

    Serial.println("OK!");
}

unsigned long millis_ultimo_sleep = 0;

void loop()
{
    if(Serial.available())
    {
        char c = Serial.read();
        if(c == 'r') {
            Serial.println("Imprimindo posições na fila: [");
            Posicao pos;
            PositionQueue.resetSend();
            while(PositionQueue.dequeueForSend(pos)) {
                Serial.print("\t{ \"timestamp\": ");
                Serial.print(pos.timestamp);
                Serial.print(", \"lat\": ");
                Serial.print(pos.lat);
                Serial.print(", \"lng\": ");
                Serial.print(pos.lng);
                Serial.print(", \"speed\": ");
                Serial.print(pos.speed);
                Serial.print(", \"course\": ");
                Serial.print(pos.course);
                Serial.print(", \"hdop\": ");
                Serial.print(pos.hdop);
                Serial.print(", \"sats\": ");
                Serial.print(pos.sats);
                Serial.println(" },");
            }
            Serial.println("]");
        } else if(c == 'p') {
            Serial.println("Lendo nova posição GPS por comando serial...");
            MyAppInstance.onTimer();
        }
    }

    LoRaWANService.loop();
}