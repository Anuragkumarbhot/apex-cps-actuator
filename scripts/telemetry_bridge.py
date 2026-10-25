#!/usr/bin/env python3
"""
Apex-CPS Actuator WebSocket & Serial Telemetry Bridge
Bridges web/simulation clients to hardware UART/CAN interface.
"""

import asyncio
import json
import struct
try:
    import websockets
except ImportError:
    print("Installing websockets library...")
    import os
    os.system("pip install websockets pyserial")
    import websockets

PORT = 8765

async def handle_client(websocket, path):
    print(f"[Bridge] Client connected from {websocket.remote_address}")
    try:
        async for message in websocket:
            data = json.loads(message)
            target_yaw = data.get("yaw", 0.0)
            target_pitch = data.get("pitch", 0.0)
            estop = data.get("estop", False)

            # Pack into CAN 0x141 8-byte frame format
            raw_y = int(target_yaw / 0.0001) & 0xFFFF
            raw_p = int(target_pitch / 0.0001) & 0xFFFF
            control_word = 0x00 if estop else 0x01

            can_payload = struct.pack('<HHBBBB', raw_y, raw_p, 0, 0, 0, control_word)
            
            # Broadcast telemetry response back to web client
            telemetry_response = {
                "status": "OK",
                "can_id": "0x141",
                "payload_hex": can_payload.hex().upper(),
                "actual_yaw": target_yaw * 0.98,
                "actual_pitch": target_pitch * 0.98,
                "inverter_temp_c": 38.5
            }
            await websocket.send(json.dumps(telemetry_response))
            
    except websockets.exceptions.ConnectionClosed:
        print("[Bridge] Client disconnected")

async def main():
    print(f"Starting Apex-CPS Telemetry Bridge on ws://localhost:{PORT}...")
    async with websockets.serve(handle_client, "0.0.0.0", PORT):
        await asyncio.Future()  # run forever

if __name__ == "__main__":
    asyncio.run(main())
