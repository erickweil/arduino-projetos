#include <CaixaDeLogica.h>

CaixaDeLogica caixa;

void setup() {

}

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
  }
  
  estava_apertando = (botaoApertado != -1);
}







