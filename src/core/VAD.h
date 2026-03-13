#pragma once

// Energy VAD
inline bool IsSpeech(const float *samples, int n)
{
    if (n == 0)
        return false;

    float energy = 0.0f;
    for (int i = 0; i < n; i++)
        energy += samples[i] * samples[i];

    energy /= n;

    return energy > 0.0005f;
}