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

// Servidor web para visualização
#ifdef WIFI_SSID
#include "WifiService.h"
#endif

#if WEB_ADMIN
#include "WebServer.h"
WebServer server(80);
bool serverStarted = false;
bool webServerSetup();
void webServerLoop();
#endif

HT_st7735 st7735;
GPSClass GPS;
WifiServiceClass WifiService;
#ifdef POSICOES_IMPL_LITTLE_FS
#include <LittleFS.h>
#include "PositionQueueLittleFS.h"
PositionQueueLittleFS PositionQueue;
#endif
#ifdef POSICOES_IMPL_RTC
#include "PositionQueue.h"
PositionQueueRTC PositionQueue;
#endif
LoRaWANServiceClass LoRaWANService;

// ============================================================================
//                                Lógica Principal
// ============================================================================
class MyApp: public App
{
public:
    size_t lastSentIndex = SIZE_MAX;
    float lastCourse = -1.0f;

    void printarDisplayGPS() {
        char str_buf[15];

        #ifdef WIFI_SSID
                st7735.st7735_write_str(0, 0, WifiService.getIP());
        #endif

        tm t = GPS.getTime();
        snprintf(str_buf, sizeof(str_buf), "%02d:%02d:%02d %d.....", 
            t.tm_hour, t.tm_min, t.tm_sec, PositionQueue.pendingSend());
        st7735.st7735_write_str(0, 20, str_buf);

        snprintf(str_buf, sizeof(str_buf), "%.7f", GPS.getLatitude());
        st7735.st7735_write_str(0, 40, str_buf);
        snprintf(str_buf, sizeof(str_buf), "%.7f", GPS.getLongitude());
        st7735.st7735_write_str(0, 60, str_buf);
    }

    void lerDadosGPS(Posicao &pos) {
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
    }
    // Chamado periodicamente para registrar uma nova leitura
    void onTimer() override
    {
        char str_buf[15];

        // pinMode(Vext, OUTPUT);
        // digitalWrite(Vext, HIGH);
        st7735.st7735_fill_screen(ST7735_BLACK);
        snprintf(str_buf, sizeof(str_buf), "%d:%d GPS...", PositionQueue.getSendIndex(), PositionQueue.pendingSend());
        st7735.st7735_write_str(0, 0, str_buf);

        // Se a última posição foi registrada hà mais de 1 segundo, força um novo fix GPS
        uint32_t locationAge = GPS.getLocationAge();
        if(locationAge > 1000) {
            Serial.print("Última posição antiga, fix GPS... age:"); Serial.println(locationAge);
            // =========================================================
            // FIX GPS. execução bloquente, aguarda até conseguir um fix
            // pode demorar de 1 segundo, ou no pior dos casos ~2 min
            // =========================================================
            for (int i = 0; i < 50; i++)
            {
                bool gps_result = GPS.waitGpsInfo(3000U, i == 0);

                if (gps_result && GPS.isUpdated())
                    break;

                // Wait 1 sec before trying again...
                //delay(500);
            }
        }

        Posicao pos;
        lerDadosGPS(pos);

        // A FAZER: em modo CLASS_A, desligar Vext após um tempo sem uso, para economizar bateria
        // digitalWrite(Vext, LOW);

        PositionQueue.enqueue(pos);
        lastCourse = GPS.getCourseDeg();

        printarDisplayGPS();

        millisLastTime = millis();
    }
    // Chamado quando deve enviar dados
    void onSend() override
    {
        const uint32_t PAYLOAD_SIZE = sizeof(Posicao);                          // dados da posição
        const uint32_t MAX_PAYLOAD_SIZE = LORAWAN_APP_DATA_MAX_SIZE - (32 + 4); // Margem de segurança (A FAZER: decidir melhor esse valor)
        const uint32_t BULK_PAYLOAD_MAX = MAX_PAYLOAD_SIZE / PAYLOAD_SIZE;      // Quantas posições cabem no payload

        uint32_t posicoes_faltando = PositionQueue.pendingSend();
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

        //PositionQueue.resetSend();
        if(!PositionQueue.beginReadAt(PositionQueue.getSendIndex())) {
            Serial.println("Erro ao iniciar leitura das posições para envio.");
            return;
        }
        uint32_t inicio = (uint32_t)PositionQueue.getSendIndex();
        LoRaWANService.writeAppData((unsigned char *)(&inicio), sizeof(inicio));

        Posicao pos;
        for (uint32_t i = 0; i < posicoes_para_enviar; i++)
        {
            //Posicao *pos = desenfileirar_posicao_envio();
            if (!PositionQueue.readNext(pos))
            {
                Serial.println("Erro ao obter posição da fila para envio.");
                break;
            }
            LoRaWANService.writeAppData((unsigned char *)(&pos), sizeof(Posicao));
        }
        lastSentIndex = PositionQueue.endRead();

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
        snprintf(str_buf, sizeof(str_buf), "ACK %d-%d......", PositionQueue.getSendIndex(), lastSentIndex-1);
        st7735.st7735_write_str(0, 0, str_buf);
        //st7735.st7735_write_str(0, 0, (String) "ACK:" + (String)posicoesInicio + " ..." + (String)numero_posicoes()); // Escreve no display

        // Remove a posição da fila
        Serial.print("Marcando posições de Nº ");
        Serial.print(PositionQueue.getSendIndex());
        Serial.print(" a ");
        Serial.print(lastSentIndex-1);
        Serial.println(" como enviadas.");
        //posicoesInicio = posicoesEnvio;
        if(lastSentIndex == SIZE_MAX)
        {
            Serial.println("Aviso: commitSend chamado sem janela ativa.");
        }
        else 
        {
            PositionQueue.commitSend(lastSentIndex);
            lastSentIndex = SIZE_MAX;
        }
    }
    // Retorna quantas mensagens estão pendentes para envio
    size_t getPendingMessages() override
    {
        return PositionQueue.pendingSend();
    }
};

