//! Blink de LED e servidor HTTP usando ESP-IDF e Rust
//! 
//! Conecte-se ao wifi e então acesse http://192.168.71.1/
//! 
//! Baseado em https://github.com/esp-rs/esp-idf-svc/blob/master/examples/wifi.rs
//! https://github.com/esp-rs/esp-idf-svc/blob/master/examples/http_server.rs
//! 

/// Importações e funções comuns a todos os ambientes (host e ESP-IDF)
use espidf_std::prelude::*;
use std::{thread, time::Duration};

/// Configurações de Wi-Fi
/// O intervalo permitido é de [8, 84], o que corresponde a uma potência real de 2 dBm a 20 dBm.
/// A unidade do parâmetro de potência (power) é de 0.25 dBm.
/// 34 * 0.25 dBm = 8.5 dBm
const MAX_RADIO_POWER: i8 = 34;

const SSID: &str = match option_env!("WIFI_SSID") {
    Some(ssid) => ssid,
    None => "Wokwi-GUEST",
};
const PASSWORD: &str = match option_env!("WIFI_PASS") {
    Some(pass) => pass,
    None => "",
};

// true para Access Point, false para Client
const WIFI_AP_MODE: bool = false;

// Html das páginas, sem precisar de alocação dinâmica (String)
static INDEX_HTML: &str = include_str!("index.html");
static HTML_LED_ON: &str = "<html><head><meta http-equiv=\"refresh\" content=\"2\"></head><body><h1>LED LIGADO</h1></body></html>";
static HTML_LED_OFF: &str = "<html><head><meta http-equiv=\"refresh\" content=\"2\"></head><body><h1>LED DESLIGADO</h1></body></html>";

// Need lots of stack to parse JSON
const STACK_SIZE: usize = 10240;

const CHANNEL: u8 = 2;

