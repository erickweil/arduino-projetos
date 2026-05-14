# ESP32-C3 (Super Mini) com Rust e std
Placa desse código: https://www.makerhero.com/produto/placa-esp32-c3/?srsltid=AfmBOopX1vOjnHhnhjBJ3-Befi_SLjP5jsdBXte9DcKYhzjCo7a4QMjk

Este projeto foi criado seguindo o tutorial abaixo:
- https://github.com/esp-rs/esp-idf-template

Primeiro siga os passos do esp-idf-template para configurar o ambiente de desenvolvimento.

Então poderá usar os comandos Make para compilar, fazer flash e monitorar a porta serial da placa. Exemplo:
- make flash
- make monitor

> Altere o valor da variável EXAMPLE no Makefile para compilar e rodar outros exemplos. ou passe via terminal Exemplo:
> - EXAMPLE=blink make flash

Mais recursos:
- https://esp-rs.github.io/std-training/
- https://github.com/esp-rs/esp-idf-svc/tree/master/examples
- https://github.com/esp-rs/esp-idf-hal/tree/master/examples
- https://github.com/esp-rs/esp-idf-sys/tree/master/examples