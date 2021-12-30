#ifndef MAZE_H
#define MAZE_H

#include "grid.h"

class Maze {
  private:
    Grid grid;

  public:
    Maze(rgb_matrix::Canvas* canvas, unsigned int height, unsigned int width);
    void generate();
};

#endif
