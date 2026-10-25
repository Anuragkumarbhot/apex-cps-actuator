/**
 * @file linear_profile.h
 * @brief Linear Interpolation (LERP) and Linear Ramp Trajectory Profiler
 * Used for constant-velocity segments and real-time setpoint smoothing.
 */

#ifndef LINEAR_PROFILE_H
#define LINEAR_PROFILE_H

#include <math.h>

typedef struct {
    float start_pos;
    float target_pos;
    float current_pos;
    float speed;        // rad/s
    float duration;     // total time in seconds
    float elapsed_time; // current elapsed time
    int active;
} LinearProfile_t;

static inline void Linear_Init(LinearProfile_t *p, float start, float target, float speed) {
    p->start_pos = start;
    p->target_pos = target;
    p->current_pos = start;
    p->speed = (speed > 0.1f) ? speed : 1.0f;
    float distance = fabsf(target - start);
    p->duration = distance / p->speed;
    p->elapsed_time = 0.0f;
    p->active = 1;
}

/**
 * @brief Linear Interpolation (LERP) utility function
 */
static inline float Linear_Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

/**
 * @brief Step the linear trajectory profile by dt seconds
 */
static inline float Linear_Step(LinearProfile_t *p, float dt) {
    if (!p->active) return p->current_pos;

    p->elapsed_time += dt;
    if (p->elapsed_time >= p->duration) {
        p->current_pos = p->target_pos;
        p->active = 0;
        return p->current_pos;
    }

    float t_ratio = p->elapsed_time / p->duration;
    p->current_pos = Linear_Lerp(p->start_pos, p->target_pos, t_ratio);
    return p->current_pos;
}

#endif /* LINEAR_PROFILE_H */
