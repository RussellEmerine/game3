#include "TriangleWave.hpp"

#include <algorithm>

// Entries per second
constexpr size_t EPS = 48000;

size_t TriangleWave::size() const {
    return sample.data.size();
}

TriangleWave::TriangleWave(float frequency, float bpm) : sample(std::vector<float>()) {
    length = 60.0f / bpm;
    sample.data.resize((size_t) (length * (float) EPS));
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
    for (size_t i = 0; i < size(); i++) {
        // manual ramp for a softer fade in/out
        float denom = (float) std::min(EPS, size());
        float vol = std::min({(float) (4 * i) / denom, (float) (4 * (size() - i)) / denom, 1.0f});
        float where = std::fmod((float) i, wavelength);
        if (where < wavelength / 2) {
            sample.data[i] = vol * 2 * where / wavelength;
        } else {
            sample.data[i] = vol * 2 * (wavelength - where) / wavelength;
        }
    }
}

TriangleWave::TriangleWave() : TriangleWave(440, 120) {}
