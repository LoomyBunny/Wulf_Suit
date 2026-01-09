# Simple Makefile for Wulf Suit - Pico SDK project

# Variables
PICO_SDK_PATH = ../pico-sdk
BOARD = pico2

# Default target
all:
	@echo "Configuring and building Wulf Suit..."
	/usr/bin/cmake -DPICO_SDK_PATH=$(realpath $(PICO_SDK_PATH)) -DPICO_BOARD=$(BOARD) -B build -S .
	/usr/bin/cmake --build build --target wulf_suit

# Clean
clean:
	rm -rf build

# Flash
flash: all
	@echo "Copy build/wulf_suit.uf2 to your Pico when in bootloader mode"
	@ls -la build/wulf_suit.uf2

# Help
help:
	@echo "Targets:"
	@echo "  all    - Configure and build (default)"
	@echo "  clean  - Remove build directory"
	@echo "  flash  - Build and show flash instructions"
	@echo "  help   - Show this help"

.PHONY: all clean flash help