use espidf_std::prelude::*;
use std::{thread, time::Duration};
use nmea::Nmea;

/// Limite máximo do buffer acumulado. Se ultrapassado, os dados são descartados
/// Uma linha GPS típica tem cerca de 80 caracteres, então 512 é um buffer razoável para acumular várias linhas antes de processar.
const MAX_BUFFER: usize = 512;

struct LineByLineIterator {
    /// Buffer de leitura
    buf: Vec<u8>,
    /// Quantos bytes no 'buf' são válidos
    fill: usize,
}

impl LineByLineIterator {
    pub fn new() -> Self {
        Self { 
            buf: vec![0; MAX_BUFFER],
            fill: 0 
        }
    }

    /// Chama `reader` passando o espaço livre do buffer e incorpora os bytes escritos.
    pub fn fill_from(&mut self, reader: impl FnOnce(&mut [u8]) -> Result<usize>) -> Result<usize> {
        let n = reader(&mut self.buf[self.fill..])?;
        self.fill += n;
        if self.fill >= MAX_BUFFER {
            self.fill = 0;
            return Err("LineByLineIterator Buffer overflow: line too long".into());
        }
        Ok(n)
    }

    /// Tenta extrair a próxima linha completa do buffer. 
    /// Retorna None se não houver uma linha completa (terminada em '\n').
    pub fn next_line(&mut self) -> Option<String> {
        let pos = self.buf[..self.fill].iter().position(|&b| b == b'\n')?;
        let line = String::from_utf8_lossy(&self.buf[..pos])
            .trim_end_matches('\r')
            .trim()
            .to_string();
        // Desloca os bytes restantes para o início (sem realocar).
        self.buf.copy_within(pos + 1..self.fill, 0);
        self.fill -= pos + 1;
        if line.is_empty() { None } else { Some(line) }
    }

    /// Retorna um iterador que extrai linhas completas do buffer até que não haja mais.
    /// TODO: fazer sem alocar String
    pub fn drain_lines(&mut self) -> impl Iterator<Item = String> + '_ {
        std::iter::from_fn(|| self.next_line())
    }
}

espidf_only! {
    use esp_idf_svc::sys as sys;
    use esp_idf_svc::hal::{gpio::{AnyIOPin, PinDriver}, peripherals::Peripherals, uart::{UartConfig, UartDriver}, units::Hertz};

    // A função main() será executada quando a placa ligar ou for resetada. Ela é o ponto de entrada do programa.
    pub fn main() -> Result<()> {
        esp_idf_svc::sys::link_patches();
        esp_idf_svc::log::EspLogger::initialize_default();
        let peripherals = Peripherals::take()?;

        #[cfg(feature = "esp32s3")]
        let mut vext = PinDriver::output(peripherals.pins.gpio3)?; // Vext Ctrl: HIGH para energizar display e GNSS onboard

        #[cfg(feature = "esp32s3")]
        {                
            // Habilita Vext para alimentar o display e módulo GNSS onboard
            vext.set_high()?;
            thread::sleep(Duration::from_millis(500));
            log::info!("Vext habilitado para alimentar o display e módulo GNSS onboard");
        }

        // Default read from Serial port (UART0) for ESP32
        #[cfg(not(feature = "esp32s3"))]
        let gps_uart = UartDriver::new(
            peripherals.uart0,
            peripherals.pins.gpio1, // UART0 TX
            peripherals.pins.gpio3, // UART0 RX
            Option::<AnyIOPin>::None,
            Option::<AnyIOPin>::None,
            &Default::default(),
        )?;

        // Serial1.begin(115200, SERIAL_8N1, 33, 34);
        #[cfg(feature = "esp32s3")]
        let gps_uart = UartDriver::new(
            peripherals.uart1,
            peripherals.pins.gpio34, // UART1 TX
            peripherals.pins.gpio33, // UART1 RX
            Option::<AnyIOPin>::None,
            Option::<AnyIOPin>::None,
            &UartConfig::new()
                .baudrate(Hertz(115_200))
        )?;

        // Criar na HEAP, isso consome memória demais!
        // https://github.com/AeroRust/nmea/issues/2
        // TODO: pesquisar outra biblioteca mais leve? o TinyGPS++ fazia como? 
        let mut nmea_parser = Box::new(Nmea::default());
        // 12304 bytes
        // log::info!("NMEA parser initialized, struct size in bytes: {:?}", std::mem::size_of_val(&*nmea_parser));

        let mut line_iterator = LineByLineIterator::new();

        // Lê linha por linha do GPS
        log::info!("INICIALIZADO!");
        let mut changed = false;
        loop {
            loop {
                let bytes_read = line_iterator.fill_from(|buf| {
                    // 5 FreeRTOS ticks ~ 50 ms
                    gps_uart.read(buf, 5).or_else(|e| {
                        if e.code() == sys::ESP_ERR_TIMEOUT {
                            Result::Ok(0)
                        } else {
                            Result::Err(Box::new(e))
                        }
                    })
                }).or_else(|e| {
                    log::error!("Failed to read from GPS UART: {}", e);
                    Result::Ok(0)
                })?;
                
                if bytes_read == 0 {
                    break; // Sem mais dados disponíveis no momento
                }

                log::info!("Read {} bytes from GPS", bytes_read);
                for line in line_iterator.drain_lines() {
                    log::info!("GPS '{}'", line);

                    // Tenta parsear a linha como uma sentença NMEA
                    nmea_parser.parse(&line).ok().map(|_| changed = true);
                }
            }

            if changed {
                changed = false;
                log::info!("FIX: {:?}", nmea_parser.fix_type());
                log::info!("LATITUDE: {:?}", nmea_parser.latitude());
                log::info!("LONGITUDE: {:?}", nmea_parser.longitude());
                log::info!("ALTITUDE: {:?}", nmea_parser.altitude());
                log::info!("TIMESTAMP: {:?}", nmea_parser.fix_timestamp());
                log::info!("SATELLITES IN VIEW: {:?}", nmea_parser.fix_satellites());
            }

            thread::sleep(Duration::from_millis(1));
        }
    }
}

