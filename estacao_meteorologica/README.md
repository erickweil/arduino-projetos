# Códigos Microcontroladores
Para ficar fácil trabalhar com arduino, esp8266, esp32 e etc... Aqui estão todo os código realizados e testes relacionados.

O que está sendo usado agora é **[EstacaoNovo.ino](./EstacaoNovo.ino)** e só funciona em um arduino Uno, e devido às limitações do UNO, envia os dados via uma conexão HTTP (Texto plano). 

Futuramente estudar como utilizar a biblioteca WeatherSensor.h com algum outro microcontrolador mais potente, como o ESP32 ou ESP8266, UNO R4, etc... que tenham a capacidade de processamento para executar uma conexão HTTPS, porém devido ao uso de interruptores e registradores específicos do arduino UNO na comunicação via rádio dessa biblioteca, fica a questão como realizar tal mudança.


# Listagem dos códigos que estão nesta pasta

## Arduino UNO (Shield Ethernet + HTTP)
- [EstacaoAlexandreOld.ino](./EstacaoAlexandreOld.ino) - Conecta à estação e envia dados para API. Primeira versão feita pelo Alexandre 
- [EstacaoAlexandreOld2.ino](./EstacaoAlexandreOld2.ino) - Código atualizado, organizado com classes e otimizações. (Disponível em https://github.com/Alxdelira/estacao_arduino/blob/main/hardware.ino)
- [EstacaoRobertoThingSpeak.ino](./EstacaoRobertoThingSpeak.ino) - Conecta à estação e envia dados para plataforma ThingSpeak com protocolo próprio. Código feito pelo Roberto Guimarães.
- [EstacaoNovo.ino](./EstacaoNovo.ino) - **Última versão**, Conecta à estação e envia dados para API, autenticado com token. Código feito por Erick Weil.

## Arduino Uno R4 (Wifi + HTTPS)
- [EstacaoTesteUnoR4.ino](./EstacaoTesteUnoR4.ino) - Apenas testes, Experimentos enviar dados à API utilizando conexão via Wifi, HTTPS e autenticação com Token. Código feito por Erick Weil. (Falta descobrir como integrar a biblioteca WeatherSensor.h)

## WeMos D1 R2 (Wifi + HTTPS)
- [Wifi.ino](./Wifi.ino) - Apenas testes, Conectar ao Wifi e encaminhar dados do serial.
- [Wifi2.ino](./Wifi2.ino) - Apenas testes, Conectar ao Wifi e encaminhar dados do serial (Utilizando SUART, segunda conexão serial via software).

Tutorial instalar e rodar programa no WeMos D1 R2 https://www.youtube.com/watch?v=yehyUmUDJXc
Driver CH340 https://sparks.gogo.co.nz/ch340.html
Github link biblioteca adicional arduino https://github.com/esp8266/Arduino

Outro tutorial https://support.envistiamall.com/kb/wemos-d1-r2-board-esp8266-arduino-nodemcu-development-board/

Exemplo conectar WI FI https://nerd-corner.com/wemos-d1-r2-setup-and-wifi-integration/
