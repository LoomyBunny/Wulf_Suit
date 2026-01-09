# Wulf Suit - Raspberry Pi Pico WS2812 LED Controller

A Pico 2 project for controlling WS2812 LED strips with mode switching and brightness control.

## Quick Start

```bash
# Build the project
make

# Flash to Pico (when in bootloader mode)
make flash
```

## Hardware

- **Board**: Raspberry Pi Pico 2
- **LED Strip**: WS2812 (RGB, not RGBW)
- **Controls**:
  - Mode switch button (GPIO 0)
  - Brightness switch button (GPIO 11)
  - LED data pin (GPIO 28)

## Features

- Multiple color modes: Red, Teal, Purple, Warm White, Cool White, Rainbow, White
- Adjustable brightness (1-255)
- Emergency Power Off (EPO) safety mode
- USB serial debug output

## Development

### Prerequisites
- Pico SDK in `../pico-sdk/`
- ARM toolchain (arm-none-eabi-gcc)
- CMake 3.13+

### Build System
Uses a simplified Makefile that:
- Configures CMake with Pico SDK
- Cross-compiles for ARM Cortex-M33
- Generates UF2 file for flashing

### Project Structure
```
├── Makefile              # Simplified build system
├── CMakeLists.txt         # Pico SDK configuration
├── wulf_suit.c          # Main application
├── ws2812.pio           # PIO program for WS2812
├── pico_sdk_import.cmake # SDK import helper
└── .gitignore           # Git ignore rules
```

## Flashing

1. Hold BOOTSEL button on Pico while plugging into USB
2. Copy `build/wulf_suit.uf2` to the RPI-RP2 drive
3. Pico will automatically reboot and run the firmware

## Mode Operation

- **Mode Button**: Cycle through color modes
- **Brightness Button**: Increase brightness
- **Both Buttons**: Activate EPO safety mode

## Git Ready

This repository is git-ready with proper `.gitignore` to exclude build artifacts while keeping essential project files.