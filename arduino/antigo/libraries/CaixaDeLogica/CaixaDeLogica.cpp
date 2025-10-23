/*
  CaixaDeLogica.pp - Biblioteca para caixa de lógica, sério?
*/

#include "Arduino.h"
#include "CaixaDeLogica.h"

//const int pin_grade[] =
//{13,12,11,
//10, 9, 8,
//7,  6, 5,
//4,  3, 2}; // pinos dos leds dispostos em sequencia

const int pin_grade[] =
{ 13,10, 7, 4,
  12, 9, 6, 3, 
  11, 8, 5, 2}; // pinos dos leds dispostos em sequencia

const int pin_buzzer = A4;
const int pin_botoes[] = {14,15,16,17};
	
CaixaDeLogica::CaixaDeLogica()
{
	
	// if analog input pin 0 is unconnected, random analog
	// noise will cause the call to randomSeed() to generate
	// different seed numbers each time the sketch runs.
	// randomSeed() will then shuffle the random function.
	randomSeed(analogRead(0));
	/*for(int x=0;x<Dx;x++)
	{
		for(int y=0;y<Dy;y++)
		{
			pinMode(P(x,y), OUTPUT);
			Serial.print("O led ");Serial.print(P(x,y));Serial.print(" foi setado na pos x:");Serial.print(x);Serial.print(" y:");Serial.println(y);
		}
	}
	*/
	for(int i=0;i<NUM_LEDS;i++)
	pinMode(pin_grade[i], OUTPUT);

	for(int i=0;i<NUM_BOTOES;i++)
	{ 
		pinMode(pin_botoes[i], INPUT);
		//Serial.print("o botao ");Serial.print(botoes[i]);Serial.print(" foi setado no index i:");Serial.print(i);
	}
	pinMode(pin_buzzer, OUTPUT);
}


void CaixaDeLogica::_led(byte index,byte estado)
{
	digitalWrite(pin_grade[index],estado); 
}

byte CaixaDeLogica::_getIndex(byte x,byte y)
{
	return x + (DX*y);
}

void CaixaDeLogica::setarLed(byte x,byte y,byte estado)
{
	if(x >= 0 && x < DX && y >= 0 && y < DY)
	_led(_getIndex(x,y), estado);  
}

boolean CaixaDeLogica::lerBotao(byte botao)
{
	if(botao >= 0 && botao < NUM_BOTOES)
	return digitalRead(pin_botoes[botao]);
}

int CaixaDeLogica::qualBotaoApertado()
{
	for(int i=0;i<NUM_BOTOES;i++)
	{ 
		if(lerBotao(i)) return i;
	}
	return -1;
}

void CaixaDeLogica::tocarNota(long nota,long duracao,boolean esperar)
{
	tone(pin_buzzer,nota,duracao);
	if(esperar) delay(duracao);
}

void CaixaDeLogica::pararNota()
{
	noTone(pin_buzzer);
}