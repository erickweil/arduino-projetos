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
#include "HT_TinyGPS++.h"

// Para conversões do tempo
#include <time.h>

// Configurações do projeto (LoRaWAN, etc)
#include "config.h"

#include "PositionQueue.h"

TinyGPSPlus GPS;
HT_st7735 st7735;

// extern's LoRaWan_APP.h
// Não alterar os nomes das variáveis ou quebra o código

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
uint8_t confirmedNbTrials = 6;

/**
 * https://berthub.eu/articles/posts/how-to-get-a-unix-epoch-from-a-utc-date-time-string/
 * https://en.cppreference.com/w/c/chrono/tm
 */
tm read_gps_time()
{
    return {
        .tm_sec = GPS.time.second(),       //  seconds after the minute – [​0​, 61](until C99)[​0​, 60](since C99)[note 1]
        .tm_min = GPS.time.minute(),       //  minutes after the hour – [​0​, 59]
        .tm_hour = GPS.time.hour(),        //  hours since midnight – [​0​, 23]
        .tm_mday = GPS.date.day(),         //  day of the month – [1, 31]
        .tm_mon = GPS.date.month() - 1,    //  months since January – [​0​, 11]
        .tm_year = GPS.date.year() - 1900, //  years since 1900
        .tm_isdst = 0                      //  Daylight Saving Time flag. The value is positive if DST is in effect, zero if not and negative if no information is available
    };
}

/**
 * Processa os dados do GPS até que não haja mais linhas.
 * Caso demore mais de 15 segundos sem chegar nenhum dado ou em uma linha pela metade desiste.
 */
bool wait_gps_info()
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
            GPS.encode(c);
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

    if (!GPS.time.isValid())
    {
        Serial.println("Tempo inválido, tentando novamente...");
        return false;
    }

    return true;
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

void write_app_data(unsigned char *puc, int size = 4)
{
    for (int i = 0; i < size; i++)
    {
        if (appDataSize >= LORAWAN_APP_DATA_MAX_SIZE)
        {
            Serial.println("appDataSize excedeu LORAWAN_APP_DATA_MAX_SIZE!");
            return;
        }
        appData[appDataSize++] = puc[i];
    }
}

void print_app_data()
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

RTC_DATA_ATTR uint32_t mensagens_sem_ack = 0;

// BUG/A FAZER: em CLASS_A, millis() zera após deep sleep. Não pode depender de millis() para controlar tempos longos.
// RTC_DATA_ATTR unsigned long millis_ultima_leitura = 0;

// Será true quando estiver descarregando a fila de posições para envio (Evitar ler posições novas enquanto isso)
RTC_DATA_ATTR bool descarregando_fila = false;

/* Prepares the payload of the frame */
static void lerPosicao()
{
    /*if (millis_ultima_leitura != 0 && millis() - millis_ultima_leitura < appTxDutyCycle - 5000)
    {
        Serial.println("Leitura GPS ignorada: muito próxima da última leitura.");
        return;
    }
    millis_ultima_leitura = millis();*/

    // pinMode(Vext, OUTPUT);
    // digitalWrite(Vext, HIGH);

    // ~2 min Fix GPS
    char str_buf[15];
    for (int i = 0; i < 120; i++)
    {
        bool gps_result = wait_gps_info();
        bool gps_valid = GPS.location.isValid() && GPS.location.isUpdated();

        st7735.st7735_fill_screen(ST7735_BLACK);
        // A FAZER: parar de usar classe String
        //char buf[30]; // Buffer para formatar strings
        //snprintf(buf, 30, "Na fila: %d", numero_posicoes());
        snprintf(str_buf, sizeof(str_buf), "%d ...%d", PositionQueue.getStart(), PositionQueue.size());
        st7735.st7735_write_str(0, 0, str_buf);

        snprintf(str_buf, sizeof(str_buf), "%02d:%02d:%02d %s", 
            GPS.time.hour(), GPS.time.minute(), GPS.time.second(), gps_valid ? "FIX" : "...");
        st7735.st7735_write_str(0, 20, str_buf);

        snprintf(str_buf, sizeof(str_buf), "%.7f", GPS.location.lat());
        st7735.st7735_write_str(0, 40, str_buf);
        snprintf(str_buf, sizeof(str_buf), "%.7f", GPS.location.lng());
        st7735.st7735_write_str(0, 60, str_buf);

        if (gps_result && gps_valid)
            break;

        // Wait 1 sec before trying again...
        delay(1000);
    }

    Posicao pos = {0, 0, 0, 0, 0, 0, 0};
    tm t = read_gps_time();
    /** Obter unix timestamp (seconds since 1970-01-01) válido a partir dos valores do GPS
            https://pubs.opengroup.org/onlinepubs/009696799/functions/mktime.html              */
    pos.timestamp = (uint32_t)mktime(&t);

    // Converter latitude e longitude para formato inteiro em degrees*(ten million)
    pos.lat = rawDegreesToInt32(GPS.location.rawLat());
    pos.lng = rawDegreesToInt32(GPS.location.rawLng());

    pos.course = (uint8_t)((GPS.course.deg() * 255.0 / 360.0) + 0.5);
    pos.speed = (uint8_t)(min(GPS.speed.kmph() + 0.5, 255.0));
    pos.hdop = (uint8_t)(min(GPS.hdop.hdop() * 10, 255.0) + 0.5);
    pos.sats = (uint8_t)(min(GPS.satellites.value(), (uint32_t)255));

    // A FAZER: em modo CLASS_A, desligar Vext após um tempo sem uso, para economizar bateria
    // digitalWrite(Vext, LOW);

    PositionQueue.enqueue(pos);
}

