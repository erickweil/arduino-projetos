#define LED 13
int v=0;
int p=0;
void setup()
{
Serial.begin(9600);  
pinMode(LED,OUTPUT);
}
void loop()
{
if(v<=p)
digitalWrite(LED,1);
else
digitalWrite(LED,0);
//Serial.println(v);
v++;
if(v>=100)
{
p++;  
v=0;
if(p>=100)
p=0;
}

}

void A(unsigned long vezes,byte porcent)
{
byte dela;  
dela = porcent/16;
for(int i =0;i<vezes/(dela+(16-dela));i++)
{ 
digitalWrite(LED, 1);
delay(dela);
digitalWrite(LED, 0);
delay(16-dela);
}

digitalWrite(LED, 0);
}




