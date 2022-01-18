#include "maze-exceptions.h"
const char* GenerationCompleteException::what() const throw() {
  return "Maze has been generated, cannot generate further.";
}
const char* GenerationIncompleteException::what() const throw() {
  return "Maze has not been generated, cannot solve it yet.";
}
const char* AlreadySolvedException::what() const throw() {
  return "Maze has been solved, cannot solve further.";
}
