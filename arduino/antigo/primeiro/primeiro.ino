/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int incomingByte;
int meubyte;
String entrada;
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);  
}

// the loop routine runs over and over again forever:
void loop() {
  //digitalWrite(led, HIGH);
  //Serial.println("Ligou");  // turn the LED on (HIGH is the voltage level)
  //delay(1000);               // wait for a second
  //digitalWrite(led, LOW);
  //Serial.println("Apagou");  // turn the LED off by making the voltage LOW
  //delay(1000); // wait for a second
  // see if there's incoming serial data:

  while(Serial.available() > 0)
{
meubyte = Serial.read();
Serial.write(meubyte);
entrada += meubyte;
}

  if (Serial.available() > 0) {
    entrada = "";
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    // if it's a capital H (ASCII 72), turn on the LED:
    if (incomingByte == 'H') {
      A(1,led);
    } 
    // if it's an L (ASCII 76) turn off the LED:
    if (incomingByte == 'L') {
      A(0,led);
    }
  }

//if (Serial.available() > 0)
//Serial.println(entrada);
}



void A(int estado,int qual)
{
if(estado == 1)
{
digitalWrite(qual, HIGH);
Serial.print("Ligou o led ");
Serial.println(qual);
}
if(estado == 0)
{
digitalWrite(led, LOW);
Serial.print("Apagou o led ");
Serial.println(qual);
}
}
