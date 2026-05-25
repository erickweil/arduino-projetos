//! Baseado em https://github.com/esp-rs/esp-idf-svc/blob/master/examples/mqtt_client.rs
//! Note: On ESP-IDF v6.0+, the MQTT component was moved out of the main tree. To enable it,
//! add the following to your `Cargo.toml`:
//! ```toml
//! [[package.metadata.esp-idf-sys.extra_components]]
//! remote_component = { name = "espressif/mqtt", version = "1.*" }
//! ```
//! 
use espidf_std::prelude::*;
use std::{time::Duration};

/// Configurações de Wi-Fi
/// O intervalo permitido é de [8, 84], o que corresponde a uma potência real de 2 dBm a 20 dBm.
/// A unidade do parâmetro de potência (power) é de 0.25 dBm.
/// 34 * 0.25 dBm = 8.5 dBm
/// [No esp32c3 precisa disso ou não funciona o wifi] 
const MAX_RADIO_POWER: Option<i8> = None; // Some(34);

const SSID: &str = match option_env!("WIFI_SSID") {
    Some(ssid) => ssid,
    None => "Wokwi-GUEST",
};
const PASSWORD: &str = match option_env!("WIFI_PASS") {
    Some(pass) => pass,
    None => "",
};

const MQTT_URL: &str = match option_env!("MQTT_URL") {
    Some(url) => url,
    None => "mqtt://host.wokwi.internal:1883",
};
const MQTT_CLIENT_ID: &str = "esp-mqtt-demo";
const MQTT_TOPIC: &str = "esp-mqtt-demo";

espidf_only! {
    use esp_idf_svc::sys::esp_wifi_set_max_tx_power;
    use esp_idf_svc::eventloop::EspSystemEventLoop;
    use esp_idf_svc::hal::peripherals::Peripherals;
    use esp_idf_svc::mqtt::client::*;
    use esp_idf_svc::nvs::EspDefaultNvsPartition;
    use esp_idf_svc::wifi::*;

    pub fn main() -> Result<()> {
        esp_idf_svc::sys::link_patches();
        esp_idf_svc::log::EspLogger::initialize_default();
        
        let peripherals = Peripherals::take()?;
        let sys_loop = EspSystemEventLoop::take()?;
        let nvs = EspDefaultNvsPartition::take()?;

        let mut esp_wifi = EspWifi::new(peripherals.modem, sys_loop.clone(), Some(nvs.clone()))?;
        let mut wifi = BlockingWifi::wrap(&mut esp_wifi, sys_loop.clone())?;
        wifi_config(&mut wifi)?;

        let mut client = mqtt_create(MQTT_URL, MQTT_CLIENT_ID)?;
        let mut counter = 0;

        log::info!("Starting MQTT publish on topic \"{MQTT_TOPIC}\"...");
        loop {
            let payload = format!("Hello MQTT! Counter: {counter}");
            
            client.enqueue(MQTT_TOPIC, QoS::AtMostOnce, false, payload.as_bytes())?;
            log::info!("Published message {counter}");
            
            counter += 1;

            std::thread::sleep(Duration::from_secs(2));
        }
    }

    fn mqtt_create(url: &str, client_id: &str) -> Result<EspMqttClient<'static>> {
        // Com new_cb, os eventos são processados no callback — sem necessidade de thread separada.
        let mqtt_client = EspMqttClient::new_cb(
            url,
            &MqttClientConfiguration {
                client_id: Some(client_id),
                ..Default::default()
            },
            |event| {
                log::info!("MQTT Event: {}", event.payload());
            },
        )?;

        Ok(mqtt_client)
    }

    fn wifi_config(
        wifi: &mut BlockingWifi<&mut EspWifi>,
    ) -> Result<()> {
        let auth_method = if PASSWORD.is_empty() {
            AuthMethod::None
        } else {
            AuthMethod::WPAWPA2Personal
        };

        wifi.set_configuration(&Configuration::Client(ClientConfiguration {
            ssid: SSID.try_into()?,
            password: PASSWORD.try_into()?,
            auth_method,
            channel: None,
            pmf_cfg: esp_idf_svc::wifi::PmfConfiguration::Capable { required: false },
            ..Default::default()
        }))?;

        wifi.start()?;
        log::info!("Wifi started");

        // --- Redução da Potência de Transmissão (Hardware Brownout Fix) ---
        // O problema: o rádio RF liga e puxa um pico de corrente repentino que pode passar de 300mA.
        // Reduzir a potência de transmissão (ironicamente) ajuda a estabilizar o sinal nessas placas, pois diminui o ruído no circuito regulador de tensão interno.
        // SAFETY: é só um binding do código C
        if let Some(power) = MAX_RADIO_POWER {
            unsafe {
                let err = esp_wifi_set_max_tx_power(power);
                if err != 0 {
                    log::warn!("Aviso: Falha ao ajustar TX power. Codigo do erro: {}", err);
                }
            }
        }

        wifi.connect()?;
        log::info!("Wifi connected");

        wifi.wait_netif_up()?;
        log::info!("Wifi netif up");

        let ip_info = wifi.wifi().sta_netif().get_ip_info()?;
        log::info!("Running on IP {}", ip_info.ip);
        
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test_log::test]
    fn test_it() {
        assert_eq!(2 + 2, 4);
    }
}