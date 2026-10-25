/**
 * @file pid_cascade.h
 * @brief Dual-Loop Cascade PID with Biquad Notch Filter & Thermal Derating
 * @author Anuragkumarbhot (anurag485bhot@gmail.com)
 */

#ifndef PID_CASCADE_H
#define PID_CASCADE_H

#include "biquad_filter.h"

#define THERMAL_DERATE_START_C 75.0f
#define THERMAL_DERATE_MAX_C   95.0f

typedef struct {
    // Outer Position Loop Gains
    float kp_pos;
    float max_vel_rad_s;

    // Inner Velocity Loop Gains
    float kp_vel;
    float ki_vel;
    float integral_vel;
    float max_current_a;

    // Gravity Feedforward
    float gravity_ff_gain;

    // Resonance Suppression Filter
    BiquadFilter_t notch_filter;
    int enable_notch;
} CascadePID_t;

void CascadePID_Init(CascadePID_t *pid, float kp_pos, float kp_vel, float ki_vel, float max_vel, float max_cur);
void CascadePID_EnableNotch(CascadePID_t *pid, float f_center_hz, float f_sample_hz, float q_factor);
float Compute_Thermal_Derating_Factor(float inverter_temp_c);
float CascadePID_Update(CascadePID_t *pid, float target_pos, float actual_pos, float actual_vel, float pitch_angle_rad, float inverter_temp_c, float dt);

#endif /* PID_CASCADE_H */
