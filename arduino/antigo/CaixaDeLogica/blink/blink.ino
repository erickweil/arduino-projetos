#include <CaixaDeLogica.h>

CaixaDeLogica caixa;

void setup() {

}

void loop() {
  caixa.setarLed(0,0,true);
  delay(500);
  caixa.setarLed(0,0,false);
  delay(500);
}



