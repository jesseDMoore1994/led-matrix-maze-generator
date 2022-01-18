#ifndef DEPTH_FIRST_SEARCH_H
#define DEPTH_FIRST_SEARCH_H
#include <exception>
#include <random>
#include <stack>

#include "cell.h"
#include "grid.h"
#include "maze-exceptions.h"
struct BacktrackException : public std::exception {
  const char* what() const throw() {
    return "Cannot backtrack further, the path is current path is empty.";
  }
};

template <typename T = Cell>
struct DepthFirstSearchStrategy {
  Grid<T>* g;
  std::stack<unsigned int> current_path;
  unsigned int starting_cell, current_cell, goal_cell;
  unsigned int init_starting_cell();
  unsigned int init_goal_cell();
  bool goal_found = false;
  DepthFirstSearchStrategy(Grid<T>* grid)
      : g(grid),
        current_path(std::stack<unsigned int>()),
        starting_cell(init_starting_cell()),
        current_cell(this->starting_cell),
        goal_cell(init_goal_cell()){};
  float dfs();
  float step();
};
#include "depth-first-search_impl.h"
#endif
