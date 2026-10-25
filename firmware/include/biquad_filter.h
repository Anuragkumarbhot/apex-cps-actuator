/**
 * @file biquad_filter.h
 * @brief Biquad Notch Filter for suppressing mechanical resonance in Apex-CPS Actuator
 * @author Anuragkumarbhot (Anurag485bhot@gmail.com)
 */

#ifndef BIQUAD_FILTER_H
#define BIQUAD_FILTER_H

#include <math.h>

typedef struct {
    float b0, b1, b2;
    float a1, a2;
    float x1, x2;
    float y1, y2;
} BiquadFilter_t;

/**
 * @brief Initialize Biquad Notch Filter coefficients
 * @param f_c Center frequency of mechanical resonance to reject (Hz)
 * @param f_s Sampling frequency (Hz)
 * @param q_factor Quality factor (sharpness of the notch)
 */
static inline void BiquadNotch_Init(BiquadFilter_t *filt, float f_c, float f_s, float q_factor) {
    float w0 = 2.0f * (float)M_PI * f_c / f_s;
    float alpha = sinf(w0) / (2.0f * q_factor);
    float cos_w0 = cosf(w0);

    float a0 = 1.0f + alpha;
    filt->b0 = 1.0f / a0;
    filt->b1 = (-2.0f * cos_w0) / a0;
    filt->b2 = 1.0f / a0;
    filt->a1 = (-2.0f * cos_w0) / a0;
    filt->a2 = (1.0f - alpha) / a0;

    filt->x1 = 0.0f; filt->x2 = 0.0f;
    filt->y1 = 0.0f; filt->y2 = 0.0f;
}

/**
 * @brief Process a single sample through the notch filter
 */
static inline float BiquadNotch_Update(BiquadFilter_t *filt, float sample) {
    float output = (filt->b0 * sample) + (filt->b1 * filt->x1) + (filt->b2 * filt->x2)
                   - (filt->a1 * filt->y1) - (filt->a2 * filt->y2);

    filt->x2 = filt->x1;
    filt->x1 = sample;
    filt->y2 = filt->y1;
    filt->y1 = output;

    return output;
}

#endif /* BIQUAD_FILTER_H */
