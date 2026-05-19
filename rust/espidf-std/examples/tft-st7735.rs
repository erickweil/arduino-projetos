#![allow(unused_imports)]
/// Exemplo: Display TFT ST7735s (0.96", 160x80) onboard do Heltec Wireless Tracker
/// Baseado no exemplo:
/// https://github.com/esp-rs/esp-idf-hal/blob/master/examples/spi_st7789.rs
/// 
/// Código com pinagem para placa Heltec Wireless Tracker (ESP32-S3FN8, display TFT ST7735s 160×80):
/// https://heltec.org/project/wireless-tracker/
///
/// Código da biblioteca ST7735 do Heltec Wireless Tracker (HT_st7735.h): 
/// (Para offsets e configuração específica do display 160×80 do HT Tracker)
/// https://github.com/HelTecAutomation/Heltec_ESP32/blob/master/src/HT_st7735.h
///
/// Outros recursos:
/// - https://esp32.implrust.com/tft-display/index.html
/// - https://wokwi.com/projects/393180528527977473
///
use espidf_std::prelude::*;

use std::{thread, time::Duration};
use mipidsi::{
    Builder,
    interface::SpiInterface,
    models::ST7735s,
    options::{ColorInversion, ColorOrder, Orientation, Rotation},
    TestImage,
};  
use embedded_graphics::{
    mono_font::{MonoTextStyle, ascii::FONT_10X20}, pixelcolor::Rgb565, prelude::*, primitives::Rectangle, text::Text
};

struct DrawBouncingText {
    text: &'static str,
    screen_size: Size,
    position: Point,
    velocity: Point,
}

impl DrawBouncingText {
    fn new(text: &'static str, screen_size: Size, position: Point, velocity: Point) -> Self {
        Self { text, screen_size, position, velocity }
    }

    pub fn draw<D>(&mut self, target: &mut D) -> Result<()>
    where
        D: DrawTarget<Color = Rgb565>,
        D::Error: core::fmt::Debug,
    {
        // Dimensões do retângulo com texto (FONT_10X20: 10px/char, 20px altura)
        let text_width = (self.text.len() as i32 * 10) + 10; // largura do texto + padding
        let text_height = 28; // altura do texto + padding

        // Verifica colisão e muda direção ao bater nos cantos/bordas
        if self.position.x <= 0 || self.position.x + text_width >= self.screen_size.width as i32 {
            self.velocity.x = -self.velocity.x;
        }
        if self.position.y <= 0 || self.position.y + text_height >= self.screen_size.height as i32 {
            self.velocity.y = -self.velocity.y;
        }

        // Move posição
        self.position += self.velocity;

        // Garante que não saia da tela
        self.position.x = self.position.x.clamp(0, self.screen_size.width as i32 - text_width);
        self.position.y = self.position.y.clamp(0, self.screen_size.height as i32 - text_height);

        // Limpa o quadrado onde o texto será desenhado
        let clear_area = Rectangle::new(
            self.position,
            Size::new(text_width as u32, text_height as u32)
        );
        target.fill_solid(&clear_area, Rgb565::BLACK)
            .map_err(|e| format!("Erro fill_solid! {:?}", e))?;

        // Cor pseudo aleatória baseada na posição (para dar um efeito visual mais interessante)
        let color = Rgb565::new(
            ((self.position.x * 71) % 32) as u8, // R
            ((self.position.y * 83) % 64) as u8, // G
            ((self.position.x * 31 + self.position.y * 23) % 32) as u8, // B
        );

        Text::new(
            self.text,
            Point::new(self.position.x + 5, self.position.y + 20), // posição do texto dentro da caixa
            MonoTextStyle::new(&FONT_10X20, color)
        )
        .draw(target)
        .map_err(|e| format!("Erro draw! {:?}", e))?;

        Ok(())
    }
}

