#![allow(unexpected_cfgs)]
//! LittleFS Hello World - ESP32-S3 (8MB Flash)
//!
//! Demonstra uso básico do LittleFS na flash interna
//! 
//! Baseado em:
//! - https://github.com/esp-rs/esp-idf-svc/blob/master/examples/sd_spi_littlefs.rs
//! - https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/examples/LITTLEFS_test/LITTLEFS_test.ino
//! - https://github.com/espressif/arduino-esp32/blob/master/libraries/LittleFS/src/LittleFS.cpp
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

use std::fs::{File, OpenOptions};
use std::io::{BufReader, Write};
use std::path;
use std::str::SplitWhitespace;
use std::time::{Duration, SystemTime};

use espidf_std::prelude::*;

// ls -l $dir
fn run_ls(dir: &path::PathBuf) -> Result<()> {
    let now = SystemTime::now();
    for entry in std::fs::read_dir(dir)? {
        let entry = entry?;
        let path = entry.path();
        let metadata = entry.metadata()?;
        println!(
            "  {} {}b {}s {}",
            if metadata.is_dir() { "d" } else { "-" },
            metadata.len(),
            metadata.modified().map_or(u64::MAX, |t|
                now.duration_since(t).unwrap_or(Duration::new(0, 0)).as_secs()
            ),
            path.display()
        );
    }

    Ok(())
}

// cat $file
fn run_cat(file: &path::PathBuf) -> Result<()> {
    let f = File::open(file)?;

    // despejar todo o arquivo na saída padrão
    let mut reader = BufReader::with_capacity(128, f);
    std::io::copy(&mut reader, &mut std::io::stdout())?;
    println!();

    Ok(())
}

// echo "Hello, LittleFS!" > $file
fn run_echo_file(file: &path::PathBuf, contents: &str) -> Result<()> {
    let mut f = File::create(file)?;
    f.write_all(contents.as_bytes())?;
    Ok(())
}

// echo "Hello again!" >> $file
fn run_append_file(file: &path::PathBuf, contents: &str) -> Result<()> {
    let mut f = OpenOptions::new()
        .append(true)
        .open(file)?;
    f.write_all(contents.as_bytes())?;
    Ok(())
}

// cp $src $dst
fn run_cp(src: &path::PathBuf, dst: &path::PathBuf) -> Result<()> {
    let mut src_f = File::open(src)?;
    let mut dst_f = File::create(dst)?;
    std::io::copy(&mut src_f, &mut dst_f)?;
    Ok(())
}

// rm $file
fn run_rm(file: &path::PathBuf) -> Result<()> {
    std::fs::remove_file(file)?;
    Ok(())
}

fn run_mkdir(dir: &path::PathBuf) -> Result<()> {
    std::fs::create_dir(dir)?;
    Ok(())
}

fn run_rmdir(dir: &path::PathBuf) -> Result<()> {
    std::fs::remove_dir(dir)?;
    Ok(())
}

fn test_filesystem(mount_point: &path::PathBuf) -> Result<()> {
    log::info!("ls {}", mount_point.display());
    run_ls(mount_point)?;

    let test_file = &mount_point.join("teste.txt");
    log::info!("cat {}", test_file.display());
    run_cat(test_file).or_else(|e| {
        log::warn!("Falha ao ler arquivo de teste: {e:?}");
        Result::Ok(())
    })?;

    let content = "Ola, LittleFS!";
    log::info!("echo '{}' > {}", content, test_file.display());
    run_echo_file(test_file, content)?;

    let content = "Tchau, LittleFS!";
    log::info!("echo '{}' >> {}", content, test_file.display());
    run_append_file(test_file, content)?;
    
    log::info!("cat {}", test_file.display());
    run_cat(test_file)?;

    // Copiar arquivo
    let copy_file = &mount_point.join("teste_copy.txt");
    log::info!("cp {} {}", test_file.display(), copy_file.display());
    run_cp(test_file, copy_file)?;

    // Listagem
    log::info!("ls {}", mount_point.display());
    run_ls(mount_point)?;

    // Remover arquivo
    log::info!("rm {}", test_file.display());
    run_rm(test_file)?;

    Ok(())
}

