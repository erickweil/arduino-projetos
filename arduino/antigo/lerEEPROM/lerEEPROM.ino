#include <EEPROM.h>
//#define LED 13
#define L 1024
#define TAXA 9600
void setup() {
Serial.begin(TAXA);  
//armazenarstring("Erick Leonardo Weil");
//Serial.println("comecou");Serial.println();
lerEEPROM();
}
void loop() {
if(Serial.available()>0)
{
Serial.println();
delay(200);
armazenarstring(lerlinha());
lerEEPROM();
Serial.println();
}

}

void lerEEPROM()
{
for (int i = 0; i < L; i++) {
byte leitura = EEPROM.read(i);
Serial.write(leitura);//Serial.print(" : ");Serial.print(leitura);Serial.print(" na pos : ");Serial.println(i);
if(i%64==0&&i>0)
Serial.println();
//digitalWrite(LED,leitura);
//delay(50);
}  
}

void armazenarstring(String txt)
{
for(int i=0;i<txt.length();i++)
{
EEPROM.write(i,txt.charAt(i));  
}
}

String lerlinha()
{
String tex="";  // cria uma string
while(Serial.available() > 0) // enquanto houver mais bytes de entrada
{ 
//Serial.print(".");  // dá um feedback de quantos caracteres foram analisados
tex +=(char) Serial.read(); // converte o byte em um char e então o concatena à string
}
return tex; // retorna a linha inteira que foi digitada
}
