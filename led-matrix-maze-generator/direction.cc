#include "direction.h"

DirectionException::DirectionException(const char* msg) {
  this->msg = msg;
}

const char* DirectionException::what() const throw () {
  return this->msg;
}

 direction getOppositeDir(direction dir) {
  if (dir == Left) {
    return Right;
  }
  else if (dir == Right) {
    return Left;
  }
  else if (dir == Up) {
    return Down;
  }
  else if (dir == Down) {
    return Up;
  }
  else {
    throw DirectionException("A direction that is not defined was attempted for use.");
  }
}
