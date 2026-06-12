/// DEPRECATED: mantido para referência, mas decidi fazer tudo inline mesmo
/// Mapeamento de pinos nomeados para o Heltec Wireless Tracker (ESP32-S3FN8).
/// https://heltec.org/project/wireless-tracker/
///
/// Referência: Wireless Tracker V1.1 datasheet (Headers J2 e J3)
/// https://resource.heltec.cn/download/Wireless_Tracker/Wireless%20Tracker1.1.pdf
///
/// # Atenção — ordem de inicialização obrigatória
///
/// 1. **`vext_ctrl` (GPIO3) → HIGH** antes de usar o TFT ou o GNSS.
///    O pino Vext fornece 3.3V para ambos os módulos; sem ele nada funciona.
///
/// 2. **`adc_ctrl` (GPIO2) → HIGH** antes de ler `vbat_read` (GPIO1).
///    GPIO2 controla o circuito divisor de tensão da bateria.
///    Sem colocá-lo em HIGH a leitura retorna valores incorretos.
///    Fórmula: `VBAT (V) = leitura_adc * 4.9 / resolução_adc`
///
/// 3. **`tft_bl` (GPIO21) → HIGH** para acender o backlight do display.
use esp_idf_svc::hal::gpio::{AnyIOPin, AnyInputPin, AnyOutputPin, Pins};

pub struct OtherPins<'a> {
    pub gpio4:  AnyIOPin<'a>,
    pub gpio5:  AnyIOPin<'a>,
    pub gpio6:  AnyIOPin<'a>,
    pub gpio7:  AnyIOPin<'a>,
    pub gpio15: AnyIOPin<'a>,
    pub gpio16: AnyIOPin<'a>,
    pub gpio17: AnyIOPin<'a>,
    pub gpio19: AnyIOPin<'a>,
    pub gpio20: AnyIOPin<'a>,
    pub gpio37: AnyIOPin<'a>,
    pub gpio43: AnyIOPin<'a>,
    pub gpio44: AnyIOPin<'a>,
    pub gpio45: AnyIOPin<'a>,
    pub gpio46: AnyIOPin<'a>,
    pub gpio47: AnyIOPin<'a>,
    pub gpio48: AnyIOPin<'a>,
}

/// Pinos do Heltec Wireless Tracker com nomes funcionais.
///
/// Obtenha via [`WirelessTrackerPins::new`] passando `peripherals.pins`.
pub struct WirelessTrackerPins<'a> {
    // ---------------------------------------------------------------
    // Alimentação e controle
    // ---------------------------------------------------------------
    /// GPIO3 — `Vext Ctrl`
    ///
    /// Controla a saída Vext (3.3V, até 350 mA) que alimenta o display
    /// TFT onboard e o módulo GNSS UC6580.
    /// **Deve ser colocado em HIGH antes de usar qualquer um deles.**
    pub vext_ctrl: AnyOutputPin<'a>,

    /// GPIO2 — `ADC Ctrl`
    ///
    /// Habilita o circuito divisor de tensão para leitura da bateria.
    /// **Deve ser colocado em HIGH antes de ler `vbat_read` (GPIO1).**
    /// Mantenha em LOW quando não estiver lendo para economizar energia.
    pub adc_ctrl: AnyOutputPin<'a>,

    // ---------------------------------------------------------------
    // Display TFT ST7735S — 0.96", 160×80 px, SPI
    // (Header J3, pinos 13–18; backlight no Header J2 pino 8)
    // ---------------------------------------------------------------
    /// GPIO42 — `TFT_SDIN`: linha MOSI do SPI do display
    pub tft_mosi: AnyOutputPin<'a>,

    /// GPIO41 — `TFT_SCLK`: clock SPI do display
    pub tft_sclk: AnyOutputPin<'a>,

    /// GPIO40 — `TFT_RS`: seleção Dado/Comando (D/C)
    ///
    /// LOW = comando, HIGH = dado. Chamado "RS" no datasheet e "DC"
    /// na maioria dos drivers de display (ex.: `mipidsi`).
    pub tft_dc: AnyOutputPin<'a>,

    /// GPIO39 — `TFT_RES`: reset do display (ativo em LOW)
    pub tft_rst: AnyOutputPin<'a>,

    /// GPIO38 — `TFT_CS`: chip select do display (ativo em LOW)
    pub tft_cs: AnyOutputPin<'a>,

    /// GPIO21 — `TFT_LED_K`: controle do backlight
    ///
    /// HIGH = backlight ligado. Só funciona se `vext_ctrl` estiver HIGH.
    pub tft_bl: AnyOutputPin<'a>,

    // ---------------------------------------------------------------
    // LoRa SX1262 — SPI dedicado
    // (Header J3, pinos 6–12)
    // ---------------------------------------------------------------
    /// GPIO9  — `LoRa_SCK`: clock SPI do LoRa
    pub lora_sck: AnyOutputPin<'a>,

    /// GPIO10 — `LoRa_MOSI`: dados ESP → SX1262
    pub lora_mosi: AnyOutputPin<'a>,

    /// GPIO11 — `LoRa_MISO`: dados SX1262 → ESP
    pub lora_miso: AnyOutputPin<'a>,

    /// GPIO8  — `LoRa_NSS`: chip select do LoRa (ativo em LOW)
    ///
    /// Chamado "NSS" no SX1262 e equivalente ao CS/SS do SPI.
    pub lora_cs: AnyOutputPin<'a>,

    /// GPIO12 — `LoRa_RST`: reset do SX1262 (ativo em LOW)
    pub lora_rst: AnyOutputPin<'a>,

    /// GPIO13 — `LoRa_Busy`: indica que o SX1262 está ocupado (HIGH = ocupado)
    ///
    /// Configure como entrada. Aguarde LOW antes de enviar comandos SPI.
    pub lora_busy: AnyInputPin<'a>,

    /// GPIO14 — `LoRa_DIO1`: linha de interrupção do SX1262
    ///
    /// Usada para IRQ de transmissão/recepção concluída. Configure como
    /// entrada com interrupção de borda de subida.
    pub lora_dio1: AnyInputPin<'a>,

    // ---------------------------------------------------------------
    // GNSS UC6580 — UART
    // (Header J2, pinos 12–15)
    // ---------------------------------------------------------------
    /// GPIO33 — `GNSS_TX`: pino TX do módulo UC6580
    ///
    /// Do ponto de vista do ESP-IDF (`UartDriver`), este é o pino **RX**
    /// (ESP recebe os dados NMEA/Unicore do módulo).
    /// Só funciona com `vext_ctrl` em HIGH.
    pub gnss_module_tx: AnyInputPin<'a>,

    /// GPIO34 — `GNSS_RX`: pino RX do módulo UC6580
    ///
    /// Do ponto de vista do ESP-IDF (`UartDriver`), este é o pino **TX**
    /// (ESP envia comandos de configuração ao módulo).
    /// Só funciona com `vext_ctrl` em HIGH.
    pub gnss_module_rx: AnyOutputPin<'a>,

    /// GPIO35 — `GNSS_RST`: reset do UC6580 (ativo em LOW)
    ///
    /// Só funciona com `vext_ctrl` em HIGH.
    pub gnss_rst: AnyOutputPin<'a>,

    /// GPIO36 — `GNSS_PPS`: Pulse Per Second do UC6580
    ///
    /// Pulso de 1 Hz sincronizado com o tempo GPS. Configure como entrada.
    /// Útil para sincronização precisa de tempo. Só funciona com `vext_ctrl` em HIGH.
    pub gnss_pps: AnyInputPin<'a>,

    // ---------------------------------------------------------------
    // LED onboard
    // ---------------------------------------------------------------
    /// GPIO18 — `LED Write Ctrl` (Header J3, pino 1)
    ///
    /// Controle do LED onboard. Provavelmente endereçável (WS2812 ou similar);
    /// use protocolo de LED RGB se for o caso.
    pub led_ctrl: AnyOutputPin<'a>,

    // ---------------------------------------------------------------
    // Botão e entradas do usuário
    // ---------------------------------------------------------------
    /// GPIO0 — `USER_SW` (Header J2, pino 2)
    ///
    /// Botão do usuário. Configure como entrada com pull-up interno.
    /// Ativo em LOW (pressionado = LOW).
    /// Atenção: GPIO0 também é usado no boot do ESP32-S3 — não segure
    /// pressionado ao ligar para não entrar em modo de download.
    pub user_btn: AnyInputPin<'a>,

    /// GPIO1 — `Vbat_Read` (Header J2, pino 3)
    ///
    /// Leitura ADC da tensão da bateria via divisor resistivo.
    /// Fórmula: `VBAT (V) = leitura_adc * 4.9 / resolução_adc`
    ///
    /// **Ative `adc_ctrl` (GPIO2) em HIGH antes de ler, e retorne para
    /// LOW após a leitura para economizar energia.**
    pub vbat_read: AnyInputPin<'a>,

    pub pins: OtherPins<'a>,
}

