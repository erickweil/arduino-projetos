#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <WeatherSensor.h>

// ==============================================================================
//                                  WeatherSensor
// https://github.com/Alxdelira/estacao_arduino/blob/main/hardware.ino
// Obs: Devido ao uso de registradores e interruptor esta parte do código só
// funciona no arduino UNO
#define RF_IN 2
WeatherSensor weather;

ISR(TIMER1_COMPA_vect)
{
  static byte count = 0;
  static byte was_hi = 0;

  if (digitalRead(RF_IN) == HIGH)
  {
      count++;
      was_hi = 1;
  }
  else
  {
      if (was_hi)
      {
          was_hi = 0;
          weather.interval = count;
          weather._interval = 1;
          count = 0;
      }
  }
}

void weather_initialize_timer() {
  TCCR1A = 0x00;
  TCCR1B = 0x09;
  TCCR1C = 0x00;
  OCR1A = 399;
  TIMSK1 = 0x02;

  pinMode(RF_IN, INPUT);
  sei(); //<-------enable interrupts--------
  Serial.println(F("Inicializado Weather Station Interrupts"));
}

void weather_print_serial() {
  Serial.println(F("--------------------------------------------------------------"));
  Serial.print(F("Tempo de intervalo de cada pacote recebido: "));                                  Serial.println(weather.spacing, DEC);
  Serial.print(F("Pacotes recebidos: "));   Serial.println(weather.packet_count, DEC);
  Serial.print(F("Sensor ID: 0x"));         Serial.println(weather.get_sensor_id(), HEX);
  Serial.print(F("Temperatura: "));         Serial.print(weather.get_temperature());                Serial.println(F(" C "));
  Serial.print(F("Humidade: "));            Serial.print(weather.get_humidity());                   Serial.println(F(" %\t"));
  Serial.print(F("Posicao do vento: "));    Serial.println(weather.get_wind_direction_str());
  // Serial.print("Posicao do vento em grau: ");    Serial.println(weather.get_wind_direction_deg());
  Serial.print(F("Velocidade do vento: ")); Serial.print(weather.get_wind_gust_kmh());              Serial.println(F(" Kmh"));
  Serial.print(F("Chuva "));                Serial.println(weather.get_rainfall());
  Serial.print(F("Bateria "));              Serial.println(weather.get_battery());
  Serial.println(F("--------------------------------------------------------------"));
}

bool weather_data_available() {
  bool ret = false;
  if (weather._interval) {
    weather.Receiver(weather.interval);  
    if (weather.acquired()) {
      weather.now = millis();
      weather.spacing = weather.now - weather.old;
      weather.old = weather.now;
      weather.packet_count++;
      weather.average_interval = weather.now / weather.packet_count;
      ret = true;
    }
    weather._interval = 0;
  }
  return ret;
}

// ===========================================================================
//                              Comunicação Ethernet
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress server_ip(192, 168, 1, 101);
EthernetClient client;

void ethernet_begin() {
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println(F("Ethernet cable is not connected."));
    }
    // try to congifure using IP address instead of DHCP:
    //Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print(F("  DHCP assigned IP "));
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}
// ===========================================================================
//                              Envio dados servidor
#include "arduino_secrets.h"
/* ./arduino_secrets.h
#define SERVER_PORT 80
#define SERVER_HOST F("estacoes-api.example.com")
#define TOKEN F("eyJhb...")
*/

/* just wrap the received data up to 80 columns in the serial print*/
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void server_print_response_serial() {
  uint32_t received_data_num = 0;
  while (client.available()) {
    /* actual data reception */
    char c = client.read();
    /* print data to serial port */
    Serial.print(c);
    if(c == '\r' || c == '\n') {
      received_data_num = 0;
    }
    
    received_data_num++;
    /* wrap data to 80 columns*/
    if(received_data_num % 80 == 0) {
      Serial.println();
      Serial.print("\t");
    }
  }
}

void server_post() {
  // Constrói o JSON da requisição
  StaticJsonDocument<200> doc;
  doc["spacing"] = weather.spacing;
  doc["packet_count"] = weather.packet_count;
  doc["sensor_id"] = weather.get_sensor_id();
  doc["temperature"] = weather.get_temperature();
  doc["humidity"] = weather.get_humidity();
  doc["wind_speed_kmh"] = weather.get_wind_gust_kmh();
  doc["rainfall"] = weather.get_rainfall();
  doc["battery"] = weather.get_battery();

  Serial.println(F("connecting..."));
  if (client.connect(server_ip, SERVER_PORT)) {
    Serial.println(F("connected"));

    client.println("POST /dados HTTP/1.1");
    client.print("Host: "); client.println(SERVER_HOST);
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Authorization: Bearer "); client.println(TOKEN);
    client.print("Content-Length: "); client.println(measureJson(doc));
    // Quebra de linha indicando fim dos headers
    client.println();

    // Corpo da requisição, com o tamanho corretamente informado no header Content-Length
    serializeJson(doc, client);

    // Espera um pouco para o servidor responder, isso talvez deveria ser feito diferente
    delay(250);

    // Escreve todos os headers e corpo da resposta no serial, caractere por caractere
    server_print_response_serial();

    client.stop();
  } else {
    Serial.println(F("connection failed"));
  }
}

// ===========================================================================
//
void setup() {
  Serial.begin(115200);
  Serial.println(F("Iniciando..."));

  // Inicializa os timers interruptores para funcionamento da biblioteca WeatherSensor
  weather_initialize_timer();
  ethernet_begin();
}

void loop() {
  delay(1);
  if(weather_data_available()) {
    weather_print_serial();
    server_post();
  }
}