#ifndef MAZE_H
#define MAZE_H

#include "grid.h"
#include "generation-strategy.h"

class Maze {
  private:
    Grid* grid;
    GenerationStrategy* strategy;

  public:
    Maze(Grid* grid, GenerationStrategy* strategy);
    void generate();
};

#endif
