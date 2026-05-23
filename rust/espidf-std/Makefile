EXAMPLE ?= gps
# MCU  	?= esp32c3
MCU	   	?= esp32s3
PARTITION_TABLE ?= partitions/large_spiffs_8MB.csv
FLASH_SIZE ?= 8mb

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
## https://github.com/esp-rs/esp-idf-sys/issues/385 (Detalhes sobre partitions.csv)
flash: build-rust
	espflash flash $(ELF) $(if $(PARTITION_TABLE),--partition-table $(PARTITION_TABLE),) --monitor $(if $(PORT),--port $(PORT),)

## Apenas monitora a porta serial (sem flash)
monitor:
	espflash monitor $(if $(PORT),--port $(PORT),)

## Executa os testes (NO HOST)
test:
	export MCU=$(MCU) && \
	export RUST_BACKTRACE=1 && \
	cargo test --example $(EXAMPLE) --no-default-features --target "x86_64-unknown-linux-gnu" -- --nocapture

## Simula no Wokwi (requer extensão Wokwi no VS Code ou wokwi-cli)
## Acontece que o wokwi não suporta o partitions.csv https://github.com/wokwi/wokwi-features/issues/523
## Então tem que usar o espflash para gerar a imagem final tudo em um único binário (merge) e depois usar esse binário no Wokwi
simulate: build-rust
	espflash save-image --flash-size $(FLASH_SIZE) --chip $(MCU) --merge $(if $(PARTITION_TABLE),--partition-table $(PARTITION_TABLE),) $(ELF) target/wokwi_merged.bin
	echo '{"flash_settings": {"flash_size": "$(FLASH_SIZE)"}, "flash_files": {"0x0": "wokwi_merged.bin"}}' > target/flasher_args.json
#	wokwi-cli --timeout 0 --flash $(ELF)
