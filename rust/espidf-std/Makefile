EXAMPLE ?= primos
# MCU  	?= esp32c3
MCU	   	?= esp32s3

# Mapeamento MCU → TARGET
ifeq ($(MCU), esp32c3)
  TARGET := riscv32imc-esp-espidf
else ifeq ($(MCU), esp32s3)
  TARGET := xtensa-esp32s3-espidf
endif

BINARY     := examples/$(EXAMPLE)
BUILD_TYPE := release
ELF        := target/$(TARGET)/$(BUILD_TYPE)/$(BINARY)
PORT       ?= # ex: PORT=/dev/ttyUSB0

build-rust:
# Para placas Xtensa tem que ser com esse aqui
# Precisa ter instalado o espup "espup install"
ifeq ($(MCU), esp32s3)
	. ${HOME}/export-esp.sh && \
	export MCU=$(MCU) && \
	cargo +esp build --example $(EXAMPLE) --target $(TARGET) --features "$(MCU)" --release
else
# Placas RISC-V (ex: ESP32-C3) pode ser rust normal
# Precisa ter instalado a toolchain com rust-src e o target "rustup target add riscv32imc-unknown-none-elf" para ESP32-C3
	export MCU=$(MCU) && \
	cargo +nightly build --example ${EXAMPLE} --target ${TARGET} --features "$(MCU)" --release
endif

## Faz flash na placa (usa espflash com monitor integrado)
## https://github.com/esp-rs/esp-idf-template#flash
flash: build-rust
	espflash flash $(ELF) --monitor $(if $(PORT),--port $(PORT),)

## Apenas monitora a porta serial (sem flash)
monitor:
	espflash monitor $(if $(PORT),--port $(PORT),)

## Executa os testes (NO HOST)
test:
	export MCU=$(MCU) && \
	cargo test --example $(EXAMPLE) --no-default-features --target "x86_64-unknown-linux-gnu"

## Simula no Wokwi (requer extensão Wokwi no VS Code ou wokwi-cli)
simulate: build-rust
	wokwi-cli --timeout 0 --flash $(ELF)
