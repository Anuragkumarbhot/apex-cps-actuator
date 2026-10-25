#!/usr/bin/env bash
# Apex-CPS Actuator MCU Flashing Automation Script for Termiux

print_banner() {
    echo "=================================================="
    echo "  Apex-CPS STM32G4 Firmware Flasher               "
    echo "=================================================="
}

print_banner

# 1. Locate firmware binary
FIRMWARE_BIN="firmware/build/apex_actuator_firmware.bin"
FIRMWARE_ELF="firmware/build/apex_actuator_firmware"

if [ ! -f "$FIRMWARE_BIN" ] && [ ! -f "$FIRMWARE_ELF" ]; then
    echo "[Error] Compiled firmware binary not found!"
    echo "Please build the firmware first using CMake inside the firmware/build/ directory."
    exit 1
fi

# 2. Check for flashing tool (OpenOCD or stm32flash)
if command -v openocd &> /dev/null; then
    echo "[Flasher] OpenOCD detected. Flashing STM32G4 via ST-Link/SWD..."
    openocd -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program $FIRMWARE_ELF verify reset exit"
elif command -v stm32flash &> /dev/null; then
    echo "[Flasher] stm32flash detected. Flashing via UART/USB DFU..."
    stm32flash -w "$FIRMWARE_BIN" -v -g 0x08000000 /dev/ttyUSB0
else
    echo "[Notice] No hardware flasher tool (openocd / stm32flash) found in Termiux."
    echo "To install OpenOCD in Termiux, run: pkg install openocd"
    echo "Binary is compiled and ready at: firmware/build/"
    exit 0
fi

echo "[Success] Firmware successfully flashed to STM32G4!"
