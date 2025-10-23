#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoJson.h>

//=========================================================================================================================================================================================
#include <WeatherSensor.h>

#define RF_IN  2

//=========================================================================================================================================================================================

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


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server(192,168,1,101);
EthernetClient client;

//=========================================================================================================================================================================================

void setup() {
  TCCR1A = 0x00;
  TCCR1B = 0x09;
  TCCR1C = 0x00;
  OCR1A  = 399;
  TIMSK1 = 0x02;

  Serial.begin(115200);
  Serial.println("Iniciado!");
  Serial.println("Aguardando dados da estacao...");
  pinMode(RF_IN, INPUT);
  sei(); //<-------enable interrupts--------

  
  int res = Ethernet.begin(mac);
  Serial.print("Resultado: -->");
  Serial.println(res); 
}
//=========================================================================================================================================================================================

void loop() {
 
  byte i;
  byte *packet;
  
  if (weather._interval) 
  {
    weather.Receiver(weather.interval);  
    if (weather.acquired()) 
    {
      weather.now = millis();
      weather.spacing = weather.now - weather.old;
      weather.old = weather.now;
      weather.packet_count++;
      weather.average_interval = weather.now / weather.packet_count;  
      
      Serial.println("--------------------------------------------------------------");
      Serial.print("Tempo de intervalo de cada pacote recebido: ");                                  Serial.println(weather.spacing, DEC);
      Serial.print("Pacotes recebidos: ");   Serial.println(weather.packet_count, DEC);
      Serial.print("Sensor ID: 0x");         Serial.println(weather.get_sensor_id(), HEX);
      Serial.print("Temperatura: ");         Serial.print(weather.get_temperature());                Serial.println(" C ");
      Serial.print("Humidade: ");            Serial.print(weather.get_humidity());                   Serial.println(" %\t");
      Serial.print("Posicao do vento: ");    Serial.println(weather.get_wind_direction_str());
      // Serial.print("Posicao do vento em grau: ");    Serial.println(weather.get_wind_direction_deg());
      Serial.print("Velocidade do vento: "); Serial.print(weather.get_wind_gust_kmh());              Serial.println(" Kmh");
      Serial.print("Chuva ");                Serial.println(weather.get_rainfall());
      Serial.print("Bateria ");              Serial.println(weather.get_battery());
      Serial.println("--------------------------------------------------------------");   

      
       Serial.println("connecting...");

  if (client.connect(server, 6097)) {
//    Serial.println("connected");
//    client.print("GET /temp/");
//    client.print(weather.packet_count);
//    client.print("/");
//    client.print(weather.get_humidity());
//    client.println(" HTTP/1.0");
//    client.println();
Serial.println("connected");

 // Allocate the JSON document
  JsonDocument doc;

  // Adicionando os valores ao documento
  doc["spacing"] = weather.spacing;
  doc["packet_count"] = weather.packet_count;
  doc["sensor_id"] = weather.get_sensor_id();
  doc["temperature"] = weather.get_temperature();
  doc["humidity"] = weather.get_humidity();
  //doc["wind_direction"] = weather.get_wind_direction_str();
  // doc["wind_direction_deg"] = weather.get_wind_direction_deg(); // Descomente se quiser incluir
  doc["wind_speed_kmh"] = weather.get_wind_gust_kmh();
  doc["rainfall"] = weather.get_rainfall();
  doc["battery"] = weather.get_battery();

  client.println("POST /temp HTTP/1.1");
  client.print("Host: ");
  client.println("192.168.1.101:6097");  
  client.println("Content-Type: text/plain");
  client.println(F("Connection: close"));
  client.print(F("Content-Length: "));
  client.println(measureJsonPretty(doc));
  client.println();

  // Write JSON document
  serializeJsonPretty(doc, client);

  // Finalize a conexão
  client.println();

    

    delay(100);
    client.stop();
  } else {
    Serial.println("connection failed");
  }

   }
   weather._interval = 0; 
  }
delay(1);
}