//! LittleFS Hello World - ESP32-S3 (8MB Flash)
//!
//! Demonstra uso básico do LittleFS na flash interna
//!
//! To use, put this in your `Cargo.toml`:
//! ```
//! [[package.metadata.esp-idf-sys.extra_components]]
//! remote_component = { name = "joltwallet/littlefs", version = "1.14" }
//! ```
//! Também no sdkconfig.defaults habilite o LittleFS:
//! ```
//! # LittleFS suporte na flash interna (sem SD card)
//! CONFIG_LITTLEFS_FOR_IDF_3_2_OR_LATER=y
//! ```
//! 
//! E certifique-se de ter uma partição "spiffs" definida no `partitions.csv` do projeto
//! (Sim, mesmo usando LittleFS, a partição ainda é chamada "spiffs" por convenção)

use std::fs::{File};
use std::io::{Read, Write};

use espidf_std::prelude::*;

const MOUNT_POINT: &str = "/lfs";
const TEST_FILE: &str = "/lfs/hello.txt";

espidf_only! {
    use esp_idf_svc::fs::littlefs::Littlefs;
    use esp_idf_svc::io::vfs::MountedLittlefs;
    use esp_idf_svc::hal::{peripherals::Peripherals};

    pub fn main() -> Result<()> {
        esp_idf_svc::sys::link_patches();
        esp_idf_svc::log::EspLogger::initialize_default();

        let _peripherals = Peripherals::take()?;

        // Cria instância do LittleFS apontando para a partição "spiffs"
        // A partição deve estar definida no partitions.csv do projeto
        log::info!("Criando partição 'spiffs' para o LittleFS...");
        let littlefs: Littlefs<()> = unsafe {
            Littlefs::new_partition("spiffs")?
        };

        // Monta o filesystem no caminho /lfs (Formata se necessário)
        // O `_mounted` mantém o mount ativo enquanto está no escopo
        log::info!("Montando LittleFS em '{}'", MOUNT_POINT);
        let mounted = MountedLittlefs::mount(littlefs, MOUNT_POINT).unwrap_or_else(|e| {
            // TODO: Isso pode? será que chamar para criar denovo a partição é seguro? (não dá para usar o valor anterior pois foi movido)
            log::warn!("Erro ao montar LittleFS: {e:?}. Tentando formatar...");
            let mut littlefs: Littlefs<()> = unsafe {
                Littlefs::new_partition("spiffs").unwrap()
            };
            littlefs.format().unwrap();
            MountedLittlefs::mount(littlefs, MOUNT_POINT).unwrap()
        });

        // --- Informações do filesystem ---
        let info = mounted.info()?;
        log::info!("--- Informações do LittleFS ---");
        log::info!("  Tamanho total : {} bytes ({} KB)", info.total_bytes, info.total_bytes / 1024);
        log::info!("  Espaço usado  : {} bytes ({} KB)", info.used_bytes,  info.used_bytes  / 1024);
        log::info!(
            "  Espaço livre  : {} bytes ({} KB)",
            info.total_bytes - info.used_bytes,
            (info.total_bytes - info.used_bytes) / 1024
        );

        // --- Escrever arquivo ---
        log::info!("--- Escrevendo arquivo: {} ---", TEST_FILE);
        const TEST_CONTENT: &str = "Olá, LittleFS! Este é um teste de leitura e escrita.";
        {
            let mut f = File::create(TEST_FILE)?;
            f.write_all(TEST_CONTENT.as_bytes())?;
            log::info!("  Conteúdo gravado: {:?}", TEST_CONTENT);
        }

        // --- Ler arquivo ---
        log::info!("--- Lendo arquivo: {} ---", TEST_FILE);
        {
            let mut f = File::open(TEST_FILE)?;
            let mut contents = String::new();
            f.read_to_string(&mut contents)?;
            log::info!("  Conteúdo lido   : {:?}", contents);

            if contents == TEST_CONTENT {
                log::info!("  ✓ Conteúdo confere!");
            } else {
                log::error!("  ✗ Conteúdo NÃO confere!");
            }
        }

        // `mounted` é descartado aqui → LittleFS desmontado automaticamente
        Ok(())
    }
}
