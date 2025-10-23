#include <CaixaDeLogica.h>

CaixaDeLogica caixa;

void setup() {

}
/*
#define NOTA_DO NOTE_C4
#define NOTA_RE NOTE_D4
#define NOTA_MI NOTE_E4
#define NOTA_FA NOTE_F4
#define NOTA_SOL NOTE_G4
#define NOTA_LA NOTE_A4S4
#define NOTA_SI NOTE_B4
*/

long _t = 1500;
long _t2 = _t/2;
long _t4 = _t/4;
long _t8 = _t/8;
long _t16 = _t/16;
long _t32 = _t/32;
long _base = NOTA_RE;
long _diff = NOTA_RE-NOTA_DO;
long bob_notas[] = {_base, _base+ _diff, _base, _base+ _diff*6, _base+ _diff*4, _base+ _diff*2, _base+ _diff*2};
long bob_tempo[] = {_t8    ,    _t8,      _t8,     _t2,     _t4,      _t8,     _t8};

long vivaldi_notas[] = {
NOTA_FA ,NOTA_LA,NOTA_LA,NOTA_LA,NOTA_SOL,NOTA_FA,      
NOTA_DOm, NOTA_DOm ,NOTA_SI,
NOTA_LA,NOTA_LA,NOTA_LA,NOTA_SOL,NOTA_FA,
NOTA_DOm,NOTA_DOm,NOTA_SI,
NOTA_LA,NOTA_SI,NOTA_DOm,NOTA_SI,NOTA_LA,NOTA_SOL
};
long vivaldi_tempo[] = { 
    _t8 ,    _t8,    _t8,    _t8,    _t16,   _t16,    
	_t4 + _t8, _t16, _t16,
    _t8,    _t8,    _t8,    _t16,   _t16,    
	_t4+_t8,    _t16,    _t16,
	_t8,_t16,_t16,_t8,_t8,_t4
	
};

int ultimoBotao = 0;
boolean estava_apertando = false;
void loop() {
  int botaoApertado = caixa.qualBotaoApertado();
  boolean esta_apertado = (botaoApertado != -1);
  if(esta_apertado && !estava_apertando)
  {
    caixa.setarLed(botaoApertado,0,true);
    if(botaoApertado != ultimoBotao)
      caixa.setarLed(ultimoBotao,0,false);
    ultimoBotao = botaoApertado;
	
	switch(botaoApertado)
	{
		case 0:
		bob_marley();
		break;
		case 1:
		vivaldi();
	}
  }
  
  estava_apertando = (botaoApertado != -1);
}

void bob_marley()
{
  int notas_size = sizeof(bob_notas)/sizeof(long);
  for(int i=0;i<notas_size;i++)
  {
    if(bob_notas[i] > 0)
      caixa.tocarNota(bob_notas[i],bob_tempo[i]);
    else
    {
      delay(bob_tempo[i]);
    }
    delay(50);
  }
}

void vivaldi()
{
  int notas_size = sizeof(vivaldi_notas)/sizeof(long);
  for(int i=0;i<notas_size;i++)
  {
    if(vivaldi_notas[i] > 0)
      caixa.tocarNota(vivaldi_notas[i],vivaldi_tempo[i]);
    else
    {
      delay(vivaldi_tempo[i]+_t32);
    }
    delay(_t16);
  }
}



