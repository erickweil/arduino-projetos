use espidf_std::prelude::*;
use std::{thread, time::Duration};

espidf_only! {
    use esp_idf_svc::hal::{gpio::PinDriver, peripherals::Peripherals};
    
    pub fn main() -> Result<()> {
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
}

/// Função que verifica se um número é primo
pub fn eh_primo(n: u64) -> bool {
    if n == 2 {
        return true;
    }
    if n < 2 || n % 2 == 0 {   
        return false;
    }
    // Começa em 3 e vai indo nos ímpares até a raiz quadrada de n
    let mut i = 3u64;
    while i * i <= n {
        if n % i == 0 {
            return false;
        }
        i += 2;
    }
    true
}

// Teste para verificar se a função primo está funcionando corretamente
//#[cfg(all(test, not(feature = "espidf")))] // Usar se quiser desativar o teste no aparelho
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_eh_primo() {
        let primos = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47];
        for &n in &primos {
            assert!(eh_primo(n), "{} deveria ser primo", n);
        }

        let primos_falsos = [0, 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20];
        for &n in &primos_falsos {
            assert!(!eh_primo(n), "{} não deveria ser primo", n);
        }
    }
}