/**
    appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
     *appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
     *if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
     *if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
     *for example, if use REGION_CN470,
     *the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
     */
// H  E  L  L  O  \0
// 48 45 4C 4C 4F 00
/*appDataSize = 0;
appData[appDataSize++] = 0x48;
appData[appDataSize++] = 0x45;
appData[appDataSize++] = 0x4C;
appData[appDataSize++] = 0x4C;
appData[appDataSize++] = 0x4F;
appData[appDataSize++] = 0x00;
*/
static void prepareTxFrame(uint8_t port)
{
    const uint32_t PAYLOAD_SIZE = sizeof(Posicao);                          // dados da posição
    const uint32_t MAX_PAYLOAD_SIZE = LORAWAN_APP_DATA_MAX_SIZE - (32 + 4); // Margem de segurança (A FAZER: decidir melhor esse valor)
    const uint32_t BULK_PAYLOAD_MAX = MAX_PAYLOAD_SIZE / PAYLOAD_SIZE;      // Quantas posições cabem no payload

    appDataSize = 0;

    uint32_t posicoes_faltando = PositionQueue.size();
    if (posicoes_faltando <= 0)
    {
        Serial.println("Nenhuma posição para enviar.");
        return;
    }

    // Quantas posições cabem no payload
    // [4 bytes contador] + N * [16 bytes posições]
    uint32_t posicoes_para_enviar = descarregando_fila ? min(posicoes_faltando, BULK_PAYLOAD_MAX) : 1;
    Serial.print("Preparando envio de ");
    Serial.print(posicoes_para_enviar);
    Serial.println(" posições...");

    PositionQueue.resetSend();
    uint32_t inicio = (uint32_t)PositionQueue.getStart();
    write_app_data((unsigned char *)(&inicio), 4);

    Posicao pos;
    for (uint32_t i = 0; i < posicoes_para_enviar; i++)
    {
        //Posicao *pos = desenfileirar_posicao_envio();
        bool ok = PositionQueue.dequeueForSend(pos);
        if (!ok)
        {
            Serial.println("Erro ao obter posição da fila para envio.");
            break;
        }
        write_app_data((unsigned char *)(&pos), sizeof(Posicao));
    }

    Serial.print("Enviando posição Nº ");
    Serial.print(inicio);
    Serial.print(": ");
#if DEBUG_SERIAL
    print_app_data();
#endif
}

/**
 * Handle any downlink acknowledgment
 * Called when a downlink is received
 */
void downLinkAckHandle()
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
    PositionQueue.commitSend();
    mensagens_sem_ack = 0;
}

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
    Serial1.begin(115200, SERIAL_8N1, 33, 34);

    // Limpa tela e exibe boas vindas
    Serial.print("3... ");
    delay(100);
    st7735.st7735_init();
    st7735.st7735_fill_screen(ST7735_BLACK);
    delay(100);
    st7735.st7735_write_str(0, 0, "Rastreio LoRaWAN");

    Serial.print("4... ");
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    Serial.println("OK!");
}

unsigned long millis_ultimo_sleep = 0;

void loop()
{
    switch (deviceState)
    {
    case DEVICE_STATE_INIT:
    {
        Serial.println("DEVICE_STATE_INIT");
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
        Serial.println("DEVICE_STATE_JOIN");
        st7735.st7735_write_str(0, 0, "join>>>      ");
        LoRaWAN.join();
        st7735.st7735_write_str(0, 0, "join>>>   ok ");
        break;
    }
    case DEVICE_STATE_SEND:
    {
        Serial.println("DEVICE_STATE_SEND");
        if (PositionQueue.size() <= 0 || descarregando_fila == false)
        {
            lerPosicao();
        }
        prepareTxFrame(appPort);
        descarregando_fila = false;

        if (appDataSize > 0)
        {
            mensagens_sem_ack++;
            LoRaWAN.send();
        }
        else
        {
            Serial.println("Nenhum dado para enviar.");
        }

        millis_ultimo_sleep = millis();
        deviceState = DEVICE_STATE_CYCLE;
        break;
    }
    case DEVICE_STATE_CYCLE:
    {
        Serial.println("DEVICE_STATE_CYCLE");

        uint32_t dutyCycleTime = appTxDutyCycle;
        // Se recebeu ACK recentemente e há mais posições na fila, envia mais rápido
        if (mensagens_sem_ack <= 3 && PositionQueue.size() > 1)
        {
            dutyCycleTime = 5000; // Envia próximo pacote em 5 segundos se houver mais posições na fila
            descarregando_fila = true;

            Serial.print("Mais posições na fila, enviando próximo pacote em 5 segundos... ");
            Serial.print("( faltam ");
            Serial.print(PositionQueue.size());
            Serial.println(" posições )");
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
        if (millis_ultimo_sleep == 0) millis_ultimo_sleep = millis();
        else if (millis() - millis_ultimo_sleep > (appTxDutyCycle + 10000))
        {
            millis_ultimo_sleep = millis();
            Serial.println("Tempo sem leituras GPS excedido, lendo nova posição...");
            lerPosicao();

            // TESTE APENAS ENQUANTO NÃO TEM LORAWAN
            // prepareTxFrame(appPort);
        }
        // Classe A irá colocar em deep sleep.
        // Classe C deixa ligado para receber downlinks.
        LoRaWAN.sleep(loraWanClass);
        break;
    }
    default:
    {
        Serial.print("DEFAULT:");
        Serial.println(deviceState);
        deviceState = DEVICE_STATE_INIT;
        break;
    }
    }
}