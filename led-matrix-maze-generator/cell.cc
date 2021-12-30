#include "cell.h"

/* -- USER DEFINED EXCEPTIONS --*/
CellException::CellException(const char* msg) {
  this->msg = msg;
}

const char* CellException::what () const throw ()
{
  return this->msg;
}

/*-- CELL DEFINITION --*/
void Cell::setColor() {
  if (this->color_override) {
    return;
  }

  // lambda function to check if a neighbor is connected
  auto isConnected = [&] () {
    for(auto const& neighbor: this->neighbors) {
      if (neighbor.second) {
        return true;
      }
    }
    return false;
  };

  // lambda function to set the color of the cell to white
  auto setWhite = [&] () {
    this->red = 255;
    this->green = 255;
    this->blue = 255;
  };

  // lambda function to set the color of the cell to green
  auto setGreen = [&] () {
    this->red = 0;
    this->green = 255;
    this->blue = 0;
  };

  // if the cell has a connection, set green, otherwise set white.
  if (isConnected()) {setGreen();} else {setWhite();}
}

// function that returns true if a neighbor exists in a certain
// direction, false if not.
bool Cell::neighborExists(direction dir) {
  return this->neighbors[dir].first != nullptr;
}

// function that generates an exception if a neighbor does not
// exist in the specified direction.
void Cell::assertNeighborExists(direction dir) {
  if (!this->neighborExists(dir)) {
    throw CellException("There is no neigbor located in that direction.");
  }
}

// function that generates an exception if a neighbor exists in
// the specified direction.
void Cell::assertNeighborDoesNotExist(direction dir) {
  if (this->neighborExists(dir)) {
    throw CellException("A neighbor is already added at this location.");
  }
}

/* Public Function Definitions */
// Constructor for a cell using the x, y position and the grid row, column position
Cell::Cell(unsigned int matrix_x_position, unsigned int matrix_y_position, unsigned int grid_row, unsigned int grid_column) {
  this->matrix_x_position = matrix_x_position;
  this->matrix_y_position = matrix_y_position;
  this->grid_row = grid_row;
  this->grid_column = grid_column;
  for(auto & neighbor: this->neighbors) {
    neighbor = std::make_pair(nullptr, false);
  }
  this->visited = false;
  this->color_override = false;
}

// returns the row position of the cell in the grid
unsigned int Cell::get_grid_row() {
  return this->grid_row;
}

// returns the column position of the cell in the grid
unsigned int Cell::get_grid_column() {
  return this->grid_column;
}

// enforce a color override with the given r, g, b values
void Cell::colorEnforce(unsigned int r, unsigned int g, unsigned int b) {
  this->red = r;
  this->green = g;
  this->blue = b;
  this->color_override = true;
}

// turn off color enforcement and let cell decide its color again
void Cell::noColorEnforce() {
  this->color_override = false;
}

// This function takes a reference to a cell and it to the neighbors array at the
// specified direction.
void Cell::addNeighbor(direction dir, Cell *neighbor) {
  assertNeighborDoesNotExist(dir);
  this->neighbors[dir].first = neighbor;
}

// This function gets the reference to a neighboring cell if it exits and throws
// an error if no neighbor exits in that direction.
Cell Cell::getNeighbor(direction dir) {
  assertNeighborExists(dir);
  return *(this->neighbors[dir].first);
}

// This function "turns on" a connection between two cells by setting the boolean
// for the for the neighboring cell to true and then signaling the other cell to
// do the same for the direction of this cell if it has not been done already.
void Cell::connectNeighbor(direction dir) {
  assertNeighborExists(dir);
  this->neighbors[dir].second = true;

  if (!this->neighbors[dir].first->isNeighborConnected(getOppositeDir(dir))) {
    this->neighbors[dir].first->connectNeighbor(getOppositeDir(dir));
  }
}

// This function returns the state of the connection between the current cell and
// the neighbor at the specified direction.
bool Cell::isNeighborConnected(direction dir) {
  assertNeighborExists(dir);
  return this->neighbors[dir].second;
}

// This function sets that the cell has been visited.
void Cell::visit() {
  this->visited = true;
}

// This function returns the visitation status of the this cell.
bool Cell::isVisited() {
  return this->visited;
}

// Get set of valid directions that point to an actual neighbor.
std::set<direction> Cell::getValidDirections() {
  std::set<direction> valid_directions;

  for(auto & dir: all_directions) {
    if (this->neighbors[dir].first != nullptr ) {
      valid_directions.insert(dir);
    }
  }

  if (valid_directions.empty()) {
    throw CellException("No valid direction exists from here.");
  }

  return valid_directions;
}

// This function returns the visitation status of the neighboring cell at a
// given direction.
bool Cell::neighborIsVisitedStatus(direction dir, bool visited_status) {
  assertNeighborExists(dir);
  return this->neighbors[dir].first->isVisited() == visited_status;
}

// Get set of valid directions to travel from here that match the provided
// visitation status.
std::set<direction> Cell::getValidDirectionsWithVisitedStatus(bool visited_status) {
  std::set<direction> filtered_directions;
  std::set<direction> valid_directions = this->getValidDirections();

  for(auto i = valid_directions.begin(); i != valid_directions.end(); i++) {
    if (neighborIsVisitedStatus(*i, visited_status)) {
      filtered_directions.insert(*i);
    }
  }

  if (filtered_directions.empty()) {
    const char* errstr;
    if (visited_status) {
      errstr = "No visited direction exists from here.";
    }
    else {
      errstr = "No unvisited direction exists from here.";
    }
    throw CellException(errstr);
  }

  return filtered_directions;
}

// This function returns true if the cell has neighbors meeting the provided
// visitation status.
bool Cell::hasNeighborsWithVisitedStatus(bool visited_status) {
  try {
    auto directions = getValidDirectionsWithVisitedStatus(visited_status);
    return true;
  }
  catch (CellException& e) {
    return false;
  }
}

// Get set of valid directions to travel from here that have the provided
// visitation status..
direction Cell::getRandomDirectionWithVisitedStatus(bool visited_status) {
  auto directions = this->getValidDirectionsWithVisitedStatus(visited_status);
  unsigned int random_idx = rand() % directions.size();
  auto choice = std::begin(directions);
  std::advance(choice, random_idx);
  return *choice;
}

// In order to draw the cell, we make sure to set the cell core on, then we draw
// a connection to each of the neighboring cells that are connected.
void Cell::draw(rgb_matrix::Canvas* canvas) {
  setColor();
  canvas->SetPixel(this->matrix_x_position, this->matrix_y_position, this->red, this->green, this->blue);
  if (neighbors[Left].second) {
    canvas->SetPixel(this->matrix_x_position, this->matrix_y_position-1, this->red, this->green, this->blue);
  }
  if (neighbors[Up].second) {
    canvas->SetPixel(this->matrix_x_position+1, this->matrix_y_position, this->red, this->green, this->blue);
  }
  if (neighbors[Right].second) {
    canvas->SetPixel(this->matrix_x_position, this->matrix_y_position+1, this->red, this->green, this->blue);
  }
  if (neighbors[Down].second)  {
    canvas->SetPixel(this->matrix_x_position-1, this->matrix_y_position, this->red, this->green, this->blue);
  }
}