impl<'a> WirelessTrackerPins<'a> {
    /// Constrói o mapeamento de pinos a partir do [`Pins`] do `esp-idf-hal`.
    ///
    /// # Exemplo
    /// ```no_run
    /// let peripherals = Peripherals::take().unwrap();
    /// let board = WirelessTrackerPins::new(peripherals.pins);
    /// ```
    #[cfg(feature = "esp32s3")]
    pub fn new(pins: Pins) -> Self {
        Self {
            vext_ctrl: pins.gpio3.into(),
            adc_ctrl: pins.gpio2.into(),
            tft_mosi: pins.gpio42.into(),
            tft_sclk: pins.gpio41.into(),
            tft_dc: pins.gpio40.into(),
            tft_rst: pins.gpio39.into(),
            tft_cs: pins.gpio38.into(),
            tft_bl: pins.gpio21.into(),
            lora_sck: pins.gpio9.into(),
            lora_mosi: pins.gpio10.into(),
            lora_miso: pins.gpio11.into(),
            lora_cs: pins.gpio8.into(),
            lora_rst: pins.gpio12.into(),
            lora_busy: pins.gpio13.into(),
            lora_dio1: pins.gpio14.into(),
            gnss_module_tx: pins.gpio33.into(),
            gnss_module_rx: pins.gpio34.into(),
            gnss_rst: pins.gpio35.into(),
            gnss_pps: pins.gpio36.into(),
            led_ctrl: pins.gpio18.into(),
            user_btn: pins.gpio0.into(),
            vbat_read: pins.gpio1.into(),
            pins: OtherPins {
                gpio4: pins.gpio4.into(),
                gpio5: pins.gpio5.into(),
                gpio6: pins.gpio6.into(),
                gpio7: pins.gpio7.into(),
                gpio15: pins.gpio15.into(),
                gpio16: pins.gpio16.into(),
                gpio17: pins.gpio17.into(),
                gpio19: pins.gpio19.into(),
                gpio20: pins.gpio20.into(),
                gpio37: pins.gpio37.into(),
                gpio43: pins.gpio43.into(),
                gpio44: pins.gpio44.into(),
                gpio45: pins.gpio45.into(),
                gpio46: pins.gpio46.into(),
                gpio47: pins.gpio47.into(),
                gpio48: pins.gpio48.into(),
            },
        }
    }

    #[cfg(not(feature = "esp32s3"))]
    pub fn new(_pins: Pins) -> Self {
        panic!("Não é a placa correta");
    }
}