// Código que usa recursos do ESP-IDF deve ficar dentro do macro espidf_only!
// Inclusive imports e funções específicas
espidf_only! {
    use esp_idf_svc::{
        http::{Method, server::EspHttpServer}, 
        io::Write,
        sys::esp_wifi_set_max_tx_power, 
        wifi::{AuthMethod, ClientConfiguration, Configuration, AccessPointConfiguration},
    };

    use esp_idf_svc::hal::{gpio, peripherals::Peripherals};
    use std::sync::{Arc, Mutex};
    use esp_idf_svc::eventloop::EspSystemEventLoop;
    use esp_idf_svc::nvs::EspDefaultNvsPartition;
    use esp_idf_svc::wifi::{BlockingWifi, EspWifi};

    pub fn main() -> Result<()> {
        esp_idf_svc::sys::link_patches();
        esp_idf_svc::log::EspLogger::initialize_default();

        // Aparentemente, se você tentar ligar o wifi IMEDIATAMENTE após o boot, ele falha silenciosamente.
        log::info!("Aguardando estabilização da porta USB CDC (1 segundos)...");
        thread::sleep(Duration::from_secs(1));

        log::info!("Inicializando...");

        // Take the peripherals singleton
        let peripherals = Peripherals::take()?;
        let sys_loop = EspSystemEventLoop::take()?;
        let nvs = EspDefaultNvsPartition::take()?;

        let mut led_builtin = gpio::PinDriver::output(
            // Board: ESP32-C3 Super Mini, GPIO8
            if cfg!(feature = "esp32c3") { peripherals.pins.gpio8 } 
            else { panic!("Qual led usar?") }
        )?;
        led_builtin.set_high()?;

        let mut wifi = BlockingWifi::wrap(
            EspWifi::new(peripherals.modem, sys_loop.clone(), Some(nvs))?,
            sys_loop,
        )?;
        config_wifi(&mut wifi)?;

        let mut server = EspHttpServer::new(&esp_idf_svc::http::server::Configuration {
            stack_size: STACK_SIZE,
            ..Default::default()
        })?;
        config_server(&mut server, AppState { 
            led_builtin 
        })?;

        // Keep wifi and the server running beyond when main() returns (forever)
        // Do not call this if you ever want to stop or access them later.
        // Otherwise you can either add an infinite loop so the main task
        // never returns, or you can move them to another thread.
        // https://doc.rust-lang.org/stable/core/mem/fn.forget.html
        core::mem::forget(wifi);
        core::mem::forget(server);

        // Main task no longer needed, free up some memory
        Ok(())

        // Alternativa: manter a main task viva com um loop infinito
        // log::info!("Entrando em loop infinito...");
        // loop {
        //     thread::sleep(Duration::from_secs(1));
        // }
    }

    struct AppState {
        led_builtin: gpio::PinDriver<'static, gpio::Output>,
    }
    fn config_server(server: &mut EspHttpServer, app_state: AppState) -> Result<()> {
        // Arc<Mutex<T>> permite compartilhar estado entre múltiplas rotas de forma segura
        let app_state = Arc::new(Mutex::new(app_state));

        server.fn_handler("/", Method::Get, move |req| {
            req.into_ok_response()?
                .write_all(INDEX_HTML.as_bytes())
                .map(|_| ())
        })?;

        // Passa uma cópia do Arc para cada handler que precisar
        let handler_state = app_state.clone();
        server.fn_handler("/led", Method::Post, move |req| {
            let mut app_state = handler_state.lock().unwrap();

            app_state.led_builtin.toggle()?;
            
            // Redireciona de volta para a página principal
            req.into_response(302, None, &[("Location", "/")])
                .map(|_| ())
        })?;

        // no último handler pode fazer move do Arc
        let handler_state = app_state;
        server.fn_handler("/led", Method::Get, move |req| {
            let app_state = handler_state.lock().unwrap();

            let led_state = app_state.led_builtin.is_set_low();

            req.into_ok_response()?
                .write_all(if led_state { 
                    HTML_LED_ON
                 } else { 
                    HTML_LED_OFF
                 }.as_bytes())
                .map(|_| ())
        })?;

        Ok(())
    }

    fn config_wifi(wifi: &mut BlockingWifi<EspWifi<'static>>) -> Result<()> {
        let auth_method = if PASSWORD.is_empty() {
            AuthMethod::None
        } else {
            AuthMethod::WPAWPA2Personal
        };

        let wifi_configuration = if WIFI_AP_MODE {
            Configuration::AccessPoint(AccessPointConfiguration {
                ssid: SSID.try_into().unwrap(),
                ssid_hidden: false,
                auth_method,
                password: PASSWORD.try_into().unwrap(),
                channel: CHANNEL,
                ..Default::default()
            })
        } else {
            Configuration::Client(ClientConfiguration {
                ssid: SSID.try_into().unwrap(),
                bssid: None,
                auth_method,
                password: PASSWORD.try_into().unwrap(),
                channel: None,
                pmf_cfg: esp_idf_svc::wifi::PmfConfiguration::Capable { required: false },
                ..Default::default()
            })
        };

        wifi.set_configuration(&wifi_configuration)?;

        log::info!("Starting Wi-Fi...");
        wifi.start()?;

        // --- Redução da Potência de Transmissão (Hardware Brownout Fix) ---
        // O problema: o rádio RF liga e puxa um pico de corrente repentino que pode passar de 300mA.
        // Reduzir a potência de transmissão (ironicamente) ajuda a estabilizar o sinal nessas placas, pois diminui o ruído no circuito regulador de tensão interno.
        // SAFETY: é só um binding do código C
        unsafe {
            let err = esp_wifi_set_max_tx_power(MAX_RADIO_POWER);
            if err != 0 {
                log::warn!("Aviso: Falha ao ajustar TX power. Codigo do erro: {}", err);
            }
        }

        if WIFI_AP_MODE {
            log::info!("Created Wi-Fi AP with WIFI_SSID `{SSID}` and password `{PASSWORD}`");

            wifi.wait_netif_up()?;
            log::info!("Wifi netif up");

            let ip_info = wifi.wifi().ap_netif().get_ip_info()?;
            log::info!("Running on IP {}", ip_info.ip);
        } else {
            /*
                let aps: Vec<AccessPointInfo> = wifi.scan()?;
                log::info!("Redes encontradas ({}):", aps.len());
                for ap in &aps {
                    log::info!("  SSID: `{}` | Canal: {} | RSSI: {} dBm | Auth: {:?}",
                        ap.ssid, ap.channel, ap.signal_strength, ap.auth_method);
                }
            */

            // Só é necessário conectar se for modo cliente
            log::info!("Connecting Wi-Fi with WIFI_SSID `{SSID}` and password `{PASSWORD}`");
            wifi.connect()?;
            log::info!("Wifi connected");

            wifi.wait_netif_up()?;
            log::info!("Wifi netif up");

            let ip_info = wifi.wifi().sta_netif().get_ip_info()?;
            log::info!("Running on IP {}", ip_info.ip);
        }     

        Ok(())
    }
}
