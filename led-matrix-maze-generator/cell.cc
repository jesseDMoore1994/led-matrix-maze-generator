#include "cell.h"

/* -- USER DEFINED EXCEPTIONS --*/
CellException::CellException(const char* msg) { this->msg = msg; }

const char* CellException::what() const throw() { return this->msg; }

ConnectionEntry::ConnectionEntry(std::shared_ptr<Connection> conn, direction dir)
    : connection(conn), dir(dir) {}

/*-- CELL DEFINITION --*/
void Cell::setColor() {
  if (this->color_override) {
    return;
  }

  // lambda function to check if a neighbor is connected
  auto isConnected = [&]() { return (this->connections.size() != 0); };

  // lambda function to set the color of the cell to white
  auto setWhite = [&]() {
    this->color.r = 255;
    this->color.g = 255;
    this->color.b = 255;
  };

  // lambda function to set the color of the cell to green
  auto setGreen = [&]() {
    this->color.r = 0;
    this->color.g = 255;
    this->color.b = 0;
  };

  // if the cell has a connection, set green, otherwise set white.
  if (isConnected()) {
    setGreen();
  } else {
    setWhite();
  }
}

/* Public Function Definitions */
// Constructor for a cell using the x, y position and the grid row, column
// position
Cell::Cell(unsigned int x_position, unsigned int y_position,
           unsigned int grid_row, unsigned int grid_column) {
  this->x_position = x_position;
  this->y_position = y_position;
  this->grid_row = grid_row;
  this->grid_column = grid_column;
  this->color = rgb_matrix::Color();
  this->visited = false;
  this->color_override = false;
}

// return the pixel position of the cell in the x coordinate
unsigned int Cell::get_x_position() const { return this->x_position; }

// return the pixel position of the cell in the y coordinate
unsigned int Cell::get_y_position() const { return this->y_position; }

// returns the row position of the cell in the grid
unsigned int Cell::get_grid_row() const { return this->grid_row; }

// returns the column position of the cell in the grid
unsigned int Cell::get_grid_column() const { return this->grid_column; }

// enforce a color override with the given r, g, b values
void Cell::colorEnforce(rgb_matrix::Color c) {
  this->color = c;
  this->color_override = true;
}

// turn off color enforcement and let cell decide its color again
void Cell::noColorEnforce() { this->color_override = false; }

// turn off color enforcement and let cell decide its color again
rgb_matrix::Color Cell::getColor() {
  this->setColor();
  return this->color;
}

// turn off color enforcement and let cell decide its color again
bool Cell::getColorEnforce() { return this->color_override; }

// This function takes a reference to a connection and a direction
// and adds it to the vector of neighbors
void Cell::addConnection(direction dir, const Connection& conn) {
  if (this->connectionExists(dir)) {
    throw CellException("A Connection is already added at this location.");
  }

  this->connections.push_back(ConnectionEntry(conn, dir));
}

// function that returns true if a neighbor exists in a certain
// direction, false if not.
bool Cell::connectionExists(direction dir) {
  for (const auto& connection : this->connections) {
    if (connection.dir == dir) {
      return true;
    }
  }
  return false;
}

// This function sets that the cell has been visited.
void Cell::visit() { this->visited = true; }

// This function returns the visitation status of the this cell.
bool Cell::isVisited() { return this->visited; }

bool Cell::operator==(const Cell& rhs) const {
  return (this->get_grid_row() == rhs.get_grid_row() &&
          this->get_grid_column() == rhs.get_grid_column() &&
          this->get_x_position() == rhs.get_x_position() &&
          this->get_y_position() == rhs.get_y_position());
}

bool Cell::operator!=(const Cell& rhs) const { return !(*this == rhs); }
