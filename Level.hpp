#pragma once

#include <vector>
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

/*
 * A level structure. Just stores starting info, should not be modified after creation.
 * Starts at the bottom left corner.
 */
struct Level {
    size_t width, height;
    size_t start_row, start_col;
    size_t goal_row, goal_col;
    
    std::vector<Cell> cells;
    
    explicit Level(std::vector<std::vector<Cell>> cells,
                   size_t start_row, size_t start_col,
                   size_t goal_row, size_t goal_col);
    
    explicit Level(std::istream in);
    
    bool has_border(size_t row, size_t col, Direction direction);
    
    Cell get(size_t row, size_t col);
};
