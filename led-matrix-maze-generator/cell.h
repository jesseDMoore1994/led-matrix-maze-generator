#ifndef CELL_H
#define CELL_H

#include <set>
#include "led-matrix.h"
#include "direction.h"

/* -- USER DEFINED EXCEPTIONS --*/
struct CellException : public std::exception
{
  const char* msg;
  CellException(const char* msg);
  const char* what () const throw ();
};

/*-- CELL DEFINITION --*/
class Cell {

  private:

    /* Member Variables */
    // x and y position of the cell (This is the center of the cell
    unsigned int matrix_x_position;
    unsigned int matrix_y_position;

    // The grid row and column position of the cell.
    unsigned int grid_row;
    unsigned int grid_column;

    // r, g, and b are the red, green, and blue color components of the cell
    unsigned int red;
    unsigned int green;
    unsigned int blue;

    // set to true if color override is in effect
    bool color_override;

    // this array of pairs holds a reference to the neighboring cell in the
    // first item and a boolean that represents if the neighbor is connected
    // or if the neighbor is not connected.
    std::pair<Cell*, bool> neighbors[MAX_NUMBER_OF_NEIGHBORS];

    // this value is used to indicate if the cell in question has been visited
    // by a generation algorithm,
    bool visited;

    /* Private Function Definitions */
    // setColor is an internal function used to set the color of the cell based
    // on the current state of the cell. If the cell is connected to at least
    // one other cell, it will turn green. By default, cells are white.
    // If color override is in effect, this function takes no action.
    void setColor();

    // function that returns true if a neighbor exists in a certain
    // direction, false if not.
    bool neighborExists(direction dir);

    // function that generates an exception if a neighbor does not
    // exist in the specified direction.
    void assertNeighborExists(direction dir);

    // function that generates an exception if a neighbor exists in
    // the specified direction.
    void assertNeighborDoesNotExist(direction dir);

  public:

    /* Public Function Definitions */
    // Constructor for a cell using the x, y position and the grid row, column position
    Cell(
      unsigned int matrix_x_position,
      unsigned int matrix_y_position,
      unsigned int grid_row,
      unsigned int grid_column
    );

    // returns the row position of the cell in the grid
    unsigned int get_grid_row();

    // returns the column position of the cell in the grid
    unsigned int get_grid_column();

    // enforce a color override with the given r, g, b values
    void colorEnforce(unsigned int r, unsigned int g, unsigned int b);

    // turn off color enforcement and let cell decide its color again
    void noColorEnforce();

    // This function takes a reference to a cell and it to the neighbors array at the
    // specified direction.
    void addNeighbor(direction dir, Cell *neighbor);

    // This function gets the reference to a neighboring cell if it exits and throws
    // an error if no neighbor exits in that direction.
    Cell getNeighbor(direction dir);

    // This function "turns on" a connection between two cells by setting the boolean
    // for the for the neighboring cell to true and then signaling the other cell to
    // do the same for the direction of this cell if it has not been done already.
    void connectNeighbor(direction dir);

    // This function returns the state of the connection between the current cell and
    // the neighbor at the specified direction.
    bool isNeighborConnected(direction dir);

    // This function sets that the cell has been visited.
    void visit();

    // This function returns the visitation status of the this cell.
    bool isVisited();

    // Get set of valid directions that point to an actual neighbor.
    std::set<direction> getValidDirections();

    // This function returns the visitation status of the neighboring cell at a
    // given direction.
    bool neighborIsVisitedStatus(direction dir, bool visited_status);

    // Get set of valid directions to travel from here that match the provided
    // visitation status.
    std::set<direction> getValidDirectionsWithVisitedStatus(bool visited_status);

    // This function returns true if the cell has neighbors meeting the provided
    // visitation status.
    bool hasNeighborsWithVisitedStatus(bool visited_status);

    // Get set of valid directions to travel from here that have the provided
    // visitation status.
    direction getRandomDirectionWithVisitedStatus(bool visited_status);

    // In order to draw the cell, we make sure to set the cell core on, then we draw
    // a connection to each of the neighboring cells that are connected.
    void draw(rgb_matrix::Canvas* canvas);
};

#endif
