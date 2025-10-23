/*
  echo 
  reenvia para o computador o dado recebido pela serial
*/
 #include <EEPROM.h>
byte byteRead;
boolean talendo=false; 
int i =0;
void setup() {                
//configura a comunicação seria com baud rate de 9600
  Serial.begin(1200);
}
 
void loop() {
  
  if(Serial.available())  //verifica se tem dados diponível para leitura
  {
//Serial.print(".");  // dá um feedback de quantos caracteres foram analisados
EEPROM.write(i,(char) Serial.read()); // converte o byte em um char e então o concatena à string
i++;  //reenvia para o computador o dado recebido
Serial.write(EEPROM.read(i-1));
  }
}


