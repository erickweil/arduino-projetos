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

// Servidor web para visualização
#ifdef WIFI_SSID
#include "WifiService.h"
#endif

HT_st7735 st7735;

#if WEB_ADMIN
#include "WebServer.h"
WebServer server(80);
bool serverStarted = false;
void webServerSetup();
void webServerLoop();
#endif

// ============================================================================
//                                Lógica Principal
// ============================================================================
class MyApp: public App
{
public:
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
        // pinMode(Vext, OUTPUT);
        // digitalWrite(Vext, HIGH);

        // ~2 min Fix GPS
        char str_buf[15];
        for (int i = 0; i < 15; i++)
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
            delay(500);
        }

        Posicao pos;
        lerDadosGPS(pos);

        // A FAZER: em modo CLASS_A, desligar Vext após um tempo sem uso, para economizar bateria
        // digitalWrite(Vext, LOW);

        PositionQueue.enqueue(pos);

#ifdef WIFI_SSID
        st7735.st7735_write_str(0, 0, WifiService.getIP());
#endif
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

#ifdef WIFI_SSID
    WifiService.setup();
#endif
#if WEB_ADMIN
    webServerSetup();
#endif
    Serial.println("OK!");
}

void loop()
{
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
    size_t count = PositionQueue.getStart();
    size_t pos_counter = 0;
    size_t size = PositionQueue.capacity();
    while(pos_counter < size && PositionQueue.getAt(pos_counter++, pos)) {
        len_bytes = 0;
        if(pos_counter > 1) {
            len_bytes += snprintf(str_buf, sizeof(str_buf), ",\n");
        }
        len_bytes += PositionQueueClass::toJson(pos, count++, str_buf + len_bytes, sizeof(str_buf) - len_bytes);
        server.sendContent(str_buf, len_bytes);
    }
    server.sendContent("]}\n");
}

void handleNotFound()
{
    server.send(404, "text/plain", "Not found");
}

void webServerSetup()
{
    // Inicializa o servidor web não bloqueante
    server.on("/", handleRoot);
    server.on("/info", handlePositions);
    server.onNotFound(handleNotFound);
    MDNS.addService("http", "tcp", 80);
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