#include "Arduino.h"
// #define PISCAR_LED

#ifdef HELTEC_BOARD
#include "HT_st7735.h"
HT_st7735 st7735;
#endif

// detecta que placa é via #ifdef
bool ehPrimo(unsigned long n) {
  // Elimina todos os números pares de uma vez
  if (n % 2UL == 0) return false;

  for(unsigned long i = 3UL; i * i <= n; i+= 2UL) {
    if(n % i == 0) {
      return false;
    }
  }
  return true;
}

// 100 Milhões
unsigned long numero = 100000000UL;

bool ledAceso = false;
void setup() {
  Serial.begin(115200);
  // initialize digital pin LED_BUILTIN as an output.
  #ifdef PISCAR_LED
  pinMode(LED_BUILTIN, OUTPUT);
  #endif

  #ifdef HELTEC_BOARD
  st7735.st7735_init();
  delay(100);
  st7735.st7735_fill_screen(ST7735_BLACK);
  #endif

  Serial.println("Iniciando...");
}

void loop() {
  if(ehPrimo(numero)) {
    Serial.println(numero);

    #ifdef PISCAR_LED
    ledAceso = !ledAceso;
    if(ledAceso) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    #endif
    #ifdef HELTEC_BOARD
    char str_buf[15];
    snprintf(str_buf, sizeof(str_buf), "%lu", numero);

    st7735.st7735_write_str(0, 0, str_buf);
    #endif

    delay(10);
  }
  numero = numero + 1UL;
}