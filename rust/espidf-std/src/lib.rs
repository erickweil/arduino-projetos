pub type Result<T> = std::result::Result<T, Box<dyn std::error::Error>>;

/// Macro para encapsular o boilerplate para rodar o código apenas no aparelho
/// e ainda poder ter testes de código puro que rodam no host. 
/// 
/// A ideia é incluir dentro deste macro apenas código específico que depende
/// de recursos do ambiente do ESP-IDF, como acesso a GPIO, Wi-Fi, etc.
/// 
/// Exemplo:
/// ```
/// espidf_std::espidf_only! {
///     pub fn main() -> Result<(), Box<dyn std::error::Error>> {
///         // código do exemplo aqui
///         Ok(())
///     }
/// }
/// ```
#[macro_export]
macro_rules! espidf_only {
    ($($body:tt)*) => {
        #[cfg(feature = "espidf")]
        fn main() -> $crate::Result<()> {
            example::main()
        }

        #[cfg(not(feature = "espidf"))]
        fn main() {
            panic!("Deveria rodar apenas na placa e não no host! Verifique se configurou para usar a feature da placa correta");
        }

        #[cfg(feature = "espidf")]
        mod example {
            use super::*;

            $($body)*
        }
    };
}

pub mod prelude {
    pub use crate::Result;
    pub use crate::espidf_only;
}

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
#[cfg(feature = "esp32s3")]
pub mod wireless_tracker {
    use esp_idf_svc::hal::gpio::{
        Gpio0, Gpio1, Gpio2, Gpio3,
        Gpio8, Gpio9, Gpio10, Gpio11, Gpio12, Gpio13, Gpio14,
        Gpio18, Gpio21,
        Gpio33, Gpio34, Gpio35, Gpio36,
        Gpio38, Gpio39, Gpio40, Gpio41, Gpio42,
    };
    use esp_idf_svc::hal::gpio::Pins;

    /// Pinos do Heltec Wireless Tracker com nomes funcionais.
    ///
    /// Obtenha via [`WirelessTrackerPins::new`] passando `peripherals.pins`.
    pub struct WirelessTrackerPins {
        // ---------------------------------------------------------------
        // Alimentação e controle
        // ---------------------------------------------------------------

        /// GPIO3 — `Vext Ctrl` (Header J2, pino 5)
        ///
        /// Controla a saída Vext (3.3V, até 350 mA) que alimenta o display
        /// TFT onboard e o módulo GNSS UC6580.
        /// **Deve ser colocado em HIGH antes de usar qualquer um deles.**
        pub vext_ctrl: Gpio3<'static>,

        /// GPIO2 — `ADC Ctrl` (Header J2, pino 4)
        ///
        /// Habilita o circuito divisor de tensão para leitura da bateria.
        /// **Deve ser colocado em HIGH antes de ler `vbat_read` (GPIO1).**
        /// Mantenha em LOW quando não estiver lendo para economizar energia.
        pub adc_ctrl: Gpio2<'static>,

        // ---------------------------------------------------------------
        // Display TFT ST7735S — 0.96", 160×80 px, SPI
        // (Header J3, pinos 13–18; backlight no Header J2 pino 8)
        // ---------------------------------------------------------------

        /// GPIO42 — `TFT_SDIN`: linha MOSI do SPI do display
        pub tft_mosi: Gpio42<'static>,

        /// GPIO41 — `TFT_SCLK`: clock SPI do display
        pub tft_sclk: Gpio41<'static>,

        /// GPIO40 — `TFT_RS`: seleção Dado/Comando (D/C)
        ///
        /// LOW = comando, HIGH = dado. Chamado "RS" no datasheet e "DC"
        /// na maioria dos drivers de display (ex.: `mipidsi`).
        pub tft_dc: Gpio40<'static>,

        /// GPIO39 — `TFT_RES`: reset do display (ativo em LOW)
        pub tft_rst: Gpio39<'static>,

        /// GPIO38 — `TFT_CS`: chip select do display (ativo em LOW)
        pub tft_cs: Gpio38<'static>,

        /// GPIO21 — `TFT_LED_K`: controle do backlight
        ///
        /// HIGH = backlight ligado. Só funciona se `vext_ctrl` estiver HIGH.
        pub tft_bl: Gpio21<'static>,

        // ---------------------------------------------------------------
        // LoRa SX1262 — SPI dedicado
        // (Header J3, pinos 6–12)
        // ---------------------------------------------------------------

        /// GPIO9  — `LoRa_SCK`: clock SPI do LoRa
        pub lora_sck: Gpio9<'static>,

        /// GPIO10 — `LoRa_MOSI`: dados ESP → SX1262
        pub lora_mosi: Gpio10<'static>,

        /// GPIO11 — `LoRa_MISO`: dados SX1262 → ESP
        pub lora_miso: Gpio11<'static>,

        /// GPIO8  — `LoRa_NSS`: chip select do LoRa (ativo em LOW)
        ///
        /// Chamado "NSS" no SX1262 e equivalente ao CS/SS do SPI.
        pub lora_cs: Gpio8<'static>,

