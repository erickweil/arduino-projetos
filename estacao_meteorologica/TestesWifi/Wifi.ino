#include <Arduino.h>
#include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define WIFI_SSID "SSID-DA-REDE"
#define WIFI_PASS "SENHA-DA-REDE"

// https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/examples/BasicHttpClient/BasicHttpClient.ino

int status = WL_IDLE_STATUS;
IPAddress server(192,168,1,213);
int port = 3000;

// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-class.html
// https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClient.h
WiFiClient client;

void setup() {
  Serial.begin(115200); //Baudrate
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
  }

  if(Serial.available() > 0) {
    // https://www.arduino.cc/reference/pt/language/functions/communication/serial/readstringuntil/
    String line = Serial.readStringUntil('\r');
    
    client.println(line);
  }
}
