#include "maze-exceptions.h"
const char* GenerationCompleteException::what() const throw() {
  return "Maze has been generated, cannot generate further.";
}
