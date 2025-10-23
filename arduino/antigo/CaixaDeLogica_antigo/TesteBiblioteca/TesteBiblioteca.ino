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

#define TONEPIN 1
// liga o pino 1
#define TONEON PORTD = PORTD | B00000010
#define TONEOFF PORTD = PORTD & B11111101

CaixaDeLogica caixa;

void setup() {

}

boolean apertado = false;
void loop() {
  if(caixa.lerBotao(0))
  {
    if(!apertado)
    {
      apertado = true;
      int tempo = 1000;
      int pausa = tempo+100;
      tocarNota(NOTA_DO,tempo);delay(pausa);
      tocarNota(NOTA_RE,tempo);delay(pausa);
      tocarNota(NOTA_MI,tempo);delay(pausa);
      tocarNota(NOTA_FA,tempo);delay(pausa);
      tocarNota(NOTA_SOL,tempo);delay(pausa);
      tocarNota(NOTA_LA,tempo);delay(pausa);
      tocarNota(NOTA_SI,tempo);delay(pausa);
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

void tocarNota(long nota,long duracao)
{
  //NewTone(A4,nota,duracao);
  //tone(pin_buzzer,nota,duracao);
  //caixa.tocarNota(nota,duracao);
}


