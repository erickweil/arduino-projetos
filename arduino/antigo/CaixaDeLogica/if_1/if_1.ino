#include <CaixaDeLogica.h>

CaixaDeLogica caixa;

void setup() {

}

void loop() {
  int botaoApertado = caixa.qualBotaoApertado();
  if(botaoApertado != -1)
  {
    caixa.setarLed(botaoApertado,0,true);
    delay(200);
    caixa.setarLed(botaoApertado,0,false);
    delay(200);
  }

}



