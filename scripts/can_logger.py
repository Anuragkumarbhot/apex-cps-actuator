#!/usr/bin/env python3
"""
Apex-CPS Actuator Real-Time CAN Telemetry Logger
Records live telemetry packets to a CSV file for controller tuning and performance analysis.
"""

import asyncio
import json
import csv
import os
from datetime import datetime

try:
    import websockets
except ImportError:
    os.system("pip install websockets")
    import websockets

LOG_DIR = "logs"
os.makedirs(LOG_DIR, exist_ok=True)
timestamp_str = datetime.now().strftime("%Y%m%d_%H%M%S")
csv_filename = os.path.join(LOG_DIR, f"telemetry_log_{timestamp_str}.csv")

async def log_telemetry():
    uri = "ws://localhost:8765"
    print(f"[Logger] Connecting to telemetry bridge at {uri}...")
    
    # Initialize CSV log file
    with open(csv_filename, mode='w', newline='') as csv_file:
        fieldnames = ['timestamp', 'can_id', 'target_yaw', 'target_pitch', 'actual_yaw', 'actual_pitch', 'inverter_temp_c', 'payload_hex']
        writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        writer.writeheader()
        
        print(f"[Logger] Logging active! Saving data to {csv_filename}")
        print(f"{'Time':<12} | {'Target Yaw':<12} | {'Actual Yaw':<12} | {'Target Pitch':<12} | {'Actual Pitch':<12}")
        print("-" * 65)

        try:
            async with websockets.connect(uri) as websocket:
                while True:
                    message = await websocket.recv()
                    data = json.loads(message)
                    
                    now = datetime.now().strftime("%H:%M:%S.%f")[:-3]
                    row = {
                        'timestamp': now,
                        'can_id': data.get('can_id', '0x141'),
                        'target_yaw': data.get('target_yaw', 0.0),
                        'target_pitch': data.get('target_pitch', 0.0),
                        'actual_yaw': data.get('actual_yaw', 0.0),
                        'actual_pitch': data.get('actual_pitch', 0.0),
                        'inverter_temp_c': data.get('inverter_temp_c', 38.0),
                        'payload_hex': data.get('payload_hex', '')
                    }
                    writer.writerow(row)
                    csv_file.flush()
                    
                    print(f"{now:<12} | {row['target_yaw']:<12.3f} | {row['actual_yaw']:<12.3f} | {row['target_pitch']:<12.3f} | {row['actual_pitch']:<12.3f}")
                    
        except websockets.exceptions.ConnectionClosedError:
            print("[Logger] Connection to WebSocket bridge lost. Retrying in 3 seconds...")
            await asyncio.sleep(3)

if __name__ == "__main__":
    try:
        asyncio.run(log_telemetry())
    except KeyboardInterrupt:
        print(f"\n[Logger] Logging stopped. Session saved to {csv_filename}")
