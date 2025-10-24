/*
  TargetMotor.cpp 
  Created by Erick L. Weil, Jun 23, 2017.
  Released into the public domain.
*/

#ifndef TargetMotor_h
#define TargetMotor_h

//https://gist.github.com/jrmedd/5516863
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif
  
#include <AccelStepper.h>
#include <EEPROM.h>

class TargetMotor
{
  public:
    TargetMotor(int _control_type,int _pin_step,int _pin_dir,int _pin_enable,int _pin_greenled,float _velocity,float _acceleration);
	AccelStepper motor;
	void run();
	long posicao_para_passos(long posicao);
	long passos_para_posicao(long passos);
	void setar_posicao_alvo(long posicao);
	void setar_passos_alvo(long passos);
	long ler_posicao();
	void parar();
	void calibrar_fim_curso();
	// prevent motor damage with default 10cm -> 2510 steps mapping
	long max_passos = 2510L;
	long max_posicao = 1000L;
	
	// start at step 0
	long passos_atual = 0L;
	long passos_alvo = 0L;
	long posicao_alvo = 0L;
  private:
    // very carefull when setting this, this can lead to damage on motor
	float velocity = 800.0f;
	float acceleration = 1000.0f;
	int control_type;
    int pin_step;
	int pin_dir;
	int pin_enable;
	int pin_greenled;

	
	// enable pin manual control, should i use the embedded solution of AccelStepper class?
	bool parado = true;
	long timeout_parar = 0L;
};

#endif