MyApp MyAppInstance;

void setupError(const char *msg)
{
    Serial.println(msg);
    st7735.st7735_fill_screen(ST7735_BLACK); delay(100);
    st7735.st7735_write_str(0, 0,  "Setup Error   ");
    st7735.st7735_write_str(0, 20, msg);
    st7735.st7735_write_str(0, 40, "              ");
    st7735.st7735_write_str(0, 60, "Rebooting 30s ");

    delay(30000);
    ESP.restart();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Inicializando...");

    // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
    // Configuração Timezone. Deve ser UTC pois o GPS fornece tempo em UTC
    configTzTime("UTC0", nullptr);

    // Enable Output 3.3V, power supply for built-in TFT and GNS
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, HIGH);

    // Start serial for GNS
    Serial.println("GPS... ");
    GPS.setup();

    // Limpa tela e exibe boas vindas
    Serial.println("Display... ");
    delay(100);
    st7735.st7735_init();
    st7735.st7735_fill_screen(ST7735_BLACK); delay(100);
    st7735.st7735_write_str(0, 0,  "|Rastreio ...|");
    st7735.st7735_write_str(0, 40, "|------------|");
    st7735.st7735_write_str(0, 60, "|Erick L Weil|");

    Serial.println("LoRaWAN... ");
    st7735.st7735_write_str(0, 20, "|     LoRaWAN|");
    if(!LoRaWANService.setup(&MyAppInstance)) {
        setupError("Setup LoRaWAN");
    }
#ifdef WIFI_SSID
    Serial.println("WiFi... ");
    st7735.st7735_write_str(0, 20, "|        WiFi|");    
    if(!WifiService.setup()) {
        setupError("Setup WiFi");
    }
#endif
#if WEB_ADMIN
    Serial.println("WebServer... ");
    st7735.st7735_write_str(0, 20, "|   WebServer|");
    if(!webServerSetup()) {
        setupError("Setup WebServer");
    }
#endif

