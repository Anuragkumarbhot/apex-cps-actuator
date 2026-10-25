#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../include/biquad_filter.h"

int main(void) {
    BiquadFilter_t notch;
    BiquadNotch_Init(&notch, 250.0f, 10000.0f, 4.0f);
    float rms_in = 0.0f, rms_err = 0.0f;
    int n_samples = 0;
    for (int n = 0; n < 5000; n++) {
        float t = (float)n / 10000.0f;
        float clean = sinf(2.0f * M_PI * 10.0f * t);
        float res = sinf(2.0f * M_PI * 250.0f * t);
        float out = BiquadNotch_Update(&notch, clean + res);
        if (n >= 1000) {
            rms_in += res * res;
            rms_err += (out - clean) * (out - clean);
            n_samples++;
        }
    }
    float ratio = sqrtf(rms_err / n_samples) / sqrtf(rms_in / n_samples);
    printf("[PASS] Biquad 250Hz Attenuation: %.1f%%\n", (1.0f - ratio) * 100.0f);
    assert(ratio < 0.15f);
    return 0;
}
