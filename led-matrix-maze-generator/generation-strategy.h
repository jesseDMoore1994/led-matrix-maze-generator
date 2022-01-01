#ifndef GENERATION_STRATEGY_H
#define GERERATION_STRATEGY_H

#include "grid.h"

class GenerationStrategy {
  protected:
    Grid* g;

  public:
    GenerationStrategy(Grid* g);
    virtual void generate() = 0;
};

#endif
