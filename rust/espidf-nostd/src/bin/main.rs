#![no_std]
#![no_main]
#[deny(clippy::mem_forget)]

use esp_hal::clock::CpuClock;
use esp_hal::main;
use esp_hal::gpio::{Level, Output, OutputConfig};
use esp_hal::time::{Duration, Instant};
use esp_println::print;

// This creates a default app-descriptor required by the esp-idf bootloader.
// For more information see: <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/app_image_format.html#application-description>
esp_bootloader_esp_idf::esp_app_desc!();

// You need a panic handler. Usually, you you would use esp_backtrace, panic-probe, or
// something similar, but you can also bring your own like this:
#[panic_handler]
fn panic(_: &core::panic::PanicInfo) -> ! {
    esp_hal::system::software_reset()
}

#[main]
fn main() -> ! {
    // SETUP Inicial, pinos e etc...
    let config = esp_hal::Config::default().with_cpu_clock(CpuClock::max());
    let peripherals = esp_hal::init(config);

    // ESP32C3
    // - GPIO8 Led builtin
    // - GPIO11-17 (should not be used)
    let mut led_builtin = Output::new(peripherals.GPIO8, Level::Low, OutputConfig::default());
    let mut counter = 0;

    print!("Olá mundo!!!\r\n");
    loop {
        // LOOP Principal
        print!("Contador: {counter}\r\n");
        counter += 1;

        led_builtin.toggle();

        // Dormir por um tempo (1 segundo)
        let delay_start = Instant::now();
        while delay_start.elapsed() < Duration::from_millis(1000) {}
    }
}
