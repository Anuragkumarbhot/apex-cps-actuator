#include "../include/pid_cascade.h"
#include <math.h>

static inline float clampf(float v, float lo, float hi) {
    return (v > hi) ? hi : ((v < lo) ? lo : v);
}

void CascadePID_Init(CascadePID_t *pid, float kp_pos, float kp_vel, float ki_vel, float max_vel, float max_cur) {
    pid->kp_pos = kp_pos; pid->kp_vel = kp_vel; pid->ki_vel = ki_vel;
    pid->integral_vel = 0.0f; pid->max_vel_rad_s = max_vel; pid->max_current_a = max_cur;
    pid->gravity_ff_gain = 1.85f; pid->enable_notch = 0;
}

void CascadePID_EnableNotch(CascadePID_t *pid, float f_center_hz, float f_sample_hz, float q_factor) {
    BiquadNotch_Init(&pid->notch_filter, f_center_hz, f_sample_hz, q_factor);
    pid->enable_notch = 1;
}

float Compute_Thermal_Derating_Factor(float temp_c) {
    if (temp_c <= THERMAL_DERATE_START_C) return 1.0f;
    if (temp_c >= THERMAL_DERATE_MAX_C) return 0.0f;
    return (THERMAL_DERATE_MAX_C - temp_c) / (THERMAL_DERATE_MAX_C - THERMAL_DERATE_START_C);
}

float CascadePID_Update(CascadePID_t *pid, float target_pos, float actual_pos, float actual_vel, float pitch_angle_rad, float inverter_temp_c, float dt) {
    float target_vel = clampf(pid->kp_pos * (target_pos - actual_pos), -pid->max_vel_rad_s, pid->max_vel_rad_s);
    float vel_err = target_vel - actual_vel;
    pid->integral_vel += vel_err * dt;
    float max_i = pid->max_current_a / (pid->ki_vel > 0.001f ? pid->ki_vel : 1.0f);
    pid->integral_vel = clampf(pid->integral_vel, -max_i, max_i);

    float raw_iq = (pid->kp_vel * vel_err) + (pid->ki_vel * pid->integral_vel) + (pid->gravity_ff_gain * cosf(pitch_angle_rad));
    float filt_iq = pid->enable_notch ? BiquadNotch_Update(&pid->notch_filter, raw_iq) : raw_iq;
    float eff_max = pid->max_current_a * Compute_Thermal_Derating_Factor(inverter_temp_c);
    return clampf(filt_iq, -eff_max, eff_max);
}
