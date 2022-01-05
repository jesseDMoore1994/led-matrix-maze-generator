#ifndef HUNT_AND_KILL_H
#define HUNT_AND_KILL_H

#include <exception>
#include <memory>

#include "graphics.h"
#include "maze.h"

struct WalkNotSuccessful : public std::exception {
  const char *what() const throw();
};

struct HuntNotSuccessful : public std::exception {
  const char *what() const throw();
};

class HuntAndKillStrategy : public GenerationStrategy {
 private:
  std::shared_ptr<Cell> current_cell;

 public:
  HuntAndKillStrategy(Grid *g)
      : GenerationStrategy::GenerationStrategy(g),
        current_cell(g->getRandomCell()) {}
  std::vector<std::vector<rgb_matrix::Color> > walk();
  std::vector<std::vector<rgb_matrix::Color> > hunt();
  virtual std::vector<std::vector<rgb_matrix::Color> > step();
  virtual std::vector<std::vector<rgb_matrix::Color> > step_all();
};

#endif
