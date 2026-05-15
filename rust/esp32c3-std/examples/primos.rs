use std::{thread, time::Duration};
use esp_idf_svc::hal::{gpio::PinDriver, peripherals::Peripherals};

// Importa a função de verificação de números primos
use esp32c3_std::primos::eh_primo;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    esp_idf_svc::sys::link_patches();
    esp_idf_svc::log::EspLogger::initialize_default();

    // Configura o pino do LED (GPIO8) como saída
    let peripherals = Peripherals::take()?;
    let mut led_builtin = PinDriver::output(peripherals.pins.gpio8)?;
    led_builtin.set_low()?;

    log::info!("Iniciando...");

    // sim, 10 bilhões é o ponto de partida!
    let mut numero: u64 = 10_000_000_000;
    loop {
        if eh_primo(numero) {
            log::info!("{}", numero);
            
            // Piscar led
            led_builtin.toggle()?;

            // Descansar né, que o cara não é de ferro (ou é?)
            thread::sleep(Duration::from_millis(10));
        }
        numero += 1;
    }
}
