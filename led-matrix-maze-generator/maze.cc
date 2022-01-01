#include "maze.h"

#include <time.h>
#include <unistd.h>

#include <exception>

#include "hunt-and-kill.h"

Maze::Maze(Grid *grid, GenerationStrategy *strategy) {
  this->grid = grid;
  this->strategy = strategy;
}

void Maze::generate() { this->strategy->generate(); }
