/*
  TLS WiFi Web client

  Board CA Root certificate bundle is embedded inside WiFi firmware:
  https://github.com/arduino/uno-r4-wifi-usb-bridge/blob/main/certificates/cacrt_all.pem

  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#wi-fi-web-client-ssl
*/
#include "Arduino_LED_Matrix.h"
#include "WiFiS3.h"
#include "WiFiSSLClient.h"
#include "IPAddress.h"
#include "ArduinoJson.h"

#include "arduino_secrets.h"
/* ./arduino_secrets.h
#define SECRET_SSID "WIFI-SSID"
#define SECRET_PASS "WIFI-PASSWORD"
#define TOKEN "eyJhb...." 
*/

// https://docs.arduino.cc/tutorials/uno-r4-wifi/led-matrix/
ArduinoLEDMatrix matrix;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(192,168,1,101);  // numeric IP for Google (no DNS)
char server[] = "estacoes-api.example.com";    // name address for Google (using DNS)
char route[] = "/dados";

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiSSLClient client;

/* -------------------------------------------------------------------------- */
// Envia os dados da estação, por enquanto fake.
/* -------------------------------------------------------------------------- */
void sendWeatherData() {
  // Constrói o JSON da requisição
  StaticJsonDocument<200> doc;
  doc["temperature"] = 90;
  doc["humidity"] = 77;
  doc["rainfall"] = 5.4;
  doc["wind_speed_kmh"] = 20;
  doc["data_hora"] = "2025-02-21T21:25:21.663Z";

  // Envia Requisição e Headers
  client.print("POST "); client.print(route); client.println(" HTTP/1.1");
  client.print("Host: "); client.println(server);
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
  printResponseToSerial();
}

/* just wrap the received data up to 80 columns in the serial print*/
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
void printResponseToSerial() {
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

/* -------------------------------------------------------------------------- */
void printWifiStatus() {
/* -------------------------------------------------------------------------- */
  if(status == WL_CONNECTED) {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
  } else {
    Serial.println("Wifi not connected");
  }
}

// Executa uma vez só, ao inicializar o aparelho
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    delay(1); // wait for serial port to connect. Needed for native USB port only
  }

  matrix.begin();
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue

    matrix.loadFrame(LEDMATRIX_DANGER);
    while (true) {
      delay(1000);
    }
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  matrix.loadFrame(LEDMATRIX_UNO);
}

// Executa toda vez, como um while(true)
void loop() {
  // attempt to connect to WiFi network:
  if (status != WL_CONNECTED) {
    matrix.loadFrame(LEDMATRIX_CLOUD_WIFI);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network.
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);

    printWifiStatus();

    if(status != WL_CONNECTED) {
      matrix.loadFrame(LEDMATRIX_EMOJI_SAD);      
      delay(1000);
    }

    return;
  }

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  matrix.loadFrame(LEDMATRIX_EMOJI_BASIC);
  if (client.connect(server, 443)) {
    matrix.loadFrame(LEDMATRIX_EMOJI_HAPPY);
    Serial.println("connected to server");
    sendWeatherData();

    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    matrix.loadFrame(LEDMATRIX_LIKE);
  } else {
    matrix.loadFrame(LEDMATRIX_DANGER);
  }

  delay(15000);
}