fn resolve_path(base_path: &path::Path, path: Option<&str>) -> Result<path::PathBuf> {
    let resolved = match path.filter(|p| !p.is_empty()) {
        None => base_path.to_path_buf(),
        Some(p) => {
            let p = std::path::Path::new(p);
            if p.is_absolute() { p.to_path_buf() } else { base_path.join(p) }
        }
    };

    Ok(path::absolute(resolved)?)
}

fn run_cmd(wd: &mut path::PathBuf, cmd: &str, parts: &mut SplitWhitespace) -> Result<()> {
    match cmd {
        "cd" => {
            let path = resolve_path(wd, parts.next())?;
            // Verifica se existe
            if !path.is_dir() {
                log::error!("Diretório não encontrado: {}", path.display());
            } else {
                let path = path.canonicalize()?;
                wd.clone_from(&path);
            }
        }
        "ls" => {
            let path = resolve_path(wd, parts.next())?;
            run_ls(&path)?;
        }
        "cat" => {
            let path = resolve_path(wd, parts.next())?;
            run_cat(&path)?;
        }
        "rm" => {
            let path = resolve_path(wd, parts.next())?;
            run_rm(&path)?;
        }
        "cp" => {
            let (src, dst) = (parts.next(), parts.next());
            let src = resolve_path(wd, src)?;
            let dst = resolve_path(wd, dst)?;
            run_cp(&src, &dst)?;
        }
        "mkdir" => {
            let path = resolve_path(wd, parts.next())?;
            run_mkdir(&path)?;
        }
        "rmdir" => {
            let path = resolve_path(wd, parts.next())?;
            run_rmdir(&path)?;
        }
        "echo" => {
            let tokens: Vec<&str> = parts.by_ref().collect();

            if let Some(op_pos) = tokens.iter().rposition(|&s| s == ">" || s == ">>") {
                let op = tokens[op_pos];
                let file_part = tokens.get(op_pos + 1).copied();
                let mut content = tokens[..op_pos].join(" ");
                content.push('\n');

                let path = resolve_path(wd, file_part)?;

                if op == ">>" {
                    run_append_file(&path, &content)?;
                } else {
                    run_echo_file(&path, &content)?;
                }
            } else {
                // sem redirecionamento: só imprime
                println!("{}", tokens.join(" "));
            }
        }
        "test" => {
            test_filesystem(wd)?;
        }
        "help" | "?" => {
            log::info!("Comandos disponíveis:");
            log::info!("  cd <dir>       - Muda o diretório de trabalho");
            log::info!("  ls [dir]       - Lista arquivos no diretório");
            log::info!("  cat <file>     - Exibe o conteúdo de um arquivo");
            log::info!("  echo <text>    - Imprime texto (sem redirecionamento)");
            log::info!("  echo <text> > <file>   - Escreve texto em arquivo (sobrescreve)");
            log::info!("  echo <text> >> <file>  - Escreve texto em arquivo (anexa)");
            log::info!("  cp <src> <dst> - Copia um arquivo");
            log::info!("  rm <file>      - Remove um arquivo");
            log::info!("  mkdir <dir>    - Cria um diretório");
            log::info!("  rmdir <dir>    - Remove um diretório vazio");
            log::info!("  test           - Roda os testes de filesystem (ls, cat, echo, cp)");
            log::info!("  help | ?       - Mostra esta ajuda");
        }
        _ => log::error!("Comando desconhecido: {cmd} experimente chamar ajuda com 'help'"),
    }
    Ok(())
}

fn run_terminal(mount_point: &str) -> Result<()> {
    let mut wd = path::PathBuf::from(mount_point);

    loop {
        print!("{}# ", wd.display());
        std::io::stdout().flush()?;

        let mut input = String::new();
        if std::io::stdin().read_line(&mut input)? == 0 {
            // EOF
            return Ok(());
        }
        let input = input.trim();
        let mut parts = input.split_whitespace();
        println!();
        if let Some(cmd) = parts.next() {
            run_cmd(&mut wd, cmd, &mut parts).or_else(|e| {
                log::error!("Erro ao executar comando '{cmd}': {e:?}");
                Result::Ok(())
            })?;
        }
    }
}


