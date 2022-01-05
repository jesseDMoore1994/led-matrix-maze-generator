#ifndef GRID_H
#define GRID_H

#include <memory>
#include <vector>

#include "cell.h"
#include "connection.h"
#include "graphics.h"

/* -- GRID DEFINITION -- */
class Grid {
 private:
  /* Member Variables */
  // 2D vector of shared pointers to Cells
  std::vector<std::vector<std::shared_ptr<Cell> > > cells;
  // vector of connections in the grid
  std::vector<Connection> connections;
  // the number of rows in the grid.
  unsigned int height;
  // the number of columns in the grid.
  unsigned int width;

  /* Private Function Definitions */
  // This function is used to create the cells that that compose
  // the grid.
  std::vector<std::vector<std::shared_ptr<Cell> > > createCells();

 public:
  /* Public Function Definitions */
  // Constructor for the grid creates the 2D vector of cells
  // and then populates the neighbors for each.
  Grid(unsigned int height, unsigned int width);

  // The is an accessor function to get all cells from the
  // grid for the purposes of scanning.
  std::vector<std::vector<std::shared_ptr<Cell> > > getCells();

  // The is an accessor function to get all connections from the
  // grid for the purposes of scanning.
  std::vector<Connection> const getConnections();

  // The is an accessor function to get a cell from the grid,
  // indexed by row and column.
  std::shared_ptr<Cell> getCell(unsigned int row, unsigned int column);

  // The is an accessor function to get a cell from the grid
  // at random
  std::shared_ptr<Cell> getRandomCell();

  // The is an accessor function to get a vector of cell from the grid,
  // that are adjacent to the cell at the given row and column.
  std::vector<std::shared_ptr<Cell> > getNeighboringCells(std::shared_ptr<Cell> c);

  // The is an accessor function to get a vector of cell from the grid,
  // that are adjacent to the cell at the given row and column and a
  // matching visitation status.
  std::vector<std::shared_ptr<Cell> > getNeighboringCellsWithVisitedStatus(std::shared_ptr<Cell> c,
                                                         bool visitedStatus);

  // returns true if the cell at row and column has at least one neighbor
  // with a matching visitation status
  bool hasAtLeastOneNeighboringCellWithVisitedStatus(std::shared_ptr<Cell> c,
                                                     bool visitedStatus);

  // returns a randomly selected cell adjacent to this one that
  // has the desired visitation status
  std::shared_ptr<Cell> getRandomNeighboringCellWithVisitedStatus(std::shared_ptr<Cell> c, bool visitedStatus);

  // connects cell a and cell b
  void connectCells(std::shared_ptr<Cell> a, std::shared_ptr<Cell> b);

  // The grid is composed of all the cells, to draw the grid,
  // all we need to do is draw each of the cells.
  std::vector<std::vector<rgb_matrix::Color> > getPixelMap();
};

#endif
