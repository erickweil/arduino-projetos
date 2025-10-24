#include <AccelStepper.h>
#include <EEPROM.h>
#include <TargetMotor.h>

// protocolo da comunicacao Arduino

// comunicação teste
const char COD_PING = '@';

// controle motor de passo ( que sobe e desce a prateleira )
const char COD_SETAR_POSICAO = 'a';
const char COD_LER_POSICAO = 'b';
const char COD_LER_ALVO = 'c';
const char COD_PARAR_MOTOR = 'd';

// calibração
const char COD_CALIBRA_MIN = '0';
const char COD_CALIBRA_POS = '1';
const char COD_SETAR_PASSOS = '2';
const char COD_ADD_PASSOS = '3';


// para não ter erro
const char COD_ESPACO = ' ';
const char COD_FIM = '\n';
const String COD_VERSAO = "2.0.0";
const String COD_OK = "OK";

const int redled = 15;

// variaveis de tempo de execução
char buffer[80];
// Definicao pinos STEP e DIR
//                    _control_type  _pin_step  _pin_dir
//AccelStepper motor1(1              ,8         ,7      );

const byte n_motores = 4;
const int MOTOR_TROCADOR = 0;
const int MOTOR_MESA = 1;
const int MOTOR_ATUADOR = 2;
const int MOTOR_GIRADOR = 3;

// atenção ao setar velocity e acceleration
TargetMotor motores[] = {
//       _control_type  _pin_step  _pin_dir  _pin_enable  _pin_greenled  _velocity  _acceleration
TargetMotor( 1            , 8        , 7       , 10         , 14           , 800.0f   , 1000.0f      ),
TargetMotor( 1            , 8        , 7       , 10         , 14           , 800.0f   , 1000.0f      ),
TargetMotor( 1            , 8        , 7       , 10         , 14           , 800.0f   , 1000.0f      ),
TargetMotor( 1            , 8        , 7       , 10         , 14           , 800.0f   , 1000.0f      )
};

void setup()
{
	Serial.begin(9600);


	motores[MOTOR_TROCADOR].max_passos = 2510L;
	motores[MOTOR_TROCADOR].max_posicao = 1000L;

	motores[MOTOR_MESA].max_passos = 2510L;
	motores[MOTOR_MESA].max_posicao = 1000L;

	motores[MOTOR_ATUADOR].max_passos = 2510L;
	motores[MOTOR_ATUADOR].max_posicao = 1000L;

	motores[MOTOR_GIRADOR].max_passos = 2510L;
	motores[MOTOR_GIRADOR].max_posicao = 1000L;


	// necessário para que o raspberry aceite a comunicação serial
	// versão?
	Serial.print(COD_OK);
	Serial.print(COD_ESPACO);
	Serial.print(COD_VERSAO);
	Serial.print(COD_ESPACO);
	//Serial.print(is_calibrated);
	Serial.print(COD_FIM);

	/*Serial.print(max_posicao);
	Serial.print(COD_ESPACO);
	Serial.print(max_passos);
	Serial.print(COD_ESPACO);
	Serial.print(velocidade_motor);
	Serial.print(COD_ESPACO);
	Serial.print(aceleracao_motor);
	Serial.print(COD_FIM);*/
}

void loop()
{
	// aqui é a parte que lê o serial sem bloquear a execução
	// isso é importante porque se a função run() do accelstepper não for chamada
	// em um espaço curto de tempo, pode acontecer do motor não chegar a velocidade desejada
	// ou se não chamar a função por um intervalo grande de tempo o que acontece é que o motor para

	// então aqui checa se chegou 1 byte ou mais na entrada serial
	// e lê 1 byte e adiciona a variável buffer
	// se o byte for um \n ( fim de linha ) passa pela função ler_instrucao()
	if(Serial.available() > 0) // se tem algo a ser lido
	{
		digitalWrite(redled, HIGH); // liga o led para indicar a comunicação chegando
		if(readline(Serial.read(), buffer, 80) > 0)  // le so mais um caractere, e retorna > 0 se tiver lido \n
		{ 
			// interpreta a instrucao contida no buffer
			ler_instrucao();
			digitalWrite(redled, LOW);
		}
	}

	// Comando para acionar o motor para ir até a posição desejada
	// essa função deve sem exceções ser chamada em um intervalo de tempo
	// o mais despresível quanto possível, para que o movimento do motor seja suave
	// isso está na ordem de microsegundos.

	motores[MOTOR_MESA].run();
	motores[MOTOR_ATUADOR].run();
	motores[MOTOR_TROCADOR].run();
	motores[MOTOR_GIRADOR].run();
}

