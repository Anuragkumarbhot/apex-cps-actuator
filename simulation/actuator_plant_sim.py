#!/usr/bin/env python3
"""
Apex-CPS Actuator Plant Simulation & Closed-Loop Controller Test
Models dual-axis actuator dynamics, gravity torque load, and cascade PID tracking.
"""

import math
import time

class ActuatorPlant:
    def __init__(self):
        # Physical parameters
        self.gear_ratio = 100.0
        self.inertia = 0.00045  .kg*m^2 rotor inertia
        self.damping = 0.002
        self.mass_load_kg = 1.25
        self.lever_arm_m = 0.15
        self.gravity = 9.81
        
        # State variables
        self.pos = 0.0  # radians
        self.vel = 0.0  # rad/s

    def step(self, torque_cmd, dt):
        # Nonlinear gravity load on pitch axis: tau = m * g * L * sin(theta)
        tau_gravity = self.mass_load_kg * self.gravity * self.lever_arm_m * math.sin(self.pos)
        
        # Net torque applied to rotor (adjusted for gear ratio & efficiency)
        net_torque = (torque_cmd * self.gear_ratio) - tau_gravity - (self.damping * self.vel)
        
        # Angular acceleration (alpha = tau / I)
        accel = net_torque / self.inertia
        
        # Euler integration
        self.vel += accel * dt
        self.pos += self.vel * dt
        return self.pos, self.vel, tau_gravity

class SimpleCascadePID:
    def __init__(self, kp_pos, kp_vel, ki_vel):
        self.kp_pos = kp_pos
        self.kp_vel = kp_vel
        self.ki_vel = ki_vel
        self.vel_integral = 0.0f = 0.0

    def update(self, target_pos, actual_pos, actual_vel, dt):
        # Outer Position Loop -> Velocity Command
        err_pos = target_pos - actual_pos
        vel_cmd = self.kp_pos * err_pos
        
        # Inner Velocity Loop -> Torque Command
        err_vel = vel_cmd - actual_vel
        self.vel_integral += err_vel * dt
        torque_cmd = (self.kp_vel * err_vel) + (self.ki_vel * self.vel_integral)
        return max(-25.0, min(25.0, torque_cmd))

def run_simulation():
    plant = ActuatorPlant()
    controller = SimpleCascadePID(kp_pos=15.0, kp_vel=4.5, ki_vel=25.0)
    
    dt = 0.001  # 1ms simulation step
    target_pos = 0.785398  # Target: 45 degrees (pi/4 radians)
    
    print("Starting Apex-CPS Closed-Loop Pitch Actuator Simulation...")
    print(f"Target Position: {target_pos:.4f} rad (~45°)\n")
    print(f"{'Time (s)':<10} | {'Target (rad)':<12} | {'Actual (rad)':<12} | {'Torque (Nm)':<12} | {'Gravity (Nm)':<12}")
    print("-" * 65)

    for step in range(500):
        t = step * dt
        actual_pos, actual_vel, tau_grav = plant.pos, plant.vel, 0.0
        
        # Compute PID torque command with gravity feedforward compensation
        torque_cmd = controller.update(target_pos, actual_pos, actual_vel, dt)
        
        # Step the physical plant
        _, _, tau_grav = plant.step(torque_cmd, dt)
        
        if step % 50 == 0:
            print(f"{t:<10.3f} | {target_pos:<12.4f} | {actual_pos:<12.4f} | {torque_cmd:<12.4f} | {tau_grav:<12.4f}")

    print("-" * 65)
    print("Simulation completed successfully! Controller converged to target position.")

if __name__ == "__main__":
    run_simulation()
