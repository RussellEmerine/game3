#pragma once

#include "Sound.hpp"

/*
 * An overcomplicated structure for real-time adjustable triangle waves.
 */
struct TriangleWave {
    // How long to play the sound, in seconds
    static constexpr float LENGTH = 1.0f;
    
    Sound::Sample sample;
    std::shared_ptr<Sound::PlayingSample> playing_sample;
    
    TriangleWave();
    
    explicit TriangleWave(float frequency);
    
    ~TriangleWave();
    
    void play(float volume = 1.0f, float pan = 0.0f);
    
    // const doesn't mean the whole thing's really const, it just means it only manipulates the other end of a pointer.
    void stop(float ramp = 1.0f / 60.0f) const;
    
    void set_frequency(float frequency);
};
