#!/usr/bin/env python3
"""
Apex-CPS Actuator UART & CAN Serial Terminal Monitor
Reads live debug logs and telemetry from STM32G4 firmware via USB/UART.
"""

import sys
import time

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print("Installing pyserial...")
    import os
    os.system("pip install pyserial")
    import serial
    import serial.tools.list_ports

def list_available_ports():
    ports = serial.tools.list_ports.comports()
    print("Available Serial Ports:")
    for p in ports:
        print(f"  - {p.device} ({p.description})")
    return ports

def monitor_serial(port_name, baudrate=115200):
    print(f"\n[Serial] Connecting to {port_name} at {baudrate} baud...")
    try:
        ser = serial.Serial(port_name, baudrate, timeout=0.1)
        print("[Serial] Connected! Listening for firmware logs (Press Ctrl+C to exit)...\n")
        
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(f"[STM32] {line}")
            time.sleep(0.01)
            
    except serial.SerialException as e:
        print(f"[Error] Serial connection error: {e}")
    except KeyboardInterrupt:
        print("\n[Serial] Monitor closed.")

if __name__ == "__main__":
    ports = list_available_ports()
    if not ports:
        print("\n[Notice] No active serial ports detected.")
        print("Connect your STM32/USB UART adapter via USB OTG to your Android device.")
        sys.exit(0)
    
    # Default to the first available port
    target_port = ports[0].device
    monitor_serial(target_port)
