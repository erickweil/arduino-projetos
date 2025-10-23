#include <CaixaDeLogica.h>

CaixaDeLogica caixa;

void setup() {

}

void loop() {
  for(int y=0; y<DYy++)
  {
    for(int x=0; x < DX;x++)
    {
      caixa.setarLed(x,y,true);
      delay(500);
      caixa.setarLed(x,y,false);
      delay(500);
    }
  }
}



