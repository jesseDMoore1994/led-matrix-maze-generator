#ifndef DIRECTION_H
#define DIRECTION_H

#include <exception>
#include <iostream>

#define MAX_NUMBER_OF_NEIGHBORS 4
struct DirectionException : public std::exception {
  const char *msg;
  DirectionException(const char *msg);
  const char *what() const throw();
};
enum direction { Left, Up, Right, Down };
constexpr std::initializer_list<direction> all_directions = {Left, Up, Right,
                                                             Down};

// define a helper function to get the opposite direction
direction getOppositeDir(direction dir);

#endif
