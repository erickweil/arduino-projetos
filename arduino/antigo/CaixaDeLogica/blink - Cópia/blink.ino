#include <CaixaDeLogica.h>

CaixaDeLogica caixa;

void setup() {

}

void loop() {
  int x = random(DX);
  int y = random(DY);
  int t1 = random(1,500);
  int t2 = random(1,500);
  caixa.setarLed(x,y,true);
  delay(t);
  caixa.setarLed(x,y,false);
  delay(t2);
}



