// Exemplo: Display TFT ST7735s (0.96", 160x80) onboard do Heltec Wireless Tracker
//
// Os pinos são mapeados via WirelessTrackerPins (src/lib.rs).
// Compile com: cargo build --example tft-st7735 --features esp32s3

use espidf_std::prelude::*;
use mipidsi::{TestImage, options::{Orientation, Rotation}};

espidf_only! {
    use esp_idf_svc::hal::gpio;
    use espidf_std::wireless_tracker::WirelessTrackerPins;
    use embedded_graphics::{
        mono_font::{ascii::FONT_10X20, MonoTextStyle},
        pixelcolor::Rgb565,
        prelude::*,
        primitives::{Rectangle, PrimitiveStyleBuilder},
        text::Text,
    };
    use esp_idf_svc::hal::{
        self,
        gpio::PinDriver,
        peripherals::Peripherals,
        //spi::{Spi, SpiDeviceDriver, SpiDriver, SpiDriverConfig, config::Config as SpiConfig},
        spi::{self},
        units::MegaHertz,
    };
    use mipidsi::{
        Builder,
        interface::SpiInterface,
        models::ST7735s,
        options::{ColorInversion, ColorOrder, Orientation},
    };
    use std::{thread, time::Duration};

    pub fn main() -> Result<()> {
        esp_idf_svc::sys::link_patches();
        esp_idf_svc::log::EspLogger::initialize_default();

        log::info!("Iniciando display TFT ST7735s...");

        let peripherals = Peripherals::take()?;
        let board = WirelessTrackerPins::new(peripherals.pins);

        // Vext Ctrl: HIGH para energizar display e GNSS onboard
        let mut vext = PinDriver::output(board.vext_ctrl)?;
        vext.set_high()?;
        thread::sleep(Duration::from_millis(50));

        // Backlight: HIGH para acender o display
        let mut backlight = PinDriver::output(board.tft_bl)?;
        backlight.set_high()?;

        let dc  = PinDriver::output(board.tft_dc)?;
        let rst = PinDriver::output(board.tft_rst)?;

        // SPI2: MOSI, SCK, sem MISO
        let spi_driver = spi::SpiDriver::new(
            peripherals.spi2,
            board.tft_sclk,
            board.tft_mosi,
            None::<gpio::AnyInputPin>,
            &spi::SpiDriverConfig::new(),
            //&spi::SpiDriverConfig::default().dma(spi::Dma::Auto(4096)),
        )?;

        let spi_device = spi::SpiDeviceDriver::new(
            &spi_driver,
            Some(board.tft_cs),
            &spi::SpiConfig::default()
                .baudrate(MegaHertz(27).into()),
        )?;

        // Buffer intermediário exigido pelo SpiInterface do mipidsi
        let mut buffer = [0u8; 512];
        let spi_interface = SpiInterface::new(spi_device, dc, &mut buffer);

        // Display ST7735s — configuração para o chip Wokwi (chip-st7735, 128×160)
        // O chip Wokwi é um ST7735 padrão 128×160, sem offset, RGB565.
        // Para hardware real (Heltec Wireless Tracker 160×80):
        //   .display_size(160, 80).display_offset(1, 26).color_order(ColorOrder::Bgr)
        let mut display = Builder::new(ST7735s, spi_interface)
            // Wokwi
            //.display_size(128, 160)
            //.display_offset(0, 0)
            //.color_order(ColorOrder::Rgb)
            //.invert_colors(ColorInversion::Normal)
            // Heltec Wireless Tracker
            .display_size(80, 160)
            .display_offset(26, 1)
            .color_order(ColorOrder::Bgr)
            .invert_colors(ColorInversion::Inverted)
            // MY|MV do HT_st7735.h = landscape com flip vertical
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

        // --- DVD bouncing "Ola mundo!" ---
        // Dimensões do display em landscape
        let SCREEN_W: i32 = display.size().width as i32;
        let SCREEN_H: i32 = display.size().height as i32;
        // Dimensões do retângulo com texto (FONT_10X20: 10px/char, 20px altura)
        // "Ola mundo!" = 10 chars * 10px = 100px largura + padding
        const BOX_W: i32 = 110;
        const BOX_H: i32 = 28;
        const PAD_X: i32 = 5;  // padding interno do texto dentro da caixa
        const PAD_Y: i32 = 20; // baseline do texto dentro da caixa (FONT_10X20)

        let mut x: i32 = 0;
        let mut y: i32 = 0;
        let mut dx: i32 = 2;
        let mut dy: i32 = 1;

        let mut color = Rgb565::RED;
        loop {
            // Move posição
            x += dx;
            y += dy;

            // Verifica colisão e muda cor ao bater nos cantos/bordas
            let hit_x = x <= 0 || x + BOX_W >= SCREEN_W;
            let hit_y = y <= 0 || y + BOX_H >= SCREEN_H;

            if hit_x { dx = -dx; }
            if hit_y { dy = -dy; }
            if hit_x || hit_y {
                // Cor pseudo aleatória
                color = Rgb565::new(
                    ((x * 13) % 255) as u8,
                    ((y * 17) % 255) as u8,
                    (((x + y) * 23) % 255) as u8
                );
            }

            // Garante que não saia da tela
            x = x.clamp(0, SCREEN_W - BOX_W);
            y = y.clamp(0, SCREEN_H - BOX_H);

            Text::new(
                "Ola mundo!", 
                Point::new(x + PAD_X, y + PAD_Y), 
                MonoTextStyle::new(&FONT_10X20, color)
            )
            .draw(&mut display)
            .map_err(|e| format!("Erro draw text: {e:?}"))?;

            thread::sleep(Duration::from_millis(30));
        }
    }
}