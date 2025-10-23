#include <CaixaDeLogica.h>

CaixaDeLogica caixa;

void setup() {

}

boolean apertou[] = {false,false,false,false};
long notas[] = {NOTA_DO,NOTA_RE,NOTA_MI,NOTA_FA};

void loop() {
  for(int i=0;i<4;i++)
  {
    if(caixa.lerBotao(i))
    {
      if(!apertou[i])
      {
        apertou[i] = true;
        caixa.setarLed(i,0,HIGH);
        caixa.tocarNota(notas[i],5000,false);
      }
    }
    else
    {
      if(apertou[i])
      {
        caixa.setarLed(i,0,LOW);
        caixa.pararNota();
        apertou[i] = false;
      }
    } 
  }
}



