//! A simple example of how to read from the ESP-IDF console
//!
//! Usa `#[cfg(esp_idf_soc_usb_serial_jtag_supported)]` para detectar automaticamente
//! se o chip possui USB Serial/JTAG integrado (ex: ESP32-S3, ESP32-C3) e configura
//! o console adequadamente. Chips sem USB (ex: ESP32) usam UART0.
#![allow(unexpected_cfgs)]
#![allow(unused_imports)]

use esp_idf_svc::hal::usb_serial::{UsbSerialConfig, UsbSerialDriver};
use esp_idf_svc::hal::gpio::AnyIOPin;
use esp_idf_svc::hal::uart::UartDriver;
use esp_idf_svc::hal::peripherals::Peripherals;
use esp_idf_svc::io::vfs::BlockingStdIo;
use esp_idf_svc::sys::EspError;
use std::io::Write;


fn main() -> Result<(), EspError> {
    esp_idf_svc::sys::link_patches();
    esp_idf_svc::log::EspLogger::initialize_default();

    // Configuração padrão:
    // - UART0
    // - 115200 baud rate
    // - 8N1
    // - RTS/CTS disabled
    // - No flow control
    // - The default TX/RX pins for UART0 (1 & 3 for ESP32, 17 & 16 for ESP32-S2 and so on.)
    // - USB Serial/JTAG (quando disponível) é automaticamente detectado e configurado

    let peripherals = Peripherals::take()?;

    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-guides/usb-serial-jtag-console.html
    // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y no sdkconfig.defaults.
    #[cfg(esp_idf_soc_usb_serial_jtag_supported)]
    let _blocking_io = {
        let usb_serial = UsbSerialDriver::new(
            peripherals.usb_serial,
            #[cfg(feature = "esp32s3")]
            peripherals.pins.gpio19, // D-
            #[cfg(feature = "esp32s3")]
            peripherals.pins.gpio20, // D+

            #[cfg(not(feature = "esp32s3"))]
            peripherals.pins.gpio18, // D-
            #[cfg(not(feature = "esp32s3"))]
            peripherals.pins.gpio19, // D+
            &UsbSerialConfig::default(),
        )?;
        BlockingStdIo::usb_serial(usb_serial)?
    };

    #[cfg(not(esp_idf_soc_usb_serial_jtag_supported))]
    let _blocking_io = {
        let uart = UartDriver::new(
            peripherals.uart0,
            peripherals.pins.gpio1, // UART0 TX
            peripherals.pins.gpio3, // UART0 RX
            Option::<AnyIOPin>::None,
            Option::<AnyIOPin>::None,
            &Default::default(),
        )?;
        BlockingStdIo::uart(uart)?
    };

    loop {
        print!("> ");
        std::io::stdout().flush().unwrap();

        let mut buffer = String::new();
        std::io::stdin().read_line(&mut buffer).unwrap();

        println!("ECHO: {buffer}");
    }
}
