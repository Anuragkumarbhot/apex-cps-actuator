#include "../include/pid_cascade.h"
#include <math.h>

static inline float clampf(float val, float min_val, float max_val) {
    if (val > max_val) return max_val;
    if (val < min_val) return min_val;
    return val;
}

void CascadePID_Init(CascadePID_t *pid, float kp_pos, float kp_vel, float ki_vel, float max_vel, float max_cur) {
    pid->kp_pos = kp_pos;
    pid->kp_vel = kp_vel;
    pid->ki_vel = ki_vel;
    pid->integral_vel = 0.0f;
    pid->max_vel_rad_s = max_vel;
    pid->max_current_a = max_cur;
    pid->gravity_ff_gain = 1.85f; // Default torque constant for Pitch axis load
    pid->enable_notch = 0;
}

void CascadePID_EnableNotch(CascadePID_t *pid, float f_center_hz, float f_sample_hz, float q_factor) {
    BiquadNotch_Init(&pid->notch_filter, f_center_hz, f_sample_hz, q_factor);
    pid->enable_notch = 1;
}

float Compute_Thermal_Derating_Factor(float inverter_temp_c) {
    if (inverter_temp_c <= THERMAL_DERATE_START_C) {
        return 1.0f; // 100% torque capability
    }
    if (inverter_temp_c >= THERMAL_DERATE_MAX_C) {
        return 0.0f; // Thermal cutoff protection triggered
    }
    // Linear ramp down between 75°C and 95°C
    return (THERMAL_DERATE_MAX_C - inverter_temp_c) / (THERMAL_DERATE_MAX_C - THERMAL_DERATE_START_C);
}

float CascadePID_Update(CascadePID_t *pid, float target_pos, float actual_pos, float actual_vel, float pitch_angle_rad, float inverter_temp_c, float dt) {
    // 1. Outer Position P-Loop -> Target Velocity
    float pos_error = target_pos - actual_pos;
    float target_vel = clampf(pid->kp_pos * pos_error, -pid->max_vel_rad_s, pid->max_vel_rad_s);

    // 2. Inner Velocity PI-Loop
    float vel_error = target_vel - actual_vel;
    pid->integral_vel += vel_error * dt;

    // Anti-windup clamping
    float max_i = pid->max_current_a / (pid->ki_vel > 0.001f ? pid->ki_vel : 1.0f);
    pid->integral_vel = clampf(pid->integral_vel, -max_i, max_i);

    // Gravity Feedforward Compensation: tau_g = K_g * cos(theta_pitch)
    float i_ff = pid->gravity_ff_gain * cosf(pitch_angle_rad);

    float raw_iq_cmd = (pid->kp_vel * vel_error) + (pid->ki_vel * pid->integral_vel) + i_ff;

    // 3. Apply Biquad Notch Filter to suppress mechanical resonance
    float filtered_iq = pid->enable_notch ? BiquadNotch_Update(&pid->notch_filter, raw_iq_cmd) : raw_iq_cmd;

    // 4. Apply Inverter Thermal Derating
    float derate_scale = Compute_Thermal_Derating_Factor(inverter_temp_c);
    float effective_max_current = pid->max_current_a * derate_scale;

    return clampf(filtered_iq, -effective_max_current, effective_max_current);
}
