#include<SoftwareSerial.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define WIFI_SSID "SSID-DA-REDE"
#define WIFI_PASS "SENHA-DA-REDE"

#define SERVER_IP 192,168,1,213
#define SERVER_PORT 3000

#define SERIAL_BAUDRATE 9600

// https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/examples/BasicHttpClient/BasicHttpClient.ino

//https://forum.arduino.cc/t/reading-serial-data-in-wrong-order/958157/13
SoftwareSerial SUART (2, 3);  //SRX = 2, STX = 3

int status = WL_IDLE_STATUS;
IPAddress server(SERVER_IP);
int port = SERVER_PORT;
WiFiClient client;

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  SUART.begin(SERIAL_BAUDRATE);

  Serial.println("ESP starts");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting...");
  while(WiFi.status()!=WL_CONNECTED){ //Loop which makes a point every 500ms until the connection process has finished
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP-Address: ");
  Serial.println(WiFi.localIP()); //Displaying the IP Address
}

void loop() {
  delay(500);
  if(WiFi.status()!=WL_CONNECTED){ //Loop which makes a point every 500ms until the connection process has finished
    Serial.println("Desconectado do WI-FI");
    return;
  }

  if(!client.connected()) {
    Serial.println("Conectando no servidor...");
    if(!client.connect(server,port)) {
      Serial.println("Falha ao conectar no servidor...");
      return;
    }

    client.print("Conectado! Listening:");
    client.print(SUART.isListening());
    client.print(" Error:");
    client.print(SUART.getWriteError());
    client.print(" Baudrate:");
    client.println(SUART.baudRate());

    client.println("------------------");
  }

  while(SUART.available() > 0) {
    //String line = Serial.readStringUntil('\r');
    //client.println(line);

    char c = SUART.read();
    client.write(c);
  }
  client.flush();
}
