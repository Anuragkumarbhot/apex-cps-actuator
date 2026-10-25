/**
 * @file friction_comp.h
 * @brief LuGre & Coulomb-Stribeck Friction Compensator for Harmonic Drive Actuators
 * Compensates for static stiction and velocity-dependent viscous/Coulomb friction.
 */

#ifndef FRICTION_COMP_H
#define FRICTION_COMP_H

#include <math.h>

typedef struct {
    float coulomb_friction;   // Constant stiction/Coulomb offset (A)
    float viscous_friction;   // Velocity proportional damping (A/(rad/s))
    float stribeck_vel;       // Stribeck velocity threshold (rad/s)
    float static_friction;    // Breakaway friction peak (A)
} FrictionComp_t;

static inline void FrictionComp_Init(FrictionComp_t *fc, float coulomb, float viscous, float stribeck_v, float static_f) {
    fc->coulomb_friction = coulomb;
    fc->viscous_friction = viscous;
    fc->stribeck_vel = stribeck_v;
    fc->static_friction = static_f;
}

/**
 * @brief Compute feedforward current required to overcome mechanical friction
 */
static inline float FrictionComp_Update(FrictionComp_t *fc, float velocity_rad_s) {
    // Viscous friction component
    float viscous = fc->viscous_friction * velocity_rad_s;

    // Stribeck & Coulomb friction component with smooth sign function
    float v_ratio = velocity_rad_s / (fc->stribeck_vel > 1e-4f ? fc->stribeck_vel : 1e-4f);
    float stribeck_factor = expf(-(v_ratio * v_ratio));
    
    // Equivalent friction torque/current sign mapping
    float sign_v = (velocity_rad_s > 0.01f) ? 1.0f : ((velocity_rad_s < -0.01f) ? -1.0f : (velocity_rad_s / 0.01f));
    
    float stribeck_coulomb = (fc->coulomb_friction + (fc->static_friction - fc->coulomb_friction) * stribeck_factor) * sign_v;

    return viscous + stribeck_coulomb;
}

#endif /* FRICTION_COMP_H */
