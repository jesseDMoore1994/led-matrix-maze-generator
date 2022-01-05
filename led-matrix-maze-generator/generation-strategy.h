#ifndef GENERATION_STRATEGY_H
#define GERERATION_STRATEGY_H

#include "graphics.h"
#include "grid.h"

class GenerationStrategy {
 protected:
  Grid *g;

 public:
  GenerationStrategy(Grid *g);
  virtual std::vector<std::vector<rgb_matrix::Color> > step() = 0;
  virtual std::vector<std::vector<rgb_matrix::Color> > step_all() = 0;
};

#endif