#ifdef POSICOES_IMPL_LITTLE_FS
    //  You only need to format LittleFS the first time you run a
    //  test or else use the LITTLEFS plugin to create a partition 
    //  https://github.com/lorol/arduino-esp32littlefs-plugin
    Serial.println("LittleFS... ");
    st7735.st7735_write_str(0, 20, "|    LittleFS|");
    if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
        setupError("Mount LittleFS");
    }

    Serial.println("PositionQueue... ");
    st7735.st7735_write_str(0, 20, "|       Queue|");
    if(!PositionQueue.begin()) {
        setupError("Begin Queue");
    }
#endif
    Serial.println("OK!");
    st7735.st7735_write_str(0, 20, "|          OK|");

    // Serial.println("Fix GPS inicial... ");
    // st7735.st7735_write_str(0, 20, "|Fix GPS...  |");
    // // =========================================================
    // // FIX GPS inicial. execução bloqueante
    // // =========================================================
    // for (int i = 0; i < 200; i++)
    // {
    //     bool gps_result = GPS.waitGpsInfo(3000U, i == 0);
    //     if (gps_result) break;
    // }
    // MyAppInstance.printarDisplayGPS();
}

void loop()
{
    // Só deveria acompanhar o GPS assim em funcionamento Classe C
    bool gpsValid = GPS.loop();
    // Identifica que houve uma mudança grande no ângulo desde a última leitura
    if(gpsValid && MyAppInstance.lastCourse >= 0.0f && GPSClass::absCourseDiff(MyAppInstance.lastCourse, GPS.getCourseDeg()) > ANGULO_MINIMO)
    {
        // Mesmo que detectar mudança de ângulo, no máximo irá registrar a cada 2 segundos (vai que dá um bug aí né)
        if (millis() - MyAppInstance.millisLastTime > 2000)
        {
            Serial.println("Detectado mudança de ângulo, lendo nova posição...");
            MyAppInstance.onTimer();
        }
    }

    LoRaWANService.loop();
#ifdef WIFI_SSID
    WifiService.loop();
#endif
#if WEB_ADMIN
    webServerLoop();
#endif
}

// ============================================================================
//                          Página Administração Web
// ============================================================================
#if WEB_ADMIN
// js/index.html https://www.toptal.com/developers/javascript-minifier https://kangax.github.io/html-minifier/
const char HtmlIndexPage[] = R"rawliteral(
<title>Rastreador - Wireless Tracker</title><meta content="width=device-width,initial-scale=1"name=viewport><style>#table-container{width:90%;overflow-x:auto}table{width:100%;border-collapse:collapse;margin-top:15px}td,th{border:1px solid #ddd;padding:8px;text-align:left}th{background-color:#f2f2f2}</style><body style=display:flex;flex:1;flex-direction:column;align-items:center><h2>Rastreador - Wireless Tracker</h2><button onclick=myrefresh() style=padding:8px>Atualizar</button><div id=info></div><div id=table-container><p style=padding:20px;text-align:center>Carregando dados...</div><script>let jsonData;async function myrefresh(){let t=document.getElementById("table-container"),a=document.getElementById("info");t.innerHTML="<p style='padding: 20px; text-align: center;'>Atualizando...</p>",jsonData=await fetch("/info",{method:"GET",headers:{Accept:"application/json"}}).then(t=>t.json());let e=jsonData.LoRaWAN,n="<b>LoRaWAN:</b><br/>";n+=`<span>Mensagens na fila: ${e.queued}, Sem ACK: ${e.withoutAck}</span><br/>`,n+=`<span>devEui: ${e.devEui}, appEui: ${e.appEui}, appKey: ${e.appKey}</span><br/>`;let o=jsonData.GPS;n+="<b>GPS:</b><br/>",n+=`<span>Data: ${new Date(o.data).toLocaleString()}</span><br/>`;let d=`${o.coords[0]},${o.coords[1]}`;n+=`<span>Coordenadas: <a href="https://maps.google.com/?q=${d}" target="_blank">${d}</a></span><br/>`,n+=`<span>Velocidade: ${o.vel} km/h, Dire\xe7\xe3o: ${o.dir}\xb0, HDOP: ${o.hdop}, Sat\xe9lites: ${o.satellites}</span><br/>`,a.innerHTML=n;let s="<table>";s+="<thead><tr>",s+="<th>N</th><th>Data</th><th>Coords</th><th>Vel</th><th>Dir</th><th>HDOP</th><th>Sats</th>",s+="</tr></thead>",s+="<tbody>";let r=jsonData.historico;r.forEach(t=>{s+="<tr>",s+=`<td>${t.n}</td>`,s+=`<td>${new Date(t.data).toLocaleString()}</td>`;let a=`${t.coords[0]},${t.coords[1]}`;s+=`<td><a href="https://maps.google.com/?q=${a}" target="_blank">${a}</a></td>`,s+=`<td>${t.vel} km/h</td>`,s+=`<td>${t.dir}\xb0</td>`,s+=`<td>${t.hdop}</td>`,s+=`<td>${t.satellites}</td>`,s+="</tr>"}),s+="</tbody></table>",t.innerHTML=s}window.onload=function(){myrefresh(),setInterval(myrefresh,3e4)};</script>
)rawliteral";

