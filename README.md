# 🤖 Arduino Projetos

Este repositório reúne todos os códigos Arduino antigos e projetos embarcados desenvolvidos ao longo do tempo. O objetivo é centralizar e organizar códigos para diferentes plataformas de hardware embarcado como Arduino, ESP32, ESP8266 e outros microcontroladores.

## 📋 Resumo do Conteúdo

A tabela abaixo fornece uma visão geral dos projetos e códigos disponíveis neste repositório:

| Pasta | Plataforma | Descrição | Status |
|-------|-----------|-----------|--------|
| *Em breve* | - | Projetos serão adicionados em breve | ⏳ |

> **Nota:** Esta tabela será atualizada conforme novos projetos forem adicionados ao repositório.

## 📁 Estrutura de Pastas Proposta

Para manter o repositório organizado e facilitar a navegação, sugere-se a seguinte estrutura de pastas:

```
arduino-projetos/
│
├── arduino/                    # Projetos para Arduino (Uno, Mega, Nano, etc.)
│   ├── basicos/               # Projetos básicos e tutoriais
│   ├── sensores/              # Projetos com sensores
│   ├── comunicacao/           # Projetos de comunicação (Serial, I2C, SPI)
│   └── avancados/             # Projetos avançados
│
├── esp32/                      # Projetos para ESP32
│   ├── wifi/                  # Projetos com WiFi
│   ├── bluetooth/             # Projetos com Bluetooth/BLE
│   ├── iot/                   # Projetos IoT
│   └── sensores/              # Projetos com sensores
│
├── esp8266/                    # Projetos para ESP8266
│   ├── wifi/                  # Projetos com WiFi
│   ├── webserver/             # Servidores web
│   └── iot/                   # Projetos IoT
│
├── testes/                     # Códigos de teste e experimentação
│   ├── arduino/               # Testes com Arduino
│   ├── esp32/                 # Testes com ESP32
│   └── esp8266/               # Testes com ESP8266
│
├── bibliotecas/                # Bibliotecas personalizadas
│   ├── sensores/              # Bibliotecas para sensores
│   ├── comunicacao/           # Bibliotecas de comunicação
│   └── utilitarios/           # Bibliotecas utilitárias
│
├── esquematicos/               # Esquemas elétricos e diagramas
│   ├── fritzing/              # Arquivos Fritzing
│   └── kicad/                 # Arquivos KiCad
│
└── docs/                       # Documentação adicional
    ├── tutoriais/             # Tutoriais e guias
    ├── datasheets/            # Datasheets de componentes
    └── referencias/           # Material de referência
```

## 🚀 Como Usar Este Repositório

### Navegação
1. Identifique a plataforma de hardware que você está usando (Arduino, ESP32, ESP8266)
2. Navegue até a pasta correspondente
3. Explore as subpastas temáticas para encontrar o projeto desejado

### Adicionando Novos Projetos
1. Escolha a pasta apropriada de acordo com a plataforma
2. Crie uma subpasta com nome descritivo do projeto
3. Inclua o código fonte (.ino, .cpp, .h)
4. Adicione um README.md na pasta do projeto com:
   - Descrição do projeto
   - Lista de componentes necessários
   - Esquema de conexões
   - Instruções de uso
5. Atualize a tabela de resumo neste README principal

### Estrutura de um Projeto Individual
```
nome-do-projeto/
├── README.md              # Documentação do projeto
├── nome-do-projeto.ino    # Código principal
├── config.h               # Configurações (se necessário)
├── esquematico.png        # Imagem do esquema (opcional)
└── bibliotecas/           # Bibliotecas específicas (se necessário)
```

## 🛠️ Plataformas Suportadas

- **Arduino**: Uno, Mega, Nano, Leonardo, Pro Mini
- **ESP32**: DevKit, WROOM, WROVER
- **ESP8266**: NodeMCU, Wemos D1 Mini
- **Outros**: STM32, Raspberry Pi Pico, etc.

## 📚 Recursos Úteis

- [Arduino Official Website](https://www.arduino.cc/)
- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [ESP8266 Community Forum](https://www.esp8266.com/)
- [PlatformIO](https://platformio.org/) - IDE alternativa para desenvolvimento embarcado

## 📝 Convenções de Código

- Use nomes descritivos para variáveis e funções
- Comente código complexo
- Siga o estilo de código padrão do Arduino
- Inclua cabeçalhos com informações de autor e data

## 🤝 Contribuindo

Este é um repositório pessoal de códigos Arduino antigos e novos projetos. Sugestões e melhorias são bem-vindas!

## 📄 Licença

Este projeto está sob a licença especificada no arquivo LICENSE.
