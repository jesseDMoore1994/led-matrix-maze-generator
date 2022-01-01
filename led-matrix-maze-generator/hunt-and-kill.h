#ifndef HUNT_AND_KILL_H
#define HUNT_AND_KILL_H

#include <exception>

#include "maze.h"

struct HuntNotSuccessful : public std::exception {
  const char *what() const throw();
};

class HuntAndKillStrategy : public GenerationStrategy {
 public:
  HuntAndKillStrategy(Grid *g) : GenerationStrategy(g) {}
  virtual void generate();
};

#endif
