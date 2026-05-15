EXAMPLE    := blink
BINARY     := examples/$(EXAMPLE)
TARGET     := riscv32imc-esp-espidf
BUILD_TYPE := release
ELF        := target/$(TARGET)/$(BUILD_TYPE)/$(BINARY)
PORT       ?= # ex: PORT=/dev/ttyUSB0

# Para placas Xtensa tem que ser com esse aqui
# Precisa ter instalado o espup "espup install"
build-espup:
	. ${HOME}/export-esp.sh && cargo build --example ${EXAMPLE} --release

# Placas RISC-V (ex: ESP32-C3) pode ser rust normal
# Precisa ter instalado a toolchain com rust-src e o target "rustup target add riscv32imc-unknown-none-elf" para ESP32-C3
build-rust:
	cargo build --example ${EXAMPLE} --release

## Faz flash na placa (usa espflash com monitor integrado)
## https://github.com/esp-rs/esp-idf-template#flash
flash: build-rust
	espflash flash $(ELF) --monitor $(if $(PORT),--port $(PORT),)

## Apenas monitora a porta serial (sem flash)
monitor:
	espflash monitor $(if $(PORT),--port $(PORT),)

## Executa os testes (NO HOST)
test:
	cargo test --lib --no-default-features --target x86_64-unknown-linux-gnu

## Simula no Wokwi (requer extensão Wokwi no VS Code ou wokwi-cli)
simulate: build-rust
	wokwi-cli --timeout 0 --flash $(ELF)