target_only! { "esp32s3",

    use esp_idf_svc::hal::gpio;
    use esp_idf_svc::hal::{
        self,
        gpio::PinDriver,
        peripherals::Peripherals,
        spi::{self},
        units::MegaHertz,
    };  
    
    
    pub fn main() -> Result<()> {
        esp_idf_svc::sys::link_patches();
        esp_idf_svc::log::EspLogger::initialize_default();

        log::info!("Iniciando display TFT ST7735...");

        let peripherals = Peripherals::take()?;

        // Configuração de Pinos do display para o Heltec Wireless Tracker
        let spi = peripherals.spi2;
        let rst = PinDriver::output(peripherals.pins.gpio39)?;
        let dc = PinDriver::output(peripherals.pins.gpio40)?;
        let mut backlight = PinDriver::output(peripherals.pins.gpio21)?;
        let sclk = peripherals.pins.gpio41;
        let sda = peripherals.pins.gpio42;
        let sdi = None::<gpio::AnyInputPin>;
        let cs = Some(peripherals.pins.gpio38);
        let mut vext = PinDriver::output(peripherals.pins.gpio3)?; // Vext Ctrl: HIGH para energizar display e GNSS onboard
        
        // Habilita Vext para alimentar o display e módulo GNSS onboard
        vext.set_high()?;
        thread::sleep(Duration::from_millis(50));
        // Liga o backlight
        backlight.set_high()?;
        thread::sleep(Duration::from_millis(50));

        let spi_device = spi::SpiDeviceDriver::new_single(
            spi,
            sclk,
            sda,
            sdi,
            cs,
            &spi::SpiDriverConfig::new(),
            &spi::SpiConfig::new()
                .baudrate(MegaHertz(26).into()),
                //.data_mode(MODE_3), // note that in order for the ST7789 to work, the data_mode needs to be set to MODE_3
        )?;

        // display interface abstraction from SPI and DC
        let mut buffer = [0u8; 512];
        let di = SpiInterface::new(
            spi_device, 
            dc, 
            &mut buffer
        );

        // crate driver
        let mut display = Builder::new(ST7735s, di)
            // Heltec Wireless Tracker
            .display_size(80, 160)
            .display_offset(26, 1)
            .color_order(ColorOrder::Bgr)
            .invert_colors(ColorInversion::Inverted)
            .orientation(Orientation::new().rotate(Rotation::Deg270))
            .reset_pin(rst)
            .init(&mut hal::delay::Ets)
            .map_err(|e| format!("Erro ao inicializar display: {e:?}"))?;

        log::info!("Display inicializado com sucesso!");

        log::info!("Display info: {}x{}, bounding box: {:?}",
            display.size().width, display.size().height,
            display.bounding_box()
        );

        TestImage::new().draw(&mut display)
            .map_err(|e| format!("Erro ao desenhar imagem de teste: {e:?}"))?;

        thread::sleep(Duration::from_millis(5000));

        display.clear(Rgb565::BLACK)
            .map_err(|e| format!("Erro clear: {e:?}"))?;

        let mut bouncing_text = DrawBouncingText::new(
            "TESTE",
            display.size(), 
            Point::new(0, 0), 
            Point::new(2, 1)
        );

        loop {
            bouncing_text.draw(&mut display)?;
            thread::sleep(Duration::from_millis(100));
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use embedded_graphics::mock_display::MockDisplay;

    #[test]
    fn test_draw() {
        let mut display = MockDisplay::new();
        display.set_allow_overdraw(true);
        // Testa a lógica de movimento e colisão do texto
        let mut bouncing_text = DrawBouncingText::new(
            "OK",
            display.size(), 
            Point::new(0, 0), 
            Point::new(5, 3)
        );

        // Simula várias atualizações para verificar se o texto se move e colide corretamente
        // Daria erro se tentar desenhar fora dos limites
        for _ in 0..500 {
            bouncing_text.draw(&mut display).unwrap();
        }
    }
}
