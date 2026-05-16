use esp_idf_svc::{http::{Method, server::EspHttpServer}, sys::{esp_wifi_get_max_tx_power, esp_wifi_set_max_tx_power}, wifi::AccessPointConfiguration};
/// Blink de LED e servidor HTTP usando ESP-IDF e Rust
/// Baseado em https://github.com/esp-rs/esp-idf-svc/blob/master/examples/wifi.rs
/// https://github.com/esp-rs/esp-idf-svc/blob/master/examples/http_server.rs
/// 

// Importações e funções comuns a todos os ambientes (host e ESP-IDF)
use espidf_std::prelude::*;
use std::{cell::RefCell, thread, time::Duration};

// Configurações de Wi-Fi
// 34 * 0.25 dBm = 8.5 dBm
const MAX_RADIO_POWER: i8 = 34;

const SSID: &str = match option_env!("WIFI_SSID") {
    Some(ssid) => ssid,
    None => env!("MCU"),
};
const PASSWORD: &str = match option_env!("WIFI_PASS") {
    Some(pass) => pass,
    None => "12345678",
};

static INDEX_HTML: &str = include_str!("index.html");

// Need lots of stack to parse JSON
const STACK_SIZE: usize = 10240;

const CHANNEL: u8 = 2;

// Código que usa recursos do ESP-IDF deve ficar dentro do macro espidf_only!
// Inclusive imports e funções específicas
espidf_only! {
    use esp_idf_svc::{
        http::{Headers, Method},
        io::{Read, Write},
        wifi::{AuthMethod, AccessPointInfo, ClientConfiguration, Configuration},
    };
    use esp_idf_svc::hal::{gpio::PinDriver, peripherals::Peripherals};
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

        // Configure the LED pin (GPIO8) as output
        // Board: ESP32-C3 Super Mini
        let mut led_builtin = PinDriver::output(peripherals.pins.gpio8)?;
        led_builtin.set_high()?;

        let mut wifi = BlockingWifi::wrap(
            EspWifi::new(peripherals.modem, sys_loop.clone(), Some(nvs))?,
            sys_loop,
        )?;
        config_wifi_ap(&mut wifi)?;

        let mut server = EspHttpServer::new(&esp_idf_svc::http::server::Configuration {
            stack_size: STACK_SIZE,
            ..Default::default()
        })?;

        // Com uma única rota pode passar o ownership, mas precisa de RefCell para mutabilidade
        let led_builtin = RefCell::new(led_builtin);
        server.fn_handler("/", Method::Get, move |req| {
            // detecta o parâmetro de query `led` e liga/desliga o LED de acordo
            let query = req.uri().split('?').nth(1).unwrap_or("");
            if query.contains("led=on") {
                led_builtin.borrow_mut().set_low()?;
            } else if query.contains("led=off") {
                led_builtin.borrow_mut().set_high()?;
            }

            req.into_ok_response()?
                .write_all(INDEX_HTML.as_bytes())
                .map(|_| ())
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

    fn config_wifi_ap(wifi: &mut BlockingWifi<EspWifi<'static>>) -> Result<()> {
        /*let wifi_configuration: Configuration = Configuration::Client(ClientConfiguration {
            ssid: SSID.trim().try_into().unwrap(),
            bssid: None,
            auth_method: AuthMethod::WPAWPA2Personal,
            password: PASSWORD.trim().try_into().unwrap(),
            channel: None,
            pmf_cfg: esp_idf_svc::wifi::PmfConfiguration::Capable { required: false },
            ..Default::default()
        });*/

        // If instead of creating a new network you want to serve the page
        // on your local network, you can replace this configuration with
        // the client configuration from the http_client example.
        let wifi_configuration = Configuration::AccessPoint(AccessPointConfiguration {
            ssid: SSID.try_into().unwrap(),
            ssid_hidden: false,
            auth_method: AuthMethod::WPA2Personal,
            password: PASSWORD.try_into().unwrap(),
            channel: CHANNEL,
            ..Default::default()
        });

        wifi.set_configuration(&wifi_configuration)?;

        log::info!("Starting Wi-Fi...");
        wifi.start()?;

        // --- Redução da Potência de Transmissão (Hardware Brownout Fix) ---
        // O problema: o rádio RF liga e puxa um pico de corrente repentino que pode passar de 300mA.
        // Reduzir a potência de transmissão (ironicamente) ajuda a estabilizar o sinal nessas placas, pois diminui o ruído no circuito regulador de tensão interno.
        unsafe {
            let err = esp_wifi_set_max_tx_power(MAX_RADIO_POWER);
            if err != 0 {
                log::warn!("Aviso: Falha ao ajustar TX power. Codigo do erro: {}", err);
            }
        }

        /*
        // Só é necessário conectar se for modo cliente
        let aps: Vec<AccessPointInfo> = wifi.scan()?;
        log::info!("Redes encontradas ({}):", aps.len());
        for ap in &aps {
            log::info!("  SSID: `{}` | Canal: {} | RSSI: {} dBm | Auth: {:?}",
                ap.ssid, ap.channel, ap.signal_strength, ap.auth_method);
        }

        log::info!("Connecting to Wi-Fi with WIFI_SSID `{SSID}` and WIFI_PASS `{PASSWORD}`");
        //log::info!("Connecting to Wi-Fi with WIFI_SSID `{SSID}`");
        wifi.connect()?;
        log::info!("Wifi connected");
        */
        log::info!("Create Wi-Fi with WIFI_SSID `{SSID}` and WIFI_PASS `{PASSWORD}`");

        wifi.wait_netif_up()?;
        log::info!("Wifi netif up");

        //let ip_info = wifi.wifi().sta_netif().get_ip_info()?;
        let ip_info = wifi.wifi().ap_netif().get_ip_info()?;
        log::info!("Running on IP {}", ip_info.ip);

        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_it() {
        assert_eq!(2 + 2, 4);
    }
}