#ifndef MAZE_EXCEPTIONS_H
#define MAZE_EXCEPTIONS_H
#include <exception>
struct GenerationCompleteException : public std::exception {
  const char* what() const throw();
};
struct GenerationIncompleteException : public std::exception {
  const char* what() const throw();
};
struct AlreadySolvedException : public std::exception {
  const char* what() const throw();
};
#endif