void handleRoot()
{
    server.send(200, "text/html", HtmlIndexPage);    
}

void handlePositions()
{
    Posicao pos;
    char str_buf[512];
    int len_bytes = 0;

    // Obter querys
    size_t page = 1;
    if(server.hasArg("page"))
    {
        page = max(min(server.arg("page").toInt(), 1000000000L), 1L);
    }

    size_t limit = 100;
    if(server.hasArg("limit"))
    {
        limit = max(min(server.arg("limit").toInt(), 1000L), 1L);
    }

    // Para enviar chunked.
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "application/json", "{\n");

    // Informações gerais:
    // LoRaWAN: eui, addr, rssi, snr, acks,
    // GPS: ultima posição, fix,
    // Fila: tamanho, posições armazenadas, posições enviadas, últimas posições enviadas
    
    len_bytes = 0;
    len_bytes += snprintf(str_buf, sizeof(str_buf), "\"LoRaWAN\":");
    len_bytes += LoRaWANService.infoJson(str_buf + len_bytes, sizeof(str_buf) - len_bytes);
    server.sendContent(str_buf, len_bytes);

    MyAppInstance.lerDadosGPS(pos);
    len_bytes = 0;
    len_bytes += snprintf(str_buf, sizeof(str_buf), ",\n\"GPS\":");
    len_bytes += PositionQueueClass::toJson(pos, 0, str_buf + len_bytes, sizeof(str_buf) - len_bytes);
    server.sendContent(str_buf, len_bytes);

    server.sendContent(",\n\"historico\":[\n");
    //size_t queueMaxSize = PositionQueue.capacity() + 1;
    //size_t posIndex = (PositionQueue.getStart() + (page - 1) * limit) % queueMaxSize;
    size_t repeats = min(PositionQueue.size(), limit);
    size_t posicao = (PositionQueue.getStart() + (page - 1) * limit) % (PositionQueue.capacity() + 1);
    PositionQueue.beginReadAt(posicao);
    while(repeats > 0 && PositionQueue.readNext(pos)) {
        len_bytes = 0;
        len_bytes += PositionQueueClass::toJson(pos, posicao, str_buf, sizeof(str_buf));
        if(repeats > 1) {
            len_bytes += snprintf(str_buf + len_bytes, sizeof(str_buf) - len_bytes, ",\n");
        }
        server.sendContent(str_buf, len_bytes);

        posicao = (posicao + 1) % (PositionQueue.capacity() + 1);
        repeats--;
    }
    PositionQueue.endRead();
    server.sendContent("\n]}\n");
}

void handleNotFound()
{
    server.send(404, "text/plain", "Not found");
}

bool webServerSetup()
{
    // Inicializa o servidor web não bloqueante
    server.on("/", handleRoot);
    server.on("/info", handlePositions);
    server.onNotFound(handleNotFound);
    if(!MDNS.addService("http", "tcp", 80)) {
        Serial.println("Erro ao registrar serviço mDNS");
        return false;
    }

    return true;
}

void webServerLoop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        if(serverStarted) {
            server.stop();
            serverStarted = false;
        }
        return;
    }

    if (!serverStarted)
    {
        server.begin();
        serverStarted = true;
        return;
    }

    server.handleClient();
}
#endif