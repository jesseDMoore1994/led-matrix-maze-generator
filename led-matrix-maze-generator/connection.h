#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>

class Cell;
#include "cell.h"
#include "graphics.h"

/* -- GRID DEFINITION -- */
class Connection {
 private:
  /* Member Variables */
  // The cells on either side of the connection.
  std::shared_ptr<Cell> a;
  std::shared_ptr<Cell> b;
  rgb_matrix::Color color = rgb_matrix::Color(0, 255, 0);

 public:
  /* Public Function Definitions */
  // Constructor for the Connection has two cells.
  Connection(std::shared_ptr<Cell> a, std::shared_ptr<Cell> b);
  rgb_matrix::Color getColor();
  std::shared_ptr<Cell> other(std::shared_ptr<Cell> origin);
  std::shared_ptr<Cell> getA();
  std::shared_ptr<Cell> getB();
};

#endif