int i;
void ler_instrucao()
{
	i = 0;
	char cod = buffer[i++];
	int q_motor = -1;
	switch(cod)
	{
	case COD_PING:
		Serial.print(COD_OK);
		Serial.print(COD_FIM);
	break;
	case COD_SETAR_POSICAO:
		// pula o caractere de espaço
		i++;
		q_motor = parseNumber();

		motores[q_motor].setar_posicao_alvo(parseNumber());
		Serial.print(COD_OK);
		Serial.print(COD_FIM);
	break;
	case COD_SETAR_PASSOS:
		// pula o caractere de espaço
		i++;
		q_motor = parseNumber();

		motores[q_motor].setar_passos_alvo(parseNumber());
		Serial.print(COD_OK);
		Serial.print(COD_FIM);
	break;
	case COD_ADD_PASSOS:
		// pula o caractere de espaço
		i++;
		q_motor = parseNumber();

		motores[q_motor].setar_passos_alvo(parseNumber() + motores[q_motor].passos_alvo);
		Serial.print(COD_OK);
		Serial.print(COD_FIM);
	break;
	case COD_LER_POSICAO:
		// pula o caractere de espaço
		i++;
		q_motor = parseNumber();

		Serial.print(COD_OK);
		Serial.print(COD_ESPACO);
		Serial.print(motores[q_motor].ler_posicao());
		Serial.print(COD_ESPACO);
		Serial.print(motores[q_motor].passos_atual);
		Serial.print(COD_FIM);
	break;
	case COD_LER_ALVO:
		// pula o caractere de espaço
		i++;
		q_motor = parseNumber();

		Serial.print(COD_OK);
		Serial.print(COD_ESPACO);
		Serial.print(motores[q_motor].posicao_alvo);
		Serial.print(COD_ESPACO);
		Serial.print(motores[q_motor].passos_alvo);
		Serial.print(COD_FIM);
	break;
	case COD_CALIBRA_MIN:
		// pula o caractere de espaço
		i++;
		q_motor = parseNumber();

		motores[q_motor].calibrar_fim_curso();
		Serial.print(COD_OK);
		Serial.print(COD_FIM);
	break;
	case COD_PARAR_MOTOR:
		// pula o caractere de espaço
		i++;
		q_motor = parseNumber();

		motores[q_motor].parar();
		Serial.print(COD_OK);
		Serial.print(COD_FIM);
	break;
	case COD_CALIBRA_POS:
		// pula o caractere de espaço
		i++;
		q_motor = parseNumber();

		// nao pula o caractere de espaço, o parsenumber ja pulou
		motores[q_motor].max_posicao = parseNumber();
		// nao pula o caractere de espaço, o parsenumber ja pulou
		motores[q_motor].max_passos =  parseNumber();
		Serial.print(COD_OK);
		Serial.print(COD_FIM);
	break;
	// não calibrar velocidade, por causa que nao solucinei a leitura de valores float
	// e caso algo aconteça errado ao setar a velocidade do motor, isso pode danificar os motores
	// o controlador, e até mesmo o arduino
	/*case COD_CALIBRA_VEL:
	  // pula o caractere de espaço
	  i++;
	  q_motor = parseNumber();
	  // nao pula o caractere de espaço, o parsenumber ja pulou
	  velocidade_motor = (int)parseNumber();
	  // nao pula o caractere de espaço, o parsenumber ja pulou
	  aceleracao_motor = (int)parseNumber();
	  motor1.setMaxSpeed((float)velocidade_motor);
	  motor1.setSpeed((float)velocidade_motor);
	  motor1.setAcceleration((float)aceleracao_motor);
	  Serial.print(COD_OK);
	  Serial.print(COD_FIM);
	break;*/
	}
}

long parseNumber()
{
	char r;
	long num = 0L;
	bool negativo = false;
	while((r = buffer[i++]) != 0)
	{
		switch(r)
		{
			case '-':
				negativo = true;
			break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				num *= 10L;
				num += r - '0';
			break;
			default:
				if(negativo)
				return -num;
				else
				return num;
			break;
		}
	}
	if(negativo)
	return -num;
	else
	return num;
}

int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\r': // Ignore CR
        break;
      case '\n': // Return on new-line
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len-1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}
