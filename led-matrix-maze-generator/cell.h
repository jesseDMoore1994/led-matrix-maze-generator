#ifndef CELL_H
#define CELL_H

#include <memory>
#include <set>
#include <vector>

#include "direction.h"

class Connection;
#include "connection.h"
#include "graphics.h"

/* -- USER DEFINED EXCEPTIONS --*/
struct CellException : public std::exception {
  const char* msg;
  CellException(const char* msg);
  const char* what() const throw();
};

struct ConnectionEntry {
  std::shared_ptr<Connection> connection;
  direction dir;
  ConnectionEntry(std::shared_ptr<Connection> conn, direction dir);
};

/*-- CELL DEFINITION --*/
class Cell {
 private:
  /* Member Variables */
  // x and y position of the cell (This is the center of the cell
  unsigned int x_position;
  unsigned int y_position;

  // The grid row and column position of the cell.
  unsigned int grid_row;
  unsigned int grid_column;

  // The color associated with the cell.
  rgb_matrix::Color color;

  // set to true if color override is in effect
  bool color_override;

  // this array of pairs holds a reference to the neighboring cell in the
  // first item and a boolean that represents if the neighbor is connected
  // or if the neighbor is not connected.
  std::vector<ConnectionEntry> connections;

  // this value is used to indicate if the cell in question has been visited
  // by a generation algorithm,
  bool visited;

  /* Private Function Definitions */
  // setColor is an internal function used to set the color of the cell based
  // on the current state of the cell. If the cell is connected to at least
  // one other cell, it will turn green. By default, cells are white.
  // If color override is in effect, this function takes no action.
  void setColor();

 public:
  /* Public Function Definitions */
  // Constructor for a cell using the x, y position and the grid row, column
  // position
  Cell(unsigned int matrix_x_position, unsigned int matrix_y_position,
       unsigned int grid_row, unsigned int grid_column);

  // returns the pixel position of the cell in the x coordinate
  unsigned int get_x_position() const;

  // returns the pixel position of the cell in the y coordinate
  unsigned int get_y_position() const;

  // returns the row position of the cell in the grid
  unsigned int get_grid_row() const;

  // returns the column position of the cell in the grid
  unsigned int get_grid_column() const;

  // enforce a color override with the given r, g, b values
  void colorEnforce(rgb_matrix::Color c);

  // turn off color enforcement and let cell decide its color again
  void noColorEnforce();

  // Query the state of color enforcement
  bool getColorEnforce();

  // Returns the color of the cell
  rgb_matrix::Color getColor();

  // This function takes a reference to a connection and a direction
  // and adds it to the vector of neighbors.
  void addConnection(direction dir, const Connection& conn);

  // function that returns true if a neighbor exists in a certain
  // direction, false if not.
  bool connectionExists(direction dir);

  // This function sets that the cell has been visited.
  void visit();

  // This function returns the visitation status of the this cell.
  bool isVisited();

  // Defines equality for cell types
  bool operator==(const Cell& rhs) const;

  // Defines equality for cell types
  bool operator!=(const Cell& rhs) const;
};

#endif
