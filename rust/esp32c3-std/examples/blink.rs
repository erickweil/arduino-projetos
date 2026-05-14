use std::{thread, time::Duration};

use esp_idf_svc::hal::{gpio::{PinDriver}, peripherals::Peripherals};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    // It is necessary to call this function once. Otherwise, some patches to the runtime
    // implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71
    esp_idf_svc::sys::link_patches();

    // Bind the log crate to the ESP Logging facilities
    esp_idf_svc::log::EspLogger::initialize_default();
    log::info!("Olá, mundo!");

    // Take the peripherals singleton
    let peripherals = Peripherals::take()?;
    // let sysloop = EspSystemEventLoop::take()?;

    let mut led_builtin = PinDriver::output(peripherals.pins.gpio8)?;
    led_builtin.set_low()?;

    // The main loop of the application
    let mut counter = 0;
    loop {
        // piscar led
        led_builtin.toggle()?;

        log::info!("Contando... {}", counter);
        counter += 1;

        thread::sleep(Duration::from_millis(500));
    }
}
