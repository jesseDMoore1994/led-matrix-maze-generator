#ifndef HUNT_AND_KILL_H
#define HUNT_AND_KILL_H
#include <exception>
#include <random>

#include "cell.h"
#include "grid.h"
#include "maze-exceptions.h"
struct CantWalkException : public std::exception {
  const char* what() const throw() {
    return "Cannot walk further, all connectable cells from here have been visited";
  }
};

struct HuntFailedException : public std::exception {
  const char* what() const throw() {
    return "No cell found in the hunt, must be finished with generation";
  };
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
