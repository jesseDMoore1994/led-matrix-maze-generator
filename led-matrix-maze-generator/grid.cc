#include "grid.h"

std::vector<std::vector<Cell>> Grid::createCells() {
  // 2D vector of Cells
  std::vector<std::vector<Cell>> cells;
  // for the height of the grid we will create a new row of cells
  for (unsigned int i = 0; i < this->height; i++) {
    std::vector<Cell> row;
    // for the width of the grid we will push a new cell into the
    // row at the appropriate offset
    for (unsigned int j = 0; j < this->width; j++) {
      Cell c(i*2, j*2, i, j);
      // push the cell on the back of the row
      row.push_back(c);
    }
    // Now that the row is populated, add it to the pile.
    cells.push_back(row);
  }
  //we have all the rows now. send it!
  return cells;
}

// Once the cells have been populated in the grid, we walk each cell to make sure
// that it knows its neighbors.
void Grid::populateNeighbors() {
  // this lambda function can determine if a cell has a neighbor in a given direction
  // by checking the cell's location in the grid.
  auto hasNeighbor = [&] (direction dir, std::size_t row, std::size_t column) {
    switch(dir) {
      case Left:
        return column != 0;
        break;
      case Up:
        return row != this->height-1;
        break;
      case Right:
        return column != this->width-1;
        break;
      case Down:
        return row != 0;
        break;
      default:
        throw DirectionException("A direction that is not defined was attempted for use.");
    }
  };

  // this lambda function adds the neighbors on each side of cell appropriately
  // if it is determined that it has a neighbor in that direction.
  auto addNeighborsToCell = [&] (Cell* cell, std::size_t row, std::size_t column) {
    if (hasNeighbor(Right, row, column))
      cell->addNeighbor(Right, &(this->cells[row][column+1]));
    if (hasNeighbor(Up, row, column))
      cell->addNeighbor(Up, &(this->cells[row+1][column]));
    if (hasNeighbor(Left, row, column))
      cell->addNeighbor(Left, &(this->cells[row][column-1]));
    if (hasNeighbor(Down, row, column))
      cell->addNeighbor(Down, &(this->cells[row-1][column]));
  };

  // For each cell in the grid, we add its neighbors.
  for (std::size_t i = 0; i < this->cells.size(); i++) {
    for (std::size_t j = 0; j < this->cells[i].size(); j++) {
      addNeighborsToCell(&this->cells[i][j], i, j);
    }
  }
}

/* Public Function Definitions */
// Constructor for the grid creates the 2D vector of cells
// and then populates the neighbors for each.
Grid::Grid(rgb_matrix::Canvas *canvas, unsigned int height, unsigned int width) {
  this->canvas = canvas;
  this->height = height;
  this->width = width;
  this->cells = createCells();
  populateNeighbors();
}

// Destructor needs to clear the canvas
Grid::~Grid() {
  this->canvas->Fill(0, 0, 0);
}

// The grid is composed of all the cells, to draw the grid,
// all we need to do is draw each of the cells.
void Grid::draw() {
  for ( auto & row : this->cells ) {
    for ( auto & cell : row ) {
      cell.draw(this->canvas);
    }
  }
}

// The is an accessor function to get all cells from the
// grid for the purposes of scanning.
std::vector<std::vector<Cell>> const Grid::getCells() {
  return this->cells;
}

// The is an mutator function to set a cell in the grid
void Grid::setCell(Cell c) {
  this->cells[c.get_grid_row()][c.get_grid_column()] = c;
}

// The is an accessor function to get a cell from the grid,
// indexed by row and column.
Cell Grid::getCell(unsigned int row, unsigned int column) {
  return this->cells[row][column];
}

// The is an accessor function to get a cell from the grid
// at random
Cell Grid::getRandomCell() {
  unsigned int row = rand() % (this->height-1);
  unsigned int column = rand() % (this->width-1);
  return this->cells[row][column];
}
