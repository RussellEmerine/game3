#include "TriangleWave.hpp"

// Entries per second
constexpr size_t EPS = 48000;
// Number of indices
constexpr size_t SIZE = (size_t) (TriangleWave::LENGTH * EPS);

TriangleWave::TriangleWave(float frequency) : sample(std::vector<float>(SIZE)) {
    set_frequency(frequency);
}

TriangleWave::~TriangleWave() {
    stop();
}

void TriangleWave::play(float volume, float pan) {
    playing_sample = Sound::play(sample, volume, pan);
}

void TriangleWave::stop(float ramp) const {
    if (playing_sample) {
        playing_sample->stop(ramp);
    }
}

void TriangleWave::set_frequency(float frequency) {
    float wavelength = (float) EPS / frequency;
    for (size_t i = 0; i < EPS; i++) {
        float vol = std::min({(float) (4 * i) / EPS, (float) (4 * (EPS - i)) / EPS, 1.0f});
        float where = std::fmod((float) i, wavelength);
        if (where < wavelength / 2) {
            sample.data[i] = vol * 2 * where / wavelength;
        } else {
            sample.data[i] = vol * 2 * (wavelength - where) / wavelength;
        }
    }
}

TriangleWave::TriangleWave() : TriangleWave(440) {}
