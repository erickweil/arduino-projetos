# ESP32-C3 (Super Mini) com Rust e std
Placa desse código: https://www.makerhero.com/produto/placa-esp32-c3/?srsltid=AfmBOopX1vOjnHhnhjBJ3-Befi_SLjP5jsdBXte9DcKYhzjCo7a4QMjk

Este projeto foi criado seguindo o tutorial abaixo:
- https://github.com/esp-rs/esp-idf-template

Primeiro siga os passos do esp-idf-template para configurar o ambiente de desenvolvimento.

Então poderá usar os comandos Make para compilar, fazer flash e monitorar a porta serial da placa. Exemplo:
- make flash
- make monitor

> Altere o valor da variável EXAMPLE no Makefile para compilar e rodar outros exemplos.

Também é possível rodar testes, mas eles não são executados no aparelho, apenas no host. Para rodar os testes, use o comando:
- make test

> Códigos que podem ser testados devem ser escritos de forma a não depender de recursos específicos do ESP-IDF, ou seja, devem ser escritos usando apenas a biblioteca padrão do Rust e estarem localizados fora do macro espidf_only!

## Testes no host e o macro `espidf_only!`

O projeto fornece a macro `espidf_only!` (./src/lib.rs), a ideia é permitir ter testes no host sem precisar de hardware.

```rust
// Importações e funções comuns a todos os ambientes (host e ESP-IDF)
use std::{thread, time::Duration};

fn soma(a: i32, b: i32) -> i32 {
    a + b
}

// Código que usa recursos do ESP-IDF deve ficar dentro do macro espidf_only!
// Inclusive imports e funções (específicos)
espidf_std::espidf_only! {
    use esp_idf_svc::hal::{gpio::PinDriver, peripherals::Peripherals};

    // A função main deve ser public e retornar um Result deste tipo
    pub fn main() -> Result<(), Box<dyn std::error::Error>> {
        esp_idf_svc::sys::link_patches();
        esp_idf_svc::log::EspLogger::initialize_default();

        log::info!("Hello, ESP-IDF! 3 + 5 = {}", soma(3, 5));

        Ok(())
    }
}

// Aqui testamos a função soma() sem depender do ESP-IDF, ou seja, sem precisar de hardware. O teste é executado no computador host.
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_it() {
        assert_eq!(soma(3, 5), 8);
    }
}
```

Para detalhes veja o exemplo blink (./examples/blink.rs)

Mais recursos:
- https://esp-rs.github.io/std-training/
- https://github.com/esp-rs/esp-idf-svc/tree/master/examples
- https://github.com/esp-rs/esp-idf-hal/tree/master/examples
- https://github.com/esp-rs/esp-idf-sys/tree/master/examples

## Roadmap

A ideia aqui é validar o uso do Rust em pequenos exemplos para os requisitos básicos de projetos em geral, mas começando com o que o sistema de rastreio precisaria.

Esp32C3 Super Mini:
- [x] ./examples/primos.rs Rust com suporte std para o ESP32 (esp-idf-template)
- [x] ./examples/blink_http_server/ Utilização do Wifi
- [x] Utilizar Sistema de arquivos LittleFS
- [ ] Comunicação MQTT

Heltec Wireless Tracker (Esp32S3FN8):
- [x] ./examples/tft-st7735.rs Utilizar Display ST7735
- [ ] Interação com módulo GPS via UART
- [ ] Comunicação LoRaWAN

Se tudo isso funcionar criar novo projeto de rastreio em rust. 