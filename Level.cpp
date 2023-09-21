#include "Level.hpp"
#include "read_write_chunk.hpp"
#include "load_save_png.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <cmath>

bool has_bottom(Cell cell) {
    return cell == Cell::Corner || cell == Cell::Horiz;
}

bool has_left(Cell cell) {
    return cell == Cell::Corner || cell == Cell::Vert;
}

FrequencyAxis::FrequencyAxis() : min(0), max(0), start(0), goal(0) {};

float FrequencyAxis::get_frequency(float x) const {
    assert(0.0f <= x && x <= 1.0f && "Out of bounds fraction given to FrequencyAxis");
    return std::exp2f(std::log2f(min) + (std::log2f(max) - std::log2f(min)) * x);
}

float FrequencyAxis::get_position(float f) const {
    assert(min <= f && f <= max && "Out of bounds frequency given to FrequencyAxis");
    return (std::log2f(f) - std::log2f(min)) / (std::log2f(max) - std::log2f(min));
}

bool Level::has_border(size_t row, size_t col, Direction direction) {
    assert(row < height() && col < width() && "invalid coordinates into level");
    switch (direction) {
        case Direction::Up:
            return row + 1 == height() || has_bottom(cells[row + 1][col]);
        case Direction::Down:
            return has_bottom(cells[row][col]);
        case Direction::Left:
            return has_left(cells[row][col]);
        case Direction::Right:
            return col + 1 == width() || has_left(cells[row][col + 1]);
        default:
            return false;
    }
}

size_t Level::width() const {
    return cells[0].size();
}

size_t Level::height() const {
    return cells.size();
}

Level::Level(std::filesystem::path const &dir) : background((dir / "background.wav").string()) {
    // sound loaded above
    
    // load floats from chunk format
    std::ifstream in(dir / "meta.chunk", std::ios::binary);
    std::vector<float> meta;
    read_chunk(in, "meta", &meta);
    assert(meta.size() == 9 && "wrong amount of floats in the chunk");
    x.min = meta[0];
    x.max = meta[1];
    x.start = meta[2];
    x.goal = meta[3];
    y.min = meta[4];
    y.max = meta[5];
    y.start = meta[6];
    y.goal = meta[7];
    bpm = meta[8];
    
    // load cells from png format
    glm::uvec2 size;
    std::vector<glm::u8vec4> data;
    load_png((dir / "cells.png").string(), &size, &data, LowerLeftOrigin);
    cells.resize(size.y);
    for (size_t row = 0; row < size.y; row++) {
        cells[row].resize(size.x);
        for (size_t col = 0; col < size.x; col++) {
            auto color = data[row * size.x + col];
            // green is corner, red is horiz, blue is vert, black is none
            if (color.g) {
                cells[row][col] = Cell::Corner;
            } else if (color.r) {
                cells[row][col] = Cell::Horiz;
            } else if (color.b) {
                cells[row][col] = Cell::Vert;
            } else {
                cells[row][col] = Cell::None;
            }
        }
    }
}
