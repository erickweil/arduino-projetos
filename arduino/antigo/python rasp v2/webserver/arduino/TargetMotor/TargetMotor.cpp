/*
  TargetMotor.cpp 
  Created by Erick L. Weil, Jun 23, 2017.
  Released into the public domain.
*/

// support for older arduino ide
//https://gist.github.com/jrmedd/5516863
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif
#include "TargetMotor.h"

TargetMotor::TargetMotor(int _control_type,int _pin_step,int _pin_dir,int _pin_enable,int _pin_greenled,float _velocity,float _acceleration) :
	motor(_control_type,_pin_step,_pin_dir),
	control_type(_control_type),
	pin_step(_pin_step),
	pin_dir(_pin_dir),
	pin_enable(_pin_enable),
	pin_greenled(_pin_greenled),
	velocity(_velocity),
	acceleration(_acceleration)
{
	pinMode(_pin_greenled, OUTPUT);
	pinMode(pin_enable, OUTPUT);
	// Configuracoes iniciais motor de passo
	motor.setMaxSpeed(velocity);
	motor.setSpeed(velocity);
	motor.setAcceleration(acceleration);
}

void TargetMotor::run()
{
	// atualiza os valores passo atual e passo alvo
	passos_atual = motor.currentPosition();
	
	// seta o target caso não tenha setado ainda
	if(motor.targetPosition() != passos_alvo)
	{
		// comando para dizer o alvo da posicao
		motor.moveTo(passos_alvo);
	}
	
	// desliga o motor caso não esteja sendo usado, e liga se for usar
	// segundo a kalatec neste vídeo https://www.youtube.com/watch?v=RzdTuPVbET4 aos 20:25 do vídeo
	// diz que o enable serve para dimuir a corrente no motor quando não estiver usando ele
	// desligar o motor quando não é usado aumenta sua vida útil
	// e talvez esquente menos, já que o que esse pino faz é diminuir a corrente para abaixo da nominal
	// mas tem que ver se isso não vai fazer a prateleira cair, já que o torque vai diminuir também ( o eixo fica leve )
	if(passos_atual != passos_alvo) // precisa andar
	{
		if(parado)
		{
			parado = false;
			digitalWrite(pin_enable, LOW); // ativa o motor
			digitalWrite(pin_greenled, HIGH); // liga o led para dizer que ligou o motor mesmo
		}
		timeout_parar = 0;
	}
	else // precisa parar
	{
		if(!parado)
		{
			// mudar para miliseconds?
			timeout_parar++;
			if(timeout_parar > 1000)
			{
				parado = true;
				digitalWrite(pin_enable, HIGH); // desativa o motor
				digitalWrite(pin_greenled, LOW); // desliga o led para dizer que desligou o motor mesmo
				timeout_parar = 0;
			}
		}
	}

	// Comando para acionar o motor para ir até a posição desejada
	// essa função deve sem exceções ser chamada em um intervalo de tempo
	// o mais despresível quanto possível, para que o movimento do motor seja suave
	// isso está na ordem de microsegundos.
	motor.run();
}

void TargetMotor::calibrar_fim_curso()
{
	posicao_alvo = 0L;
	passos_alvo = 0L;
	passos_atual = 0L;
	motor.setCurrentPosition(0L);
}

void TargetMotor::parar()
{
	setar_passos_alvo(passos_atual);
	// usar um parar mais instantaneo
	motor.stop();
}

long TargetMotor::posicao_para_passos(long posicao)
{
	// max_passos -> max_posicao
	// X passos   -> posicao
	// X*max_posicao = max_passos*posicao
	// x = (max_passos*posicao)/max_posicao
        float fmax_passos = (float)max_passos;
        float fposicao = (float)posicao;
        float fmax_posicao = (float)max_posicao;
	return (long)((fmax_passos*fposicao)/fmax_posicao);// + min_passos;
}

long TargetMotor::passos_para_posicao(long passos)
{
	// max_posicao -> max_passos
	// X posicao  ->  passos
	// x*max_passos = max_posicao*passos
	// x = (max_posicao*passos)/max_passos
        float fmax_passos = (float)max_passos;
        float fpassos = (float)(passos);//-min_passos);
        float fmax_posicao = (float)max_posicao;
	return (long)((fmax_posicao*fpassos)/fmax_passos);
}

void TargetMotor::setar_posicao_alvo(long posicao)
{
	posicao_alvo = posicao;
	passos_alvo = posicao_para_passos(posicao_alvo);
}

void TargetMotor::setar_passos_alvo(long passos)
{
	posicao_alvo = passos_para_posicao(passos);
	passos_alvo = passos;
}

long TargetMotor::ler_posicao()
{
	return passos_para_posicao(passos_atual);
}

// parte que salva e carrega informações do EEPROM
// para que a configuração dos motores seja feita pela interface gráfica
// de acordo com a necessidade que aparecer

/*
void load_configurations()
{
	is_calibrated = readEEPROM_long(EEPROM_iscalibrated);
	if(is_calibrated == 1L)
	{
		max_passos = readEEPROM_long(EEPROM_max_passos);
		max_posicao = readEEPROM_long(EEPROM_max_posicao);
		velocidade_motor = (int)readEEPROM_long(EEPROM_velocidade_motor);
		aceleracao_motor = (int)readEEPROM_long(EEPROM_aceleracao_motor);
	}
	else
	{
		max_passos = 2510L;
		max_posicao = 1000L;
	}
}

void save_configurations()
{
	writeEEPROM_long(EEPROM_max_passos,max_passos);
	writeEEPROM_long(EEPROM_max_posicao,max_posicao);
	writeEEPROM_long(EEPROM_velocidade_motor,velocidade_motor);
	writeEEPROM_long(EEPROM_aceleracao_motor,aceleracao_motor);
	writeEEPROM_long(EEPROM_iscalibrated,1L);
}

long readEEPROM_long(int index)
{
	int address = index * 4;
	
	//Read the 4 bytes from the eeprom memory.
    long b0 = EEPROM.read(address + 0);
    long b1 = EEPROM.read(address + 1);
    long b2 = EEPROM.read(address + 2);
    long b3 = EEPROM.read(address + 3);

    //Return the recomposed long by using bitshift.
    return ((b0 << 0L) & 0xFF) + ((b1 << 8L) & 0xFFFF) + ((b2 << 16L) & 0xFFFFFF) + ((b3 << 24L) & 0xFFFFFFFF);
}

void writeEEPROM_long(int index, long value)
{
	int address = index * 4;
	//Decomposition from a long to 4 bytes by using bitshift.
	//b3 = Most significant -> b0 = Least significant byte
	byte b0 = (value & 0xFF);
	byte b1 = ((value >> 8) & 0xFF);
	byte b2 = ((value >> 16) & 0xFF);
	byte b3 = ((value >> 24) & 0xFF);

	//Write the 4 bytes into the eeprom memory.
	EEPROM.write(address + 0, b0);
	EEPROM.write(address + 1, b1);
	EEPROM.write(address + 2, b2);
	EEPROM.write(address + 3, b3);
}*/

