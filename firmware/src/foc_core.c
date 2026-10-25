#include "foc_core.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline float FOC_PI_Update(FOC_PI_t *pi, float setpoint, float feedback, float dt) {
    float error = setpoint - feedback;
    float p_term = pi->kp * error;
    float new_integral = pi->integral + (pi->ki * error * dt);
    float output = p_term + new_integral;
    if (output > pi->out_max) output = pi->out_max;
    else if (output < pi->out_min) output = pi->out_min;
    else pi->integral = new_integral;
    return output;
}

void FOC_Init(FOC_Instance_t *inst, uint8_t pole_pairs, float kt, float v_dc, uint32_t pwm_arr) {
    inst->pole_pairs = pole_pairs;
    inst->torque_constant_kt = kt;
    inst->v_dc_bus = v_dc;
    inst->pwm_period_ticks = (float)pwm_arr;
    inst->pi_d.kp = 3.5f; inst->pi_d.ki = 1800.0f; inst->pi_d.integral = 0.0f;
    inst->pi_d.out_max = (v_dc * ONE_BY_SQRT3);
    inst->pi_d.out_min = -(v_dc * ONE_BY_SQRT3);
    inst->pi_q = inst->pi_d;
}

void FOC_UpdateElectricalAngle(FOC_Instance_t *inst, float mech_angle_rad, float offset_rad) {
    float elec = fmodf(((float)inst->pole_pairs * mech_angle_rad) - offset_rad, 2.0f * (float)M_PI);
    if (elec < 0.0f) elec += (2.0f * (float)M_PI);
    inst->theta_elec_rad = elec;
}

void FOC_ExecuteCurrentLoop(FOC_Instance_t *inst, float target_torque_nm, float dt) {
    inst->i_alpha = inst->ia_meas_a;
    inst->i_beta  = (inst->ia_meas_a * ONE_BY_SQRT3) + (inst->ib_meas_a * TWO_BY_SQRT3);
    float s = sinf(inst->theta_elec_rad), c = cosf(inst->theta_elec_rad);
    inst->i_d =  (inst->i_alpha * c) + (inst->i_beta * s);
    inst->i_q = -(inst->i_alpha * s) + (inst->i_beta * c);
    inst->v_d = FOC_PI_Update(&inst->pi_d, 0.0f, inst->i_d, dt);
    inst->v_q = FOC_PI_Update(&inst->pi_q, target_torque_nm / inst->torque_constant_kt, inst->i_q, dt);
    inst->v_alpha = (inst->v_d * c) - (inst->v_q * s);
    inst->v_beta  = (inst->v_d * s) + (inst->v_q * c);
}
