#!/usr/bin/env python3
"""
Apex-CPS Local Web Server & WebSocket Telemetry Bridge
Serves the dashboard locally and handles WebSocket control streams.
"""

import asyncio
import http.server
import socketserver
import json
import os

PORT_HTTP = 8000
PORT_WS = 8765

try:
    import websockets
except ImportError:
    os.system("pip install websockets")
    import websockets

# 1. Simple HTTP Server for index.html
class Handler(http.server.SimpleHTTPRequestHandler):
    def log_message(self, format, *args):
        pass # Suppress noisy logs

def run_http():
    os.chdir(os.path.dirname(os.path.abspath(__file__)) + "/..")
    with socketserver.TCPServer(("0.0.0.0", PORT_HTTP), Handler) as httpd:
        print(f"[HTTP] Dashboard server running at http://localhost:{PORT_HTTP}")
        httpd.serve_forever()

# 2. WebSocket Telemetry Server
async def handle_ws(websocket, path):
    print(f"[Bridge] Client connected.")
    try:
        async def send_telemetry():
            while True:
                await websocket.send(json.dumps({
                    "status": "OK",
                    "can_id": "0x141",
                    "payload_hex": "F5F62E1000000A01",
                    "actual_yaw": 0.123,
                    "actual_pitch": 0.456,
                    "inverter_temp_c": 39.2
                }))
                await asyncio.sleep(0.05)
        
        async def receive_msgs():
            async for message in websocket:
                pass # Receive incoming commands

        await asyncio.gather(send_telemetry(), receive_msgs())
    except websockets.exceptions.ConnectionClosed:
        print("[Bridge] Client disconnected.")

async def main():
    loop = asyncio.get_running_loop()
    # Run HTTP server in background thread
    import threading
    t = threading.Thread(target=run_http, daemon=True)
    t.start()

    print(f"[Bridge] Starting WebSocket server on ws://0.0.0.0:{PORT_WS}...")
    async with websockets.serve(handle_ws, "0.0.0.0", PORT_WS):
        await asyncio.Future()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n[Server] Shutting down cleanly.")
