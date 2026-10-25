#!/usr/bin/env python3
"""
Apex-CPS Actuator Telemetry Visualizer & Performance Analyzer
Parses CSV logs and plots position tracking trajectories and error metrics.
"""

import os
import glob
import sys

try:
    import pandas as pd
    import matplotlib.pyplot as plt
    import numpy as np
except ImportError:
    print("Installing data analysis packages (pandas, matplotlib)...")
    os.system("pip install pandas matplotlib numpy")
    import pandas as pd
    import matplotlib.pyplot as plt
    import numpy as np

LOG_DIR = "logs"

def get_latest_log():
    if not os.path.exists(LOG_DIR):
        print(f"Error: Log directory '{LOG_DIR}' does not exist.")
        return None
    files = glob.glob(os.path.join(LOG_DIR, "telemetry_log_*.csv"))
    if not files:
        print("Error: No telemetry CSV log files found.")
        return None
    latest_file = max(files, key=os.path.getctime)
    return latest_file

def analyze_logs():
    csv_path = get_latest_log()
    if not csv_path:
        return

    print(f"Analyzing telemetry log: {csv_path}")
    df = pd.read_csv(csv_path)
    
    if df.empty or len(df) < 5:
        print("Error: Log file is empty or contains insufficient data points.")
        return

    # Compute tracking errors
    df['yaw_error'] = df['target_yaw'] - df['actual_yaw']
    df['pitch_error'] = df['target_pitch'] - df['actual_pitch']

    yaw_rms = np.sqrt(np.mean(df['yaw_error']**2))
    pitch_rms = np.sqrt(np.mean(df['pitch_error']**2))

    print("\n--- Performance Metrics Summary ---")
    print(f"Total Samples Logged : {len(df)}")
    print(f"Yaw Axis RMS Error   : {yaw_rms:.4f} rad")
    print(f"Pitch Axis RMS Error : {pitch_rms:.4f} rad")
    print(f"Max Inverter Temp    : {df['inverter_temp_c'].max():.1f} °C")
    print("-----------------------------------\n")

    # Plotting trajectories
    plt.figure(figsize=(10, 6))
    
    plt.subplot(2, 1, 1)
    plt.plot(df['target_yaw'], label='Target Yaw (rad)', color='#2f81f7', linestyle='--')
    plt.plot(df['actual_yaw'], label='Actual Yaw (rad)', color='#3fb950')
    plt.title('Apex-CPS Dual-Axis Actuator Tracking Performance')
    plt.ylabel('Yaw (rad)')
    plt.legend(loc='upper right')
    plt.grid(True, alpha=0.3)

    plt.subplot(2, 1, 2)
    plt.plot(df['target_pitch'], label='Target Pitch (rad)', color='#da3633', linestyle='--')
    plt.plot(df['actual_pitch'], label='Actual Pitch (rad)', color='#a371f7')
    plt.xlabel('Sample Index')
    plt.ylabel('Pitch (rad)')
    plt.legend(loc='upper right')
    plt.grid(True, alpha=0.3)

    plt.tight_layout()
    plot_filename = csv_path.replace('.csv', '_plot.png')
    plt.savefig(plot_filename)
    print(f"Performance plot successfully saved to {plot_filename}")

if __name__ == "__main__":
    analyze_logs()
