#include "connection.h"

/* Public Function Definitions */
// Constructor for the grid contains a reference to two cells
Connection::Connection(std::shared_ptr<Cell> a, std::shared_ptr<Cell> b) : a(a), b(b) {
  // b
  // ^
  // |
  // v
  // a
  if (a->get_grid_row() < b->get_grid_row() &&
      a->get_grid_column() == b->get_grid_column()) {
    a->addConnection(Up, *this);
    b->addConnection(Down, *this);
  }
  // a
  // ^
  // |
  // v
  // b
  else if (a->get_grid_row() > b->get_grid_row() &&
           a->get_grid_column() == b->get_grid_column()) {
    a->addConnection(Down, *this);
    b->addConnection(Up, *this);
  }
  // a <----> b
  else if (a->get_grid_row() == b->get_grid_row() &&
           a->get_grid_column() < b->get_grid_column()) {
    a->addConnection(Right, *this);
    b->addConnection(Left, *this);
  }
  // b <----> a
  else if (a->get_grid_row() == b->get_grid_row() &&
           a->get_grid_column() > b->get_grid_column()) {
    a->addConnection(Left, *this);
    b->addConnection(Right, *this);
  } else {
    throw "Direction Error";
  }
}

rgb_matrix::Color Connection::getColor() { return this->color; }

std::shared_ptr<Cell> Connection::other(std::shared_ptr<Cell> origin) {
  if (origin == this->a) {
    return this->b;
  } else {
    return this->a;
  }
}

std::shared_ptr<Cell> Connection::getA() { return this->a; }

std::shared_ptr<Cell> Connection::getB() { return this->b; }