        /// GPIO12 — `LoRa_RST`: reset do SX1262 (ativo em LOW)
        pub lora_rst: Gpio12<'static>,

        /// GPIO13 — `LoRa_Busy`: indica que o SX1262 está ocupado (HIGH = ocupado)
        ///
        /// Configure como entrada. Aguarde LOW antes de enviar comandos SPI.
        pub lora_busy: Gpio13<'static>,

        /// GPIO14 — `LoRa_DIO1`: linha de interrupção do SX1262
        ///
        /// Usada para IRQ de transmissão/recepção concluída. Configure como
        /// entrada com interrupção de borda de subida.
        pub lora_dio1: Gpio14<'static>,

        // ---------------------------------------------------------------
        // GNSS UC6580 — UART
        // (Header J2, pinos 12–15)
        // ---------------------------------------------------------------

        /// GPIO33 — `GNSS_TX`: pino TX do módulo UC6580
        ///
        /// Do ponto de vista do ESP-IDF (`UartDriver`), este é o pino **RX**
        /// (ESP recebe os dados NMEA/Unicore do módulo).
        /// Só funciona com `vext_ctrl` em HIGH.
        pub gnss_module_tx: Gpio33<'static>,

        /// GPIO34 — `GNSS_RX`: pino RX do módulo UC6580
        ///
        /// Do ponto de vista do ESP-IDF (`UartDriver`), este é o pino **TX**
        /// (ESP envia comandos de configuração ao módulo).
        /// Só funciona com `vext_ctrl` em HIGH.
        pub gnss_module_rx: Gpio34<'static>,

        /// GPIO35 — `GNSS_RST`: reset do UC6580 (ativo em LOW)
        ///
        /// Só funciona com `vext_ctrl` em HIGH.
        pub gnss_rst: Gpio35<'static>,

        /// GPIO36 — `GNSS_PPS`: Pulse Per Second do UC6580
        ///
        /// Pulso de 1 Hz sincronizado com o tempo GPS. Configure como entrada.
        /// Útil para sincronização precisa de tempo. Só funciona com `vext_ctrl` em HIGH.
        pub gnss_pps: Gpio36<'static>,

        // ---------------------------------------------------------------
        // LED onboard
        // ---------------------------------------------------------------

        /// GPIO18 — `LED Write Ctrl` (Header J3, pino 1)
        ///
        /// Controle do LED onboard. Provavelmente endereçável (WS2812 ou similar);
        /// use protocolo de LED RGB se for o caso.
        pub led_ctrl: Gpio18<'static>,

        // ---------------------------------------------------------------
        // Botão e entradas do usuário
        // ---------------------------------------------------------------

        /// GPIO0 — `USER_SW` (Header J2, pino 2)
        ///
        /// Botão do usuário. Configure como entrada com pull-up interno.
        /// Ativo em LOW (pressionado = LOW).
        /// Atenção: GPIO0 também é usado no boot do ESP32-S3 — não segure
        /// pressionado ao ligar para não entrar em modo de download.
        pub user_btn: Gpio0<'static>,

        /// GPIO1 — `Vbat_Read` (Header J2, pino 3)
        ///
        /// Leitura ADC da tensão da bateria via divisor resistivo.
        /// Fórmula: `VBAT (V) = leitura_adc * 4.9 / resolução_adc`
        ///
        /// **Ative `adc_ctrl` (GPIO2) em HIGH antes de ler, e retorne para
        /// LOW após a leitura para economizar energia.**
        pub vbat_read: Gpio1<'static>,
    }

    impl WirelessTrackerPins {
        /// Constrói o mapeamento de pinos a partir do [`Pins`] do `esp-idf-hal`.
        ///
        /// # Exemplo
        /// ```no_run
        /// let peripherals = Peripherals::take().unwrap();
        /// let board = WirelessTrackerPins::new(peripherals.pins);
        /// ```
        pub fn new(pins: Pins) -> Self {
            Self {
                vext_ctrl:      pins.gpio3,
                adc_ctrl:       pins.gpio2,
                tft_mosi:       pins.gpio42,
                tft_sclk:       pins.gpio41,
                tft_dc:         pins.gpio40,
                tft_rst:        pins.gpio39,
                tft_cs:         pins.gpio38,
                tft_bl:         pins.gpio21,
                lora_sck:       pins.gpio9,
                lora_mosi:      pins.gpio10,
                lora_miso:      pins.gpio11,
                lora_cs:        pins.gpio8,
                lora_rst:       pins.gpio12,
                lora_busy:      pins.gpio13,
                lora_dio1:      pins.gpio14,
                gnss_module_tx: pins.gpio33,
                gnss_module_rx: pins.gpio34,
                gnss_rst:       pins.gpio35,
                gnss_pps:       pins.gpio36,
                led_ctrl:       pins.gpio18,
                user_btn:       pins.gpio0,
                vbat_read:      pins.gpio1,
            }
        }
    }
}