#ifndef GRID_H
#define GRID_H

#include <vector>

#include "cell.h"

/* -- GRID DEFINITION -- */
class Grid {
 private:
  /* Member Variables */
  // 2D vector of shared pointers to Cells
  std::vector<std::vector<Cell> > cells;
  // reference to the RGB canvas, assumed to be freed by the caller
  rgb_matrix::Canvas *canvas;
  // the number of rows in the grid.
  unsigned int height;
  // the number of columns in the grid.
  unsigned int width;

  /* Private Function Definitions */
  // This function is used to create the cells that that compose
  // the grid.
  std::vector<std::vector<Cell> > createCells();

  // Once the cells have been populated in the grid, we walk each cell to make
  // sure that it knows its neighbors.
  void populateNeighbors();

 public:
  /* Public Function Definitions */
  // Constructor for the grid creates the 2D vector of cells
  // and then populates the neighbors for each.
  Grid(rgb_matrix::Canvas *canvas, unsigned int height, unsigned int width);

  // Destructor needs to clear the canvas
  ~Grid();

  // The grid is composed of all the cells, to draw the grid,
  // all we need to do is draw each of the cells.
  void draw();

  // The is an accessor function to get all cells from the
  // grid for the purposes of scanning.
  const std::vector<std::vector<Cell> >& getCells();

  // The is an mutator function to set a cell in the grid
  void setCell(Cell c);

  // The is an accessor function to get a cell from the grid,
  // indexed by row and column.
  Cell getCell(unsigned int row, unsigned int column);

  // The is an accessor function to get a cell from the grid
  // at random
  Cell getRandomCell();
};

#endif
