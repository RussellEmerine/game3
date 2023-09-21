#pragma once

#include "Sound.hpp"

#include <vector>
#include <filesystem>
#include <iostream>

// Enum to represent whether a cell has borders to its top and left.
enum class Cell {
    None,
    Horiz,
    Vert,
    Corner,
};

bool has_bottom(Cell cell);

bool has_left(Cell cell);

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

struct FrequencyAxis {
    float min, max, start, goal;
    
    FrequencyAxis();
    
    /*
     * Interpolates the frequency given coordinate x, which represents the fraction of the way from min to max.
     * 0 <= x <= 1 should be true.
     */
    float get_frequency(float x) const;
    
    /*
     * Reverts a frequency to a position, represented as a fraction of the way from min to max.
     * min <= f <= max should be true.
     */
    float get_position(float f) const;
};

/*
 * A level structure. Just stores starting info, should not be modified after creation.
 * Starts at the bottom left corner.
 *
 * TODO: add frequency bounds, beep rate and background sound
 */
struct Level {
    FrequencyAxis x, y;
    float bpm;
    
    std::vector<std::vector<Cell>> cells;
    
    Sound::Sample background;
    
    explicit Level(std::filesystem::path const &dir);
    
    bool has_border(size_t row, size_t col, Direction direction);
    
    size_t width() const;
    
    size_t height() const;
};
