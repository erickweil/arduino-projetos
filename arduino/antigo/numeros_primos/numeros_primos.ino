int led = 13;
long teste = 0;
void setup()
{
 pinMode(led,OUTPUT);
 Serial.begin(9600); 
}

void loop()
{
if(primo(teste))
{
Serial.print("o valor \'");  
Serial.print(teste);
Serial.println("\' faz parte dos primos");
A();
} 
teste++;
}
void A()
{
if(digitalRead(led) == LOW)  
digitalWrite(led,HIGH);
else
digitalWrite(led,LOW);
}
boolean primo(long n)
{  
for(int i=2;i<n;i++)
{
if(n%i==0)
return false;
}
return true;
}

