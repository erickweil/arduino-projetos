#include <Servo.h>

const int servoPin = 5;

Servo servo;

void setup()
{
  servo.attach(servoPin);
  servo.write(0);
  Serial.begin(9600);
  Serial.println("Enter angle in degrees");
}

void loop()
{ 
  if(Serial.available())
  {
    int angle = Serial.parseInt();
    servo.writeMicroseconds(angle);
    Serial.print(servo.read());
    Serial.println("   OK");
    if(angle == 90){
      servo.detach();
    }else if(angle!=90){
      servo.attach(servoPin);
    }
  }
}
