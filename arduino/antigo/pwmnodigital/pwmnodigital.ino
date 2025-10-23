byte porcent=50;
byte umbyte=0;
byte lol = 2;
byte olo = 1;
void setup()
{
pinMode(13,OUTPUT);  
Serial.begin(9600);
}

void loop()
{
for(byte i=0;i<100;i++)
{
if(i<porcent)
digitalWrite(13,HIGH);
else
digitalWrite(13,LOW);
espera(10);
}
if(Serial.available())
{ 
porcent = Serial.read();  
}
}

void espera(unsigned long b)
{
for(unsigned long i =0;i<b;i++)
{
lol = olo;
}
}

float proxnum()
{  
umbyte =Serial.read();// Lê o prox byte na porta serial
while(umbyte == ' ')
umbyte =Serial.read();
String num="";
if(umbyte == '-'||umbyte == '+')
{
num += (char)umbyte;
umbyte= Serial.read();
}
while(isDigit(umbyte)||umbyte=='.'||umbyte==' ') // enquanto os bytes que forem lidos ainda forem digitos ou um ponto
{
if(umbyte != ' ')  
num += (char)umbyte; // converte o byte ASCII em um char, e então concatena a string 'num'
umbyte = Serial.read(); // Lê o prox byte na porta serial
}
if(num == ""){
Serial.print(" ---- Faltou um numero aqui! ----- ");return(0);}
return(atof(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
// atof é abreviatura de "ASCII to Float" , e c_str() faz a conversão de String para um array de char ( char[] )
/*
por (atof) ser uma função do c++, ela apenas trabalha com char[] e não com String ( faz parte das livrarias do arduino ) , por isso a necessidade de
converter o string em um array de chars antes de converter em um float
*/
}

