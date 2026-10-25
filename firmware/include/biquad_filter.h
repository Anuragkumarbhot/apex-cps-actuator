#ifndef BIQUAD_FILTER_H
#define BIQUAD_FILTER_H
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

typedef struct {
    float b0, b1, b2, a1, a2;
    float x1, x2, y1, y2;
} BiquadFilter_t;

static inline void BiquadNotch_Init(BiquadFilter_t *f, float f_c, float f_s, float q) {
    float w0 = 2.0f * (float)M_PI * f_c / f_s;
    float alpha = sinf(w0) / (2.0f * q);
    float cos_w0 = cosf(w0);
    float a0 = 1.0f + alpha;
    f->b0 = 1.0f / a0;
    f->b1 = (-2.0f * cos_w0) / a0;
    f->b2 = 1.0f / a0;
    f->a1 = (-2.0f * cos_w0) / a0;
    f->a2 = (1.0f - alpha) / a0;
    f->x1 = 0.0f; f->x2 = 0.0f; f->y1 = 0.0f; f->y2 = 0.0f;
}

static inline float BiquadNotch_Update(BiquadFilter_t *f, float x) {
    float y = (f->b0 * x) + (f->b1 * f->x1) + (f->b2 * f->x2) - (f->a1 * f->y1) - (f->a2 * f->y2);
    f->x2 = f->x1; f->x1 = x;
    f->y2 = f->y1; f->y1 = y;
    return y;
}
#endif
