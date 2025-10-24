#include <AccelStepper.h>
#include <Servo.h>
#include <EEPROM.h>
// parte do motor
int velocidade_motor = 800; 
int aceleracao_motor = 1000;

// Definicao pino ENABLE
const int pino_enable = 10;

// Definicao pinos STEP e DIR
//AccelStepper motor1(1,7,4 );
AccelStepper motor1(1,8,7 );

// Definicao pino Servo motor
const int servoPin = 12;
Servo servo;

// parte da comunicação Pi e Arduino
const int greenled = 14;
const int redled = 15;

// comunicação teste
const char COD_PING = '@';

// controle motor de passo ( que sobe e desce a prateleira )
const char COD_SETAR_POSICAO = 'a';
const char COD_LER_POSICAO = 'b';
const char COD_LER_ALVO = 'c';
const char COD_PARAR_MOTOR = 'd';

// controle motor servo ( que gira a amostra)
const char COD_SETAR_GIROVEL = 'e';
const char COD_LER_GIROVEL = 'f';

// calibração
const char COD_CALIBRA_MIN = '0';
const char COD_CALIBRA_MAX = '1';
const char COD_SETAR_PASSOS = '2';
const char COD_ADD_PASSOS = '3';
const char COD_SALVAR_CALIBRA = '4';
const char COD_CALIBRA_MOTOR = '5';


// para não ter erro
const char COD_ESPACO = ' ';
const char COD_FIM = '\n';
const String COD_VERSAO = "1.0.0";
const String COD_OK = "OK";

// indices da memoria EEPROM
const int EEPROM_iscalibrated = 0;
const int EEPROM_max_passos = 1;
const int EEPROM_max_posicao = 2;
const int EEPROM_velocidade_motor = 3;
const int EEPROM_aceleracao_motor = 4;
int is_calibrated = 0;
// variaveis de tempo de execução
char buffer[80];
long posicao_alvo = 0L;
long passos_atual = 0L;
long passos_alvo = 0L;
long max_passos = 0L;
long max_posicao = 0L;
//long min_passos = 0L;


int servo_girovel = 0;
bool servo_detached = true;
// ajudadores
bool parado = true;
long timeout_parar = 0;

void setup()
{
  // carrega as configuracoes do EEPROM
  load_configurations();
  //max_passos = 2510L;
  //max_posicao = 1000L;  //décimos de milímetro
  
  
  pinMode(greenled, OUTPUT);
  pinMode(redled, OUTPUT);
  Serial.begin(9600);
  
  pinMode(pino_enable, OUTPUT);
  // Configuracoes iniciais motor de passo
  motor1.setMaxSpeed((float)velocidade_motor);
  motor1.setSpeed((float)velocidade_motor);
  motor1.setAcceleration((float)aceleracao_motor);
  
  
  // necessário para que o raspberry aceite a comunicação serial
  // versão?
  Serial.print(COD_OK);
  Serial.print(COD_ESPACO);
  Serial.print(COD_VERSAO);
  Serial.print(COD_ESPACO);
  Serial.print(is_calibrated);
  Serial.print(COD_FIM);
  
   Serial.print(max_posicao);
   Serial.print(COD_ESPACO);
   Serial.print(max_passos);
   Serial.print(COD_ESPACO);
   Serial.print(velocidade_motor);
   Serial.print(COD_ESPACO);
   Serial.print(aceleracao_motor);
   Serial.print(COD_FIM);
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
    
  // desliga o motor caso não esteja sendo usado, e liga se for usar
  // segundo a kalatec neste vídeo https://www.youtube.com/watch?v=RzdTuPVbET4 aos 20:25 do vídeo
  // diz que o enable serve para dimuir a corrente no motor quando não estiver usando ele
  // desligar o motor quando não é usado aumenta sua vida útil
  // e talvez esquente menos, já que o que esse pino faz é diminuir a corrente para abaixo da nominal
  // mas tem que ver se isso não vai fazer a prateleira cair, já que o torque vai diminuir também ( o eixo fica leve )
  passos_atual = motor1.currentPosition();
  if(passos_atual != passos_alvo) // precisa andar
  {
	if(parado)
	{
		parado = false;
		digitalWrite(pino_enable, LOW); // ativa o motor
		digitalWrite(greenled, HIGH); // liga o led para dizer que ligou o motor mesmo
	}
	timeout_parar = 0;
  }
  else // precisa parar
  {
	if(!parado)
	{
		timeout_parar++;
		if(timeout_parar > 1000)
		{
			parado = true;
			digitalWrite(pino_enable, HIGH); // desativa o motor
			digitalWrite(greenled, LOW); // desliga o led para dizer que desligou o motor mesmo
			timeout_parar = 0;
		}
	}
  }
  
  // seta o target caso não tenha setado ainda
  if(motor1.targetPosition() != passos_alvo)
  {
    // comando para dizer o alvo da posicao
    motor1.moveTo(passos_alvo);
  }
  
  // Comando para acionar o motor para ir até a posição desejada
  // essa função deve sem exceções ser chamada em um intervalo de tempo
  // o mais despresível quanto possível, para que o movimento do motor seja suave
  // isso está na ordem de microsegundos.
  motor1.run();
}

long posicao_para_passos(long posicao)
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

long passos_para_posicao(long passos)
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

void setar_posicao_alvo(long posicao)
{
	posicao_alvo = posicao;
	passos_alvo = posicao_para_passos(posicao_alvo);
}

void setar_passos_alvo(long passos)
{
	posicao_alvo = passos_para_posicao(passos);
	passos_alvo = passos;
}

long ler_posicao()
{
	return passos_para_posicao(passos_atual);
}

int i;
void ler_instrucao()
{
  i = 0;
  char cod = buffer[i++];
  switch(cod)
  {
	case COD_PING:
	  Serial.print(COD_OK);
	  Serial.print(COD_FIM);
	break;
    case COD_SETAR_POSICAO:
      // pula o caractere de espaço
      i++;
      //
      setar_posicao_alvo(parseNumber());
      Serial.print(COD_OK);
	  Serial.print(COD_FIM);
    break;
	case COD_SETAR_PASSOS:
      // pula o caractere de espaço
      i++;
      //
      setar_passos_alvo(parseNumber());
      Serial.print(COD_OK);
	  Serial.print(COD_FIM);
    break;
	case COD_ADD_PASSOS:
      // pula o caractere de espaço
      i++;
      //
      setar_passos_alvo(parseNumber() + passos_alvo);
      Serial.print(COD_OK);
	  Serial.print(COD_FIM);
    break;
    case COD_LER_POSICAO:
      Serial.print(COD_OK);
      Serial.print(COD_ESPACO);
      Serial.print(ler_posicao());
	  Serial.print(COD_ESPACO);
	  Serial.print(passos_atual);
      Serial.print(COD_FIM);
    break;
    case COD_LER_ALVO:
      Serial.print(COD_OK);
      Serial.print(COD_ESPACO);
      Serial.print(posicao_alvo);
	  Serial.print(COD_ESPACO);
	  Serial.print(passos_alvo);
      Serial.print(COD_FIM);
    break;
    case COD_CALIBRA_MIN:
	  posicao_alvo = 0L;
	  passos_alvo = 0L;
	  passos_atual = 0L;
	  motor1.setCurrentPosition(0L);
      Serial.print(COD_OK);
	  Serial.print(COD_FIM);
    break;
	case COD_PARAR_MOTOR:
	  setar_passos_alvo(passos_atual);
	  // usar um parar mais instantaneo
	  motor1.stop();
      Serial.print(COD_OK);
	  Serial.print(COD_FIM);
    break;
	case COD_CALIBRA_MAX:
	  // pula o caractere de espaço
      i++;
      //
      max_posicao = parseNumber();
      // nao pula o caractere de espaço, o parsenumber ja pulou
	  max_passos =  parseNumber();
	  Serial.print(COD_OK);
	  Serial.print(COD_FIM);
	break;
	case COD_CALIBRA_MOTOR:
	  // pula o caractere de espaço
	  i++;
	  //
	  velocidade_motor = (int)parseNumber();
	  // nao pula o caractere de espaço, o parsenumber ja pulou

	  aceleracao_motor = (int)parseNumber();
	  motor1.setMaxSpeed((float)velocidade_motor);
	  motor1.setSpeed((float)velocidade_motor);
	  motor1.setAcceleration((float)aceleracao_motor);
	  Serial.print(COD_OK);
	  Serial.print(COD_FIM);
	break;
	case COD_SALVAR_CALIBRA:
	  //salva a calibracao na EEPROM
	  save_configurations();
	  Serial.print(COD_OK);
	  Serial.print(COD_FIM);
	break;
	case COD_LER_GIROVEL:
      Serial.print(COD_OK);
      Serial.print(COD_ESPACO);
      Serial.print(servo_girovel);
      Serial.print(COD_FIM);
    break;
	case COD_SETAR_GIROVEL:
		// pula o caractere de espaço
		i++;
		//
		servo_girovel = parseNumber();
		if(servo_girovel != 0)
		{
			if(servo_detached)
			{
				servo.attach(servoPin);
				servo_detached = false;
			}
			//servo.writeMicroseconds(servo_girovel);
			switch(servo_girovel)
			{
				case -3: servo.writeMicroseconds(700); break;
				case -2: servo.writeMicroseconds(1000); break;
				case -1: servo.writeMicroseconds(1150); break;
				
                                // 1280

				case 1: servo.writeMicroseconds(1450); break;
				case 2: servo.writeMicroseconds(1700); break;
				case 3: servo.writeMicroseconds(2200); break;
			}
		}
		else if(!servo_detached)
		{
			servo.detach();
			servo_detached = true;
		}
		
		Serial.print(COD_OK);
		Serial.print(COD_FIM);
	break;
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
}
