use std::{thread, time::Duration};

// A função main() será executada quando a placa ligar ou for resetada. Ela é o ponto de entrada do programa.
fn main() {
    // It is necessary to call this function once. Otherwise, some patches to the runtime
    // implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71
    esp_idf_svc::sys::link_patches();

    // Bind the log crate to the ESP Logging facilities
    esp_idf_svc::log::EspLogger::initialize_default();
    
    // Aqui define variáveis necessárias e inicializa pinos, etc...
    log::info!("Inicializando...");

    loop {
        // Executa em loop e imprime a mensagem a cada segundo
        log::info!("Olá mundo, ESP-IDF!!!");

        // Aguarda 1 segundo antes de imprimir a mensagem novamente
        // Veja que estamos usando a função sleep() da biblioteca padrão do Rust (std)
        thread::sleep(Duration::from_millis(1000));
    }
}