/*
  Comunicar.cpp - library para comunicação serial mais simples.
  Criado por Erick Leonardo Weil, 2014, Setembro, 30.
  lançado no dominio publico.
*/

#include "Arduino.h"
#include "Comunicar.h"

//Comunicar::Comunicar(byte a)
//{
//_umbyte = a;
//}

String Comunicar::proxlinha(char div)
{
_umbyte = ' ';
while(Serial.available()==0)
{
delay(100);
}
String tex="";  // cria uma string
_umbyte = Serial.read();
do
{// enquanto houver mais bytes de entrada
tex +=(char) _umbyte; // converte o byte em um char e então o concatena à string
_umbyte = Serial.read();
}
while(Serial.available() > 0&&(char)_umbyte!=div);
if(_umbyte != div)
tex +=(char) _umbyte;
return tex; // retorna a linha inteira que foi digitada  
}

float Comunicar::proxfloat()
{
_umbyte = ' ';
while(Serial.available()==0)
{
delay(100);
}
_umbyte =Serial.read();// Lê o prox byte na porta serial
while(_umbyte == ' ')
_umbyte =Serial.read();
String num="";
//if(umbyte == '(') 
//return(fazerconta()); // examina se encontrou um inicio de parenteses, e se sim, retorna o valor da conta dentro desse parenteses
if(_umbyte == '-'||_umbyte == '+')
{
num += (char)_umbyte;
_umbyte= Serial.read();
}
while(isDigit(_umbyte)||_umbyte=='.'||_umbyte==' ') // enquanto os bytes que forem lidos ainda forem digitos ou um ponto
{
if(_umbyte != ' ')  
num += (char)_umbyte; // converte o byte ASCII em um char, e então concatena a string 'num'
_umbyte = Serial.read(); // Lê o prox byte na porta serial
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
long Comunicar::proxlong()
{
_umbyte = ' ';
while(Serial.available()==0)
{
delay(100);
}
_umbyte =Serial.read();// Lê o prox byte na porta serial
while(_umbyte == ' ')
_umbyte =Serial.read();
String num="";
if(_umbyte == '-'||_umbyte == '+')
{
num += (char)_umbyte;
_umbyte= Serial.read();
}
while(isDigit(_umbyte)||_umbyte==' ') // enquanto os bytes que forem lidos ainda forem digitos ou um ponto
{
if(_umbyte != ' ')  
num += (char)_umbyte; // converte o byte ASCII em um char, e então concatena a string 'num'
_umbyte = Serial.read(); // Lê o prox byte na porta serial
}
if(num == "")
return(0);
return(atol(num.c_str())); // faz a conversão da string num que contêm o numero completo, com todos os digitos concatenados, em um float
}

String Comunicar::tipo()
{
char thisChar = Serial.peek();
    if(isAlpha(thisChar)) {
      return "letra";
    }
    if(isWhitespace(thisChar)) {
     return "espaço";
    }
    if(isDigit(thisChar)||thisChar == '-'||thisChar == '+') {
      return "numero";
    }
    if(isPunct(thisChar)) {
      return "pontuação";
    }
    if(isSpace(thisChar)) {
      return "espaço";
    }
return "estranho";	
}

Comunicar Ler;