#ifndef MAZE_H
#define MAZE_H

#include "generation-strategy.h"
#include "grid.h"

class Maze {
 private:
  Grid *grid;
  GenerationStrategy *strategy;

 public:
  Maze(Grid *grid, GenerationStrategy *strategy);
  void generate_step();
  void generate();
};

#endif
