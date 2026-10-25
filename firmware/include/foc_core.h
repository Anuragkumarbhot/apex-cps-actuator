#ifndef FOC_CORE_H
#define FOC_CORE_H
#include <stdint.h>
#include <stdbool.h>

#define ONE_BY_SQRT3   (0.57735026919f)
#define TWO_BY_SQRT3   (1.15470053838f)
#define SQRT3_BY_TWO   (0.86602540378f)

typedef struct {
    float kp, ki, integral, out_max, out_min;
} FOC_PI_t;

typedef struct {
    uint8_t  pole_pairs;
    float    torque_constant_kt, v_dc_bus, pwm_period_ticks, theta_elec_rad;
    float    ia_meas_a, ib_meas_a, i_alpha, i_beta, i_d, i_q;
    FOC_PI_t pi_d, pi_q;
    float    v_d, v_q, v_alpha, v_beta;
    uint32_t ccr_u, ccr_v, ccr_w;
} FOC_Instance_t;

void FOC_Init(FOC_Instance_t *inst, uint8_t pole_pairs, float kt, float v_dc, uint32_t pwm_arr);
void FOC_UpdateElectricalAngle(FOC_Instance_t *inst, float mech_angle_rad, float offset_rad);
void FOC_ExecuteCurrentLoop(FOC_Instance_t *inst, float target_torque_nm, float dt);
#endif