espidf_only! {
    // Nome da partição definida no partitions.csv
    const PARTITION_LABEL: &str = "spiffs";
    const MOUNT_POINT: &str = "/littlefs";
    const FORMAT_ON_FAIL: bool = true;

    use esp_idf_svc::hal::peripherals::Peripherals;
    use esp_idf_svc::hal::usb_serial::{UsbSerialConfig, UsbSerialDriver};
    use esp_idf_svc::io::vfs::BlockingStdIo;
    use esp_idf_svc::fs::littlefs::Littlefs;
    use esp_idf_svc::io::vfs::MountedLittlefs;
    use esp_idf_svc::sys;

    pub fn main() -> Result<()> {
        esp_idf_svc::sys::link_patches();
        esp_idf_svc::log::EspLogger::initialize_default();

        let peripherals = Peripherals::take()?;

        // Monta o filesystem — equivalente ao LittleFS.begin(true) do Arduino:
        // formata automaticamente se a partição estiver corrompida ou não inicializada.
        // Obs: no arduino a term a diferença que o grow_on_mount que é true por padrão, aqui respeitará o partitions.csv
        log::info!("Inicializando partição '{}'", PARTITION_LABEL);
        let littlefs: Littlefs<()> = unsafe { Littlefs::new_partition(PARTITION_LABEL)? };

        log::info!("Montando LittleFS em '{}'", MOUNT_POINT);
        let mounted = MountedLittlefs::mount(littlefs, MOUNT_POINT)
            .or_else(|e| -> Result<_> {
                if !FORMAT_ON_FAIL || e.code() != sys::ESP_FAIL {
                    return Err(e.into()); 
                }
                log::warn!("Falha ao montar LittleFS: {e:?}. Formatando partição '{PARTITION_LABEL}'...");
                // Precisa criar novamente pois o anterior foi movido
                let mut littlefs: Littlefs<()> = unsafe { Littlefs::new_partition(PARTITION_LABEL)? };
                littlefs.format()?;
                log::info!("Partição formatada. Remontando...");
                Ok(MountedLittlefs::mount(littlefs, MOUNT_POINT)?)
            })?;
        
        let info = mounted.info()?;
        log::info!("Informações do sistema de arquivos:");
        log::info!("  Tamanho total : {} bytes ({} KB)", info.total_bytes, info.total_bytes / 1024);
        log::info!("  Espaço usado  : {} bytes ({} KB)", info.used_bytes,  info.used_bytes  / 1024);
        log::info!(
            "  Espaço livre  : {} bytes ({} KB)",
            info.total_bytes - info.used_bytes,
            (info.total_bytes - info.used_bytes) / 1024
        );

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

        run_terminal(MOUNT_POINT)?;

        // drop de mounted aqui
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test_log::test]
    fn test_littlefs() {
        // Testa a função de teste do filesystem em ./assets sem precisar do hardware
        let mount_point = path::PathBuf::from("./assets");
        // Cria o diretório se não existir
        std::fs::create_dir_all(&mount_point).unwrap();
        test_filesystem(&mount_point).unwrap();
    }

    fn run_cmds (wd: &mut path::PathBuf, cmds: &[&str]) -> Result<()> {
        for cmd in cmds {
            let mut parts = cmd.split_whitespace();
            let cmd_name = parts.next().unwrap_or("");
            println!("> {cmd}");
            run_cmd(wd, cmd_name, &mut parts)
                .map_err(|e| {
                    log::error!("Erro ao executar comando '{cmd}': {e:?}");
                    e
                })?;
            println!();
        }
        Ok(())
    }

    #[test_log::test]
    fn test_cmds() {
        // Testa a função de execução de comandos em ./assets sem precisar do hardware
        let mut mount_point = path::PathBuf::from("./assets");
        // Cria o diretório se não existir
        std::fs::create_dir_all(&mount_point).unwrap();

        run_cmds(&mut mount_point, &[
            "ls",
            "mkdir testdir",
            "cd testdir",
            "echo Hello > testfile.txt",
            "cat testfile.txt",
            "echo World >> testfile.txt",
            "cat testfile.txt",
            "cp testfile.txt copy.txt",
            "cd ..",
            "rm testdir/testfile.txt",
            "rm testdir/copy.txt",
            "rmdir testdir",
        ]).unwrap();

        assert!(std::fs::metadata(format!("{}/testdir", &mount_point.display())).is_err());
    }
}