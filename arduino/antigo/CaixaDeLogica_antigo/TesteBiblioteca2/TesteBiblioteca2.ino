#include <CaixaDeLogica.h>
#include <NewTone.h>
#include <digitalWriteFast.h>

#define NOP __asm__ __volatile__ ("nop\n\t")
#define NOP10 NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP
#define NOP9 NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP
#define NOP8 NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP
#define NOP7 NOP;NOP;NOP;NOP;NOP;NOP;NOP
#define NOP6 NOP;NOP;NOP;NOP;NOP;NOP
#define NOP5 NOP;NOP;NOP;NOP;NOP
#define NOP4 NOP;NOP;NOP;NOP
#define NOP3 NOP;NOP;NOP
#define NOP2 NOP;NOP
#define NOP1 NOP

#define NOP11 NOP10;NOP1
#define NOP12 NOP10;NOP2
#define NOP13 NOP10;NOP3
#define NOP14 NOP10;NOP4
#define NOP15 NOP10;NOP5
#define NOP16 NOP10;NOP6
#define NOP17 NOP10;NOP7
#define NOP18 NOP10;NOP8
#define NOP19 NOP10;NOP9
#define NOP20 NOP10;NOP10

#define NOP21 NOP20;NOP1
#define NOP22 NOP20;NOP2
#define NOP23 NOP20;NOP3
#define NOP24 NOP20;NOP4
#define NOP25 NOP20;NOP5
#define NOP26 NOP20;NOP6
#define NOP27 NOP20;NOP7
#define NOP28 NOP20;NOP8
#define NOP29 NOP20;NOP9
#define NOP30 NOP20;NOP10

#define NOP31 NOP30;NOP1
#define NOP32 NOP30;NOP2
#define NOP33 NOP30;NOP3
#define NOP34 NOP30;NOP4
#define NOP35 NOP30;NOP5
#define NOP36 NOP30;NOP6
#define NOP37 NOP30;NOP7
#define NOP38 NOP30;NOP8
#define NOP39 NOP30;NOP9
#define NOP40 NOP30;NOP10

#define NOP41 NOP40;NOP1
#define NOP42 NOP40;NOP2
#define NOP43 NOP40;NOP3
#define NOP44 NOP40;NOP4
#define NOP45 NOP40;NOP5
#define NOP46 NOP40;NOP6
#define NOP47 NOP40;NOP7
#define NOP48 NOP40;NOP8
#define NOP49 NOP40;NOP9
#define NOP50 NOP40;NOP10

#define TONEPIN A4
// liga o pino 1
//#define TONEON PORTD = PORTD | B00000010
//#define TONEOFF PORTD = PORTD & B11111101
//PORTC 
#define TONEON PORTC = PORTC | B00010000
#define TONEOFF PORTC = PORTC & B11101111

CaixaDeLogica caixa;
#define TONEDELAY 25
#define FTONEDELAY 25.0

byte* sin_wave = 0;
int sin_wave_size = 0;
float microsdelay;
float microsperiod;
float freq;
void setup() {
  float r;
  float wave_amplitude;
  float normalized_amplitude;
  float percent_amplitude;
  
    /*
  
  F = 1 /T
  F*T = 1
  T = 1/F
  
  MICROS = (T * 1_000_000)/2
  MICROS = T * 500_000
  MICROS = 1/F * 500_000;
  MICROS = 500_000/F
  
  tempo_i = MICROS*2
  loops * tempo_i = tempo_total
  loops = tempo_total/tempo_i 
  
  */
  freq = (float)NOTE_C3;
  microsdelay  = 500000.0/freq;
  microsperiod = 1000000.0/freq;
  Serial.begin(9600);
  int samples = (int)(microsperiod/FTONEDELAY);
  
  // Allocation (let's suppose size contains some value discovered at runtime,
  // e.g. obtained from some external source or through other program logic)
  sin_wave = new byte [samples];
  sin_wave_size = samples;
  for(int i=0;i<sin_wave_size;i++)
  {
    //calculate pwm percentage based on sine wave
    r = (float)i/(float)sin_wave_size;
    r = r * 6.28;
    wave_amplitude = sin(r);
    normalized_amplitude = wave_amplitude + 1.0;
    percent_amplitude = (normalized_amplitude / 2.0);
    percent_amplitude = min(0.8,max(0.2,percent_amplitude));
    //calculate pwm on and off times based on calculated percentage
    sin_wave[i] = max(5,(int)(FTONEDELAY*percent_amplitude));
    Serial.println(sin_wave[i]);
  }
  

  Serial.print("Frequencia:");  Serial.print(freq); Serial.println(" Hertz");
  Serial.print("Period:"); Serial.print(microsperiod); Serial.println(" uS");
  Serial.print("Samples:"); Serial.println(sin_wave_size);
}

boolean apertado = false;
void loop() {
  if(caixa.lerBotao(0))
  {
    if(!apertado)
    {
      apertado = true;
      noInterrupts();
        tocar(1000);
      interrupts();
      delay(1000);
      tocarQuadrado(1000);
      delay(200);
      caixa.tocarNota(freq,1000);
      delay(1200);
      NewTone(TONEPIN,freq,1000);
      delay(1200);
      caixa.setarLed(0,0,HIGH);
    }
  }
  else
  {
    if(apertado)
    {
      caixa.setarLed(0,0,LOW);
      apertado = false;
    }
  }
}

void tocar(int tempo)
{

  float microtempo = ((float)tempo)*1000.0;
  int loops = (int)(microtempo/microsperiod);

  for(int i=0;i< loops;i++)
  {
    playwave(); 
//percent_Ontone(pin,microsdelay);
    //digitalWriteFast(pin,HIGH);
    //TONEON;
    //delayMicroseconds(microsdelay_on);
//percent_Offtone(pin,microsdelay);
    //digitalWriteFast(pin,LOW);
    //TONEOFF;
    //delayMicroseconds(microsdelay_off);
  }

}

void tocarQuadrado(int tempo)
{

  float microtempo = ((float)tempo)*1000.0;
  int loops = (int)(microtempo/microsperiod);
  for(int i=0;i< loops;i++)
  {
    TONEON;
    delayMicroseconds(microsdelay);
    TONEOFF;
    delayMicroseconds(microsdelay);
  }

}

void playwave()
{
  //int loops = microsdelay/100;
  // 160 para 10 micros
  // 75 75
  // 4 para cada toneon off
  //
  for(int i=0;i<sin_wave_size;i++)
  {
    TONEON;
    //digitalWriteFast(1,HIGH);
    delayMicroseconds(sin_wave[i]);

    //digitalWriteFast(1,LOW);
    TONEOFF;
    delayMicroseconds(TONEDELAY-sin_wave[i]);
  }
}


