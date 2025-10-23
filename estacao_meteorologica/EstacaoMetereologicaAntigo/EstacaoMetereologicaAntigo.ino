/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3 and 4  in a single ThingSpeak update every 20 seconds.
  
  Hardware: Arduino Ethernet
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
 - Requires the Ethernet library
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  #define SECRET_MAC {0x90, 0xA2, 0xDA, 0x10, 0x40, 0x4F}

#define SECRET_CH_ID 0000000      // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "0000000000000000"  
  Copyright 2018, The MathWorks, Inc.
*/
 
#include "ThingSpeak.h"
#include <Ethernet.h>
#include <SPI.h>
#define SECRET_MAC {0x90, 0xA2, 0xDA, 0x10, 0x40, 0x4F}
#define SECRET_CH_ID 0000000 
#define SECRET_WRITE_APIKEY "0000000000000000"  


byte mac[] = SECRET_MAC;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 101);
IPAddress myDns(192, 168, 0, 1);

EthernetClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
int number1 = 0;
int number2 = random(0,100);
int number3 = random(0,100);
int number4 = random(0,100);

//=========================================================================================================================================================================================
#include "WeatherSensor.h"

#define RF_IN  2

//========================================CODIGO ESTAÇÃO=================================================================================================================================================

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
//=================================================================================================================================


void setup() {
  TCCR1A = 0x00;
  TCCR1B = 0x09;
  TCCR1C = 0x00;
  OCR1A  = 399;
  TIMSK1 = 0x02;
  
  Ethernet.init(10);  // Most Arduino Ethernet hardware
  Serial.begin(115200);  //Initialize serial
  Serial.println("Iniciado!");
  Serial.println("Aguardando dados da estacao...");
  pinMode(RF_IN, INPUT);
  sei(); //<-------enable interrupts--------
    
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak

   //server.begin(); 
}

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
   }
   weather._interval = 0; 
  }
  //====================================================fim===================================//
  // set the fields with the values
  ThingSpeak.setField(1, weather.get_temperature()); //number1
  ThingSpeak.setField(2, weather.get_humidity());
  ThingSpeak.setField(3, weather.get_wind_direction_str());
  ThingSpeak.setField(4, weather.get_wind_gust_kmh());

   
  
  // write to the ThingSpeak channel 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the values
  number1++;
  if(number1 > 99){
    number1 = 0;
  }
  number2 = random(0,100);
  number3 = random(0,100);
  number4 = random(0,100);
  
  delay(20000); // Wait 20 seconds to update the channel again
}