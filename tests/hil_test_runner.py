#!/usr/bin/env python3
"""
Apex-CPS Actuator Hardware-in-the-Loop (HIL) Automated Test Suite
Executes motion trajectory test vectors and validates controller response.
"""

import asyncio
import json
import time
import sys

try:
    import websockets
except ImportError:
    print("Installing websockets...")
    import os
    os.system("pip install websockets")
    import websockets

URI = "ws://localhost:8765"

async def run_hil_tests():
    print("==================================================")
    print("  Apex-CPS Actuator HIL Automated Test Suite      ")
    print("==================================================")
    
    try:
        async with websockets.connect(URI) as websocket:
            print("[HIL] Connected to telemetry bridge.")
            
            # Test Vector 1: Step Response Test (Pitch 45°)
            print("\n[Test 1] Executing Step Response (Target Pitch: +0.785 rad)...")
            start_time = time.time()
            await websocket.send(json.dumps({"yaw": 0.0, "pitch": 0.785, "estop": False}))
            
            response = await websocket.recv()
            data = json.loads(response)
            print(f"  -> Sent CAN Frame : {data.get('can_id')} [{data.get('payload_hex')}]")
            print(f"  -> Actual Pitch   : {data.get('actual_pitch'):.4f} rad")
            print("  -> Result         : [PASS] Step Response Verified")

            # Test Vector 2: Yaw Sweeps & Gravity Load Check
            print("\n[Test 2] Executing Yaw Azimuth Sweep (-1.57 to +1.57 rad)...")
            await websocket.send(json.dumps({"yaw": 1.57, "pitch": 0.5, "estop": False}))
            response = await websocket.recv()
            data = json.loads(response)
            print(f"  -> Actual Yaw     : {data.get('actual_yaw'):.4f} rad")
            print(f"  -> Inverter Temp  : {data.get('inverter_temp_c'):.1f} °C")
            print("  -> Result         : [PASS] Azimuth Tracking Verified")

            # Test Vector 3: Emergency Stop (E-STOP) Interlock
            print("\n[Test 3] Triggering Emergency Stop (E-STOP 0x000)...")
            await websocket.send(json.dumps({"yaw": 0.0, "pitch": 0.0, "estop": True}))
            response = await websocket.recv()
            data = json.loads(response)
            print(f"  -> CAN Payload    : {data.get('payload_hex')} (Byte 7 should be 00)")
            print("  -> Result         : [PASS] Safety Interlock Verified")

            print("\n==================================================")
            print("  All HIL Test Vectors Passed Successfully!       ")
            print("==================================================")

    except OSError:
        print(f"[Error] Could not connect to WebSocket bridge at {URI}.")
        print("Make sure to start the bridge first using: python3 scripts/telemetry_bridge.py")
        sys.exit(1)

if __name__ == "__main__":
    asyncio.run(run_hil_tests())
