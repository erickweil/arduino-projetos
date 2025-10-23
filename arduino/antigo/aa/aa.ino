byte umbyte;
void setup()
{
Serial.begin(9600);  
}
void loop()
{
if(Serial.available())
{
delay(50);
Serial.println(proxlong());
}
}

String proxlinha(char div)
{
String tex="";  // cria uma string
umbyte = Serial.read();
while(Serial.available() > 0&&(char)umbyte!=div)
{// enquanto houver mais bytes de entrada
tex +=(char) umbyte; // converte o byte em um char e então o concatena à string
umbyte = Serial.read();
}
return tex; // retorna a linha inteira que foi digitada  
}

float proxfloat()
{
umbyte =Serial.read();// Lê o prox byte na porta serial
while(umbyte == ' ')
umbyte =Serial.read();
String num="";
//if(umbyte == '(') 
//return(fazerconta()); // examina se encontrou um inicio de parenteses, e se sim, retorna o valor da conta dentro desse parenteses
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
if(num == "")
return(0);
return(atof(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
// atof é abreviatura de "ASCII to Float" , e c_str() faz a conversão de String para um array de char ( char[] )
/*
por (atof) ser uma função do c++, ela apenas trabalha com char[] e não com String ( faz parte das livrarias do arduino ) , por isso a necessidade de
converter o string em um array de chars antes de converter em um float
*/
}
long proxlong()
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
while(isDigit(umbyte)||umbyte==' ') // enquanto os bytes que forem lidos ainda forem digitos ou um ponto
{
if(umbyte != ' ')  
num += (char)umbyte; // converte o byte ASCII em um char, e então concatena a string 'num'
umbyte = Serial.read(); // Lê o prox byte na porta serial
}
if(num == "")
return(0);
return(atol(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
// atoi é abreviatura de "ASCII to int" , e c_str() faz a conversão de String para um array de char ( char[] )
/*
por (atoi) ser uma função do c++, ela apenas trabalha com char[] e não com String ( faz parte das livrarias do arduino ) , por isso a necessidade de
converter o string em um array de chars antes de converter em um integer
*/
}
