#!/usr/bin/env python3
"""
Apex-CPS Telemetry CSV Recorder & Performance Plot Generator
Generates closed-loop step response, 250 Hz notch resonance suppression,
and thermal derating telemetry CSV data, and exports a 4-panel PNG report
plus a terminal ASCII quick-view chart.
"""

import csv
import math
import os

CSV_PATH = "logs/telemetry_run.csv"
PNG_PATH = "logs/telemetry_report.png"

class BiquadNotch:
    def __init__(self, f_c, f_s, q):
        w0 = 2.0 * math.pi * f_c / f_s
        alpha = math.sin(w0) / (2.0 * q)
        cos_w0 = math.cos(w0)
        a0 = 1.0 + alpha
        self.b0 = 1.0 / a0
        self.b1 = (-2.0 * cos_w0) / a0
        self.b2 = 1.0 / a0
        self.a1 = (-2.0 * cos_w0) / a0
        self.a2 = (1.0 - alpha) / a0
        self.x1 = self.x2 = self.y1 = self.y2 = 0.0

    def update(self, x):
        y = (self.b0 * x) + (self.b1 * self.x1) + (self.b2 * self.x2) - (self.a1 * self.y1) - (self.a2 * self.y2)
        self.x2, self.x1 = self.x1, x
        self.y2, self.y1 = self.y1, y
        return y

def compute_thermal_derating(temp_c):
    if temp_c <= 75.0:
        return 1.0
    if temp_c >= 95.0:
        return 0.0
    return (95.0 - temp_c) / (95.0 - 75.0)

def generate_telemetry_csv():
    os.makedirs("logs", exist_ok=True)
    dt = 0.001
    fs = 1000.0
    notch = BiquadNotch(250.0, fs, 4.0)

    pos = 0.0
    vel = 0.0
    integral_vel = 0.0
    kp_pos, kp_vel, ki_vel = 15.0, 4.0, 20.0
    max_vel, max_cur = 12.0, 20.0
    inertia, damping = 0.045, 0.15

    rows = []
    for step in range(1500):
        t = step * dt
        target_pos = 1.0 if t >= 0.05 else 0.0

        # Simulate inverter temperature rising from 40°C to 92°C across the run
        temp_c = 40.0 + (52.0 * (step / 1500.0))
        derate = compute_thermal_derating(temp_c)
        eff_max_cur = max_cur * derate

        target_vel = max(-max_vel, min(max_vel, kp_pos * (target_pos - pos)))
        vel_err = target_vel - vel
        integral_vel = max(-1.0, min(1.0, integral_vel + vel_err * dt))
        i_ff = 1.85 * math.cos(pos)

        # Inject 250 Hz mechanical harmonic drive ripple into raw torque command
        ripple_250hz = 1.2 * math.sin(2.0 * math.pi * 250.0 * t)
        raw_iq = (kp_vel * vel_err) + (ki_vel * integral_vel) + i_ff + ripple_250hz
        filtered_iq = notch.update(raw_iq)
        iq_cmd = max(-eff_max_cur, min(eff_max_cur, filtered_iq))

        tau_grav = 1.85 * math.cos(pos)
        accel = (iq_cmd - tau_grav - damping * vel) / inertia
        vel += accel * dt
        pos += vel * dt

        rows.append([
            round(t, 4),
            round(target_pos, 4),
            round(pos, 4),
            round(raw_iq, 4),
            round(iq_cmd, 4),
            round(temp_c, 2),
            round(eff_max_cur, 2)
        ])

    with open(CSV_PATH, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["time_s", "target_pos_rad", "actual_pos_rad", "raw_iq_a", "filtered_iq_a", "temp_c", "max_iq_limit_a"])
        writer.writerows(rows)

    print(f"[Telemetry] Saved 1,500 closed-loop samples to {CSV_PATH}")
    return rows

def print_ascii_summary(rows):
    print("\n--- Terminal ASCII Step Response Preview (Pitch Axis) ---")
    for i in range(0, len(rows), 100):
        t, tgt, pos, _, iq, temp, lim = rows[i]
        bar_len = int(max(0.0, min(1.2, pos)) * 30)
        bar = "#" * bar_len
        print(f"t={t:0.2f}s | Pos={pos:0.3f} rad | Iq={iq:5.2f}A | Temp={temp:4.1f}C | [{bar:<36}]")

def plot_png_report(rows):
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        print("\n[Notice] matplotlib not installed; skipping PNG chart generation (ASCII + CSV complete).")
        return

    t = [r[0] for r in rows]
    tgt = [r[1] for r in rows]
    pos = [r[2] for r in rows]
    raw_iq = [r[3] for r in rows]
    filt_iq = [r[4] for r in rows]
    temp = [r[5] for r in rows]
    lim = [r[6] for r in rows]

    fig, axs = plt.subplots(2, 2, figsize=(12, 8), facecolor="#0b0f19")
    fig.suptitle("Apex-CPS Dual-Axis Actuator — Closed-Loop HIL Telemetry Report", color="white", fontsize=14, fontweight="bold")

    panels = [
        (axs[0, 0], "1. Step Response Tracking (Pitch Axis)", "Position (rad)"),
        (axs[0, 1], "2. 250 Hz Biquad Notch Filter Suppression", "Phase Current Iq (A)"),
        (axs[1, 0], "3. Inverter Thermal Derating Curve (75°C–95°C)", "Max Current Limit (A)"),
        (axs[1, 1], "4. Tracking Error Convergence", "Error (rad)")
    ]

    for ax, title, ylabel in panels:
        ax.set_facecolor("#111827")
        ax.set_title(title, color="#38bdf8", fontsize=10, fontweight="bold")
        ax.set_xlabel("Time (s)", color="#94a3b8", fontsize=9)
        ax.set_ylabel(ylabel, color="#94a3b8", fontsize=9)
        ax.tick_params(colors="#94a3b8", labelsize=8)
        ax.grid(True, color="#1e293b", linestyle="--", linewidth=0.6)

    axs[0, 0].plot(t, tgt, "--", color="#94a3b8", label="Target (1.0 rad)")
    axs[0, 0].plot(t, pos, color="#38bdf8", linewidth=2, label="Actual Pitch")
    axs[0, 0].legend(facecolor="#1e293b", labelcolor="white", fontsize=8)

    axs[0, 1].plot(t[:300], raw_iq[:300], color="#f43f5e", alpha=0.5, label="Unfiltered (250Hz Ripple)")
    axs[0, 1].plot(t[:300], filt_iq[:300], color="#10b981", linewidth=1.5, label="Notch Filtered Iq")
    axs[0, 1].legend(facecolor="#1e293b", labelcolor="white", fontsize=8)

    axs[1, 0].plot(temp, lim, color="#f59e0b", linewidth=2, label="Derated Max Iq")
    axs[1, 0].axvline(75.0, color="#fbbf24", linestyle=":", label="75°C Derate Onset")
    axs[1, 0].set_xlabel("Inverter Temperature (°C)", color="#94a3b8", fontsize=9)
    axs[1, 0].legend(facecolor="#1e293b", labelcolor="white", fontsize=8)

    err = [tgt[i] - pos[i] for i in range(len(t))]
    axs[1, 1].plot(t, err, color="#a855f7", linewidth=1.5, label="Position Error")
    axs[1, 1].legend(facecolor="#1e293b", labelcolor="white", fontsize=8)

    plt.tight_layout()
    plt.savefig(PNG_PATH, dpi=180, facecolor="#0b0f19")
    print(f"\n[Plotter] Exported 4-panel dark-mode PNG report to {PNG_PATH}")

if __name__ == "__main__":
    data = generate_telemetry_csv()
    print_ascii_summary(data)
    plot_png_report(data)
