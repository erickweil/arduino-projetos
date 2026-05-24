/// Baseado em https://github.com/esp-rs/esp-idf-svc/blob/master/examples/mqtt_client.rs
use espidf_std::prelude::*;
use std::{time::Duration};

const SSID: &str = match option_env!("WIFI_SSID") {
    Some(ssid) => ssid,
    None => "Wokwi-GUEST",
};
const PASSWORD: &str = match option_env!("WIFI_PASS") {
    Some(pass) => pass,
    None => "",
};

const MQTT_URL: &str = "mqtt://host.wokwi.internal:1883";
const MQTT_CLIENT_ID: &str = "esp-mqtt-demo";
const MQTT_TOPIC: &str = "esp-mqtt-demo";

espidf_only! {
    
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

        let (mut client, mut conn) = mqtt_create(MQTT_URL, MQTT_CLIENT_ID)?;

        run(&mut client, &mut conn, MQTT_TOPIC)?;

        Ok(())
    }

    fn run(
        client: &mut EspMqttClient<'_>,
        connection: &mut EspMqttConnection,
        topic: &str,
    ) -> Result<()> {
        std::thread::scope(|s| {
            log::info!("About to start the MQTT client");

            // Need to immediately start pumping the connection for messages, or else subscribe() and publish() below will not work
            // Note that when using the alternative constructor - `EspMqttClient::new_cb` - you don't need to
            // spawn a new thread, as the messages will be pumped with a backpressure into the callback you provide.
            // Yet, you still need to efficiently process each message in the callback without blocking for too long.
            //
            // Note also that if you go to http://tools.emqx.io/ and then connect and send a message to topic
            // "esp-mqtt-demo", the client configured here should receive it.
            std::thread::Builder::new()
                .stack_size(6000)
                .spawn_scoped(s, move || {
                    log::info!("MQTT Listening for messages");

                    while let Ok(event) = connection.next() {
                        log::info!("[Queue] Event: {}", event.payload());
                    }

                    log::info!("Connection closed");
                })
                .unwrap();

            loop {
                if let Err(e) = client.subscribe(topic, QoS::AtMostOnce) {
                    log::error!("Failed to subscribe to topic \"{topic}\": {e}, retrying...");

                    // Re-try in 0.5s
                    std::thread::sleep(Duration::from_millis(500));

                    continue;
                }

                log::info!("Subscribed to topic \"{topic}\"");

                // Just to give a chance of our connection to get even the first published message
                std::thread::sleep(Duration::from_millis(500));

                let payload = "Hello from esp-mqtt-demo!";

                loop {
                    client.enqueue(topic, QoS::AtMostOnce, false, payload.as_bytes())?;

                    log::info!("Published \"{payload}\" to topic \"{topic}\"");

                    let sleep_secs = 2;

                    log::info!("Now sleeping for {sleep_secs}s...");
                    std::thread::sleep(Duration::from_secs(sleep_secs));
                }
            }
        })
    }

    fn mqtt_create(
        url: &str,
        client_id: &str,
    ) -> Result<(EspMqttClient<'static>, EspMqttConnection)> {
        let (mqtt_client, mqtt_conn) = EspMqttClient::new(
            url,
            &MqttClientConfiguration {
                client_id: Some(client_id),
                ..Default::default()
            },
        )?;

        Ok((mqtt_client, mqtt_conn))
    }

    fn wifi_config(
        wifi: &mut BlockingWifi<&mut EspWifi>,
    ) -> Result<()> {        
        wifi.set_configuration(&Configuration::Client(ClientConfiguration {
            ssid: SSID.try_into().unwrap(),
            password: PASSWORD.try_into().unwrap(),
            auth_method: AuthMethod::None,
            ..Default::default()
        }))?;

        wifi.start()?;
        log::info!("Wifi started");

        wifi.connect()?;
        log::info!("Wifi connected");

        wifi.wait_netif_up()?;
        log::info!("Wifi netif up");

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