#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../include/biquad_filter.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

int main(void) {
    printf("[Test] Running Biquad Notch Filter Verification...\n");

    BiquadFilter_t notch;
    float fs = 10000.0f;       // 10 kHz FOC loop frequency
    float f_notch = 250.0f;    // 250 Hz harmonic drive resonance frequency
    float q_factor = 4.0f;     // Sharpness of notch

    BiquadNotch_Init(&notch, f_notch, fs, q_factor);

    // Simulate 0.5 seconds of signal (5000 samples)
    // Signal = 1.0 * sin(10 Hz) + 1.0 * sin(250 Hz)
    float rms_in_250 = 0.0f;
    float rms_out_error = 0.0f;
    int steady_state_samples = 0;

    for (int n = 0; n < 5000; n++) {
        float t = (float)n / fs;
        float clean_cmd = sinf(2.0f * M_PI * 10.0f * t);
        float resonance = sinf(2.0f * M_PI * f_notch * t);
        float noisy_input = clean_cmd + resonance;

        float filtered_output = BiquadNotch_Update(&notch, noisy_input);

        // Evaluate after filter transient settles (after 1000 samples)
        if (n >= 1000) {
            rms_in_250 += resonance * resonance;
            float residual = filtered_output - clean_cmd;
            rms_out_error += residual * residual;
            steady_state_samples++;
        }
    }

    rms_in_250 = sqrtf(rms_in_250 / (float)steady_state_samples);
    rms_out_error = sqrtf(rms_out_error / (float)steady_state_samples);

    float attenuation_ratio = rms_out_error / rms_in_250;
    printf("  -> Input 250Hz Resonance RMS : %.4f\n", rms_in_250);
    printf("  -> Post-Filter Residual RMS  : %.4f\n", rms_out_error);
    printf("  -> Resonance Attenuation     : %.1f%%\n", (1.0f - attenuation_ratio) * 100.0f);

    // Verify at least 85% attenuation of the resonant frequency
    assert(attenuation_ratio < 0.15f);
    printf("[PASS] Biquad Notch Filter Verified Successfully!\n");

    return 0;
}
