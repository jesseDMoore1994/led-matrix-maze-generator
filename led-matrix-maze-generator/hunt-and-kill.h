#ifndef HUNT_AND_KILL_H
#define HUNT_AND_KILL_H
#include <exception>
#include <random>

#include "cell.h"
#include "grid.h"
#include "maze-exceptions.h"
struct CantWalkException : public std::exception {
  const char* what() const throw();
};

struct HuntFailedException : public std::exception {
  const char* what() const throw();
};

template <typename T = Cell>
struct HuntAndKillStrategy {
  Grid<T>* g;
  unsigned int current_cell;
  unsigned int init_current_cell();
  HuntAndKillStrategy(Grid<T>* grid)
      : g(grid), current_cell(init_current_cell()){};
  void walk();
  void hunt();
  float step();
};
#include "hunt-and-kill_impl.h"
#endif