#[cfg(test)]
mod tests {
    use nmea::sentences::FixType;
    use super::*;

    // https://swairlearn.bluecover.pt/nmea_analyser
    const GPS_STREAM: &str = "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n\
$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n\
$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n\
$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,1.2,200.8,M,-22.5,M,,0000*6C\r\n\
$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n\
$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";

    #[test]
    fn test_nmea_parsing_byte_by_byte() {
        let mut nmea_parser = Nmea::default();
        
        let mut line_iterator = LineByLineIterator::new();
        let mut lines_read = 0;

        // Alimenta byte por byte no iterador para simular a leitura do GPS
        for byte in GPS_STREAM.as_bytes() {
            line_iterator.fill_from(|buf| {
                buf[0] = *byte;
                Ok(1)
            }).expect("Failed to fill LineByLineIterator");

            // Tenta extrair linhas completas a cada byte alimentado
            for line in line_iterator.drain_lines() {
                println!("GPS '{}'", line);
                lines_read += 1;

                match nmea_parser.parse(&line) {
                    Ok(sentence) => {
                        println!("Parsed NMEA sentence: {:?}", sentence);
                    },
                    Err(e) => println!("Failed to parse NMEA sentence: {}", e),
                }
            }
        }
        
        assert_eq!(lines_read, 6);
        assert_eq!(nmea_parser.fix_type(), Some(FixType::Gps));

        println!("Latitude: {:?}", nmea_parser.latitude());
        println!("Longitude: {:?}", nmea_parser.longitude());
        println!("Altitude: {:?}", nmea_parser.altitude());
        println!("Timestamp: {:?}", nmea_parser.fix_timestamp());
        println!("Satellites in view: {:?}", nmea_parser.fix_satellites());
    }

    #[test]
    fn test_line_overflow() {
        let mut line_iterator = LineByLineIterator::new();
        let long_line = "A".repeat(MAX_BUFFER + 10) + "\n";

        let result = line_iterator.fill_from(|buf| {
            let bytes = long_line.as_bytes();
            let to_write = bytes.len().min(buf.len());
            buf[..to_write].copy_from_slice(&bytes[..to_write]);
            Ok(to_write)
        });

        assert!(result.is_err());
    }
}