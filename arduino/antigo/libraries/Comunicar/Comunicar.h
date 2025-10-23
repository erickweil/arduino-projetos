/*
  Comunicar.h - library para comunicação serial mais simples.
  Criado por Erick Leonardo Weil, 2014, Setembro, 30.
  lançado no dominio publico.
*/

#ifndef Comunicar_h
#define Comunicar_h

#include "Arduino.h"

class Comunicar
{
  public:
    //Comunicar(byte a);
    String proxlinha(char div);
    float proxfloat();
	long proxlong();
	String tipo();
  private:
    byte _umbyte;
};
extern Comunicar Ler;
#endif