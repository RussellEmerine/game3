#include "Level.hpp"
#include "read_write_chunk.hpp"

#include <cassert>

bool has_bottom(Cell cell) {
    return cell == Cell::Corner || cell == Cell::Horiz;
}

bool has_left(Cell cell) {
    return cell == Cell::Corner || cell == Cell::Vert;
}

bool Level::has_border(size_t row, size_t col, Direction direction) {
    assert(row < height && col < width && "invalid coordinates into level");
    switch (direction) {
        case Direction::Up:
            return row + 1 == height || has_bottom(cells[(row + 1) * width + col]);
        case Direction::Down:
            return has_bottom(cells[row * width + col]);
        case Direction::Left:
            return has_left(cells[row * width + col]);
        case Direction::Right:
            return col + 1 == width || has_left(cells[row * width + col + 1]);
        default:
            return false;
    }
}

Level::Level(std::vector<std::vector<Cell>> cells_,
             size_t start_row, size_t start_col,
             size_t goal_row, size_t goal_col)
        : start_row(start_row), start_col(start_col), goal_row(goal_row), goal_col(goal_col) {
    height = cells_.size();
    assert(!cells_.empty() && "cells empty");
    width = cells_[0].size();
    cells.resize(width * height);
    for (size_t row = 0; row < height; row++) {
        assert(cells_[row].size() == width && "cells row had wrong width");
        for (size_t col = 0; col < width; col++) {
            cells[row * width + col] = cells_[row][col];
        }
    }
    assert(start_row < height && "invalid start row");
    assert(start_col < height && "invalid start col");
    assert(goal_row < height && "invalid goal row");
    assert(goal_col < height && "invalid goal col");
}

Level::Level(std::istream in) {
    std::vector<size_t> meta;
    read_chunk(in, "lvlm", &meta);
    assert(meta.size() == 6 && "incorrect level metadata");
    width = meta[0];
    height = meta[1];
    start_row = meta[2];
    start_col = meta[3];
    goal_row = meta[4];
    goal_col = meta[5];
    read_chunk(in, "cell", &cells);
    assert(cells.size() == width * height && "cells had wrong size");
    assert(start_row < height && "invalid start row");
    assert(start_col < height && "invalid start col");
    assert(goal_row < height && "invalid goal row");
    assert(goal_col < height && "invalid goal col");
}

Cell Level::get(size_t row, size_t col) {
    return cells[row * width + col];
}
