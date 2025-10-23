#include <CaixaDeLogica.h>

CaixaDeLogica caixa;

void setup() {

}

void loop() {
  int botaoApertado = caixa.qualBotaoApertado();
  if(botaoApertado != -1)
  {
    // DY é a largura y da matriz de leds
    for(int i=0;i<DY;i++)
    {
      caixa.setarLed(botaoApertado,i,true);
      delay(100);
      caixa.setarLed(botaoApertado,i,false);
      delay(100);
    }
  }
}



