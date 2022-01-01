#include "maze.h"
#include "hunt-and-kill.h"
#include <exception>
#include <time.h>
#include <unistd.h>

Maze::Maze(Grid* grid, GenerationStrategy* strategy) {
  this->grid = grid;
  this->strategy =strategy;
}

void Maze::generate() {
  this->strategy->generate();
}
