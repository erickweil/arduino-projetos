// Importações e funções comuns a todos os ambientes (host e ESP-IDF)
use std::{thread, time::Duration};

fn delay(ms: u64) {
    thread::sleep(Duration::from_millis(ms));
}

// Podemos utilizar todos os recursos do Rust, como funções, closures, structs, traits, etc. 
// E eles podem ser testados normalmente no host, sem precisar rodar na placa.
fn criar_contador() -> impl FnMut() -> u64 {
    let mut count: u64 = 0;
    return move || {
        count += 1;
        count
    };
}

// Código que usa recursos do ESP-IDF deve ficar dentro do macro espidf_only!
// Inclusive imports e funções específicas
espidf_std::espidf_only! {
    use esp_idf_svc::hal::{gpio::PinDriver, peripherals::Peripherals};

    pub fn main() -> Result<(), Box<dyn std::error::Error>> {
        // It is necessary to call this function once. Otherwise, some patches to the runtime
        // implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71
        esp_idf_svc::sys::link_patches();

        // Bind the log crate to the ESP Logging facilities
        esp_idf_svc::log::EspLogger::initialize_default();
        log::info!("Olá mundo, ESP-IDF!!!");

        // Take the peripherals singleton
        let peripherals = Peripherals::take()?;
        // let sysloop = EspSystemEventLoop::take()?;

        // Configure the LED pin (GPIO8) as output
        // Board: ESP32-C3 Super Mini
        let mut led_builtin = PinDriver::output(peripherals.pins.gpio8)?;
        led_builtin.set_low()?;

        // The main loop of the application
        let mut contador = criar_contador();
        loop {
            log::info!("Contador: {}", contador());
            led_builtin.toggle()?;

            delay(1000);
        }
    }
}

// Aqui podemos escrever testes usando a biblioteca de teste do Rust, e eles podem ser executados no host, sem precisar de hardware.
// Note que eles podem testar as funções comuns, mas não podem testar código que dependa de recursos do ESP-IDF, ou seja, código dentro do macro espidf_only!
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_delay() {
        // Testa a função de delay (apenas para garantir que ela funcione, não é um teste de tempo)
        let start = std::time::Instant::now();
        delay(1);
        let elapsed = start.elapsed();
        assert!(elapsed >= std::time::Duration::from_millis(1));
    }

    #[test]
    fn test_criar_contador() {
        let mut contador = criar_contador();
        assert_eq!(contador(), 1);
        assert_eq!(contador(), 2);
        assert_eq!(contador(), 3);
    }
}