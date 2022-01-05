#include "grid.h"

std::vector<std::vector<std::shared_ptr<Cell> > > Grid::createCells() {
  // 2D vector of Cells
  std::vector<std::vector<std::shared_ptr<Cell> > > cells;
  // for the height of the grid we will create a new row of cells
  for (unsigned int i = 0; i < this->height; i++) {
    std::vector<std::shared_ptr<Cell> > row;
    // for the width of the grid we will push a new cell into the
    // row at the appropriate offset
    for (unsigned int j = 0; j < this->width; j++) {
      row.push_back(std::make_shared<Cell>(i * 2, j * 2, i, j));
    }
    // Now that the row is populated, add it to the pile.
    cells.push_back(row);
  }
  // we have all the rows now. send it!
  return cells;
}

/* Public Function Definitions */
// Constructor for the grid creates the 2D vector of cells
// and then populates the neighbors for each.
Grid::Grid(unsigned int height, unsigned int width) {
  this->height = height;
  this->width = width;
  this->cells = createCells();
}

// The is an accessor function to get all cells from the
// grid for the purposes of scanning.
std::vector<std::vector<std::shared_ptr<Cell> > > Grid::getCells() { return this->cells; }

// The is an accessor function to get all connections from the
// grid for the purposes of scanning.
std::vector<Connection> const Grid::getConnections() {
  return this->connections;
}

// The is an accessor function to get a cell from the grid,
// indexed by row and column.
std::shared_ptr<Cell> Grid::getCell(unsigned int row, unsigned int column) {
  if (row >= this->height || column >= this->width)
    throw "invalid cell attempted access";
  return this->cells[row][column];
}

// The is an accessor function to get a cell from the grid
// at random
std::shared_ptr<Cell> Grid::getRandomCell() {
  unsigned int row = rand() % (this->height - 1);
  unsigned int column = rand() % (this->width - 1);
  return this->cells[row][column];
}

// The is an accessor function to get a vector of cell from the grid,
// that are adjacent to the cell at a given row and column.
std::vector<std::shared_ptr<Cell> > Grid::getNeighboringCells(std::shared_ptr<Cell> c) {
  std::vector<std::shared_ptr<Cell> > adjacent;
  unsigned int row = c->get_grid_row();
  unsigned int column = c->get_grid_column();

  if (column != 0) {
    adjacent.push_back(this->cells[row][column - 1]);
  }
  if (row != 0) {
    adjacent.push_back(this->cells[row - 1][column]);
  }
  if (column != (this->width - 1)) {
    adjacent.push_back(this->cells[row][column + 1]);
  }
  if (row != (this->height - 1)) {
    adjacent.push_back(this->cells[row + 1][column]);
  }
  return adjacent;
}

// The is an accessor function to get a vector of cell from the grid,
// that are adjacent to the cell at a given row and column.
std::vector<std::shared_ptr<Cell> > Grid::getNeighboringCellsWithVisitedStatus(
    std::shared_ptr<Cell> c, bool visitedStatus) {
  std::vector<std::shared_ptr<Cell> > adjacent = getNeighboringCells(c);
  std::vector<std::shared_ptr<Cell> > matching;
  for (auto& neighbor : adjacent) {
    if (neighbor->isVisited() == visitedStatus) {
      matching.push_back(neighbor);
    }
  }
  return matching;
}

// The is an accessor function to get a vector of cell from the grid,
// that are adjacent to the cell at a given row and column.
bool Grid::hasAtLeastOneNeighboringCellWithVisitedStatus(std::shared_ptr<Cell> c,
                                                         bool visitedStatus) {
  std::vector<std::shared_ptr<Cell> > matching =
      getNeighboringCellsWithVisitedStatus(c, visitedStatus);

  return (matching.size() != 0);
}

// The is an accessor function to get a vector of cell from the grid,
// that are adjacent to the cell at a given row and column.
std::shared_ptr<Cell> Grid::getRandomNeighboringCellWithVisitedStatus(std::shared_ptr<Cell> c,
                                                     bool visitedStatus) {
  std::vector<std::shared_ptr<Cell> > matching =
      getNeighboringCellsWithVisitedStatus(c, visitedStatus);
  if (matching.size() == 0) {
    throw "No neighbors with that visitation status exists";
  }

  unsigned int random_idx = rand() % matching.size();
  auto choice = std::begin(matching);
  std::advance(choice, random_idx);
  return *choice;
}

void Grid::connectCells(std::shared_ptr<Cell> a, std::shared_ptr<Cell> b) {
  Connection c = Connection(a, b);

  this->connections.push_back(c);
}

// The function returns a pixel map representation
// of the grid for drawing purposes.
std::vector<std::vector<rgb_matrix::Color> > Grid::getPixelMap() {
  auto cells = this->getCells();
  unsigned int max_x = cells.back().back()->get_x_position();
  unsigned int max_y = cells.back().back()->get_y_position();
  std::vector<std::vector<rgb_matrix::Color> > pixel_map;
  for (unsigned int i = 0; i <= max_x; i++) {
    std::vector<rgb_matrix::Color> pixel_row;
    for (unsigned int j = 0; j <= max_y; j++) {
      pixel_row.push_back(rgb_matrix::Color());
    }
    pixel_map.push_back(pixel_row);
  }

  for (auto& row : this->cells) {
    for (auto& cell : row) {
      pixel_map[cell->get_x_position()][cell->get_y_position()] = cell->getColor();
    }
  }

  for (auto& connection : this->connections) {
    auto a = connection.getA();
    auto b = connection.getB();

    // same row
    if(a->get_x_position() == b->get_x_position()) {
      if (a->get_y_position() < b->get_y_position()) {
        for (unsigned int i = a->get_y_position(); i < b->get_y_position(); i++) {
          pixel_map[a->get_x_position()][i] = connection.getColor();
        }
      } else {
        for (unsigned int i = b->get_y_position(); i < a->get_y_position(); i++) {
          pixel_map[a->get_x_position()][i] = connection.getColor();
        }
      }
    }

    // same column
    if(a->get_y_position() == b->get_y_position()) {
      if (a->get_x_position() < b->get_x_position()) {
        for (unsigned int i = a->get_x_position(); i < b->get_x_position(); i++) {
          pixel_map[i][a->get_y_position()] = connection.getColor();
        }
      } else {
        for (unsigned int i = b->get_x_position(); i < a->get_x_position(); i++) {
          pixel_map[i][a->get_y_position()] = connection.getColor();
        }
      }
    }
  }

  return pixel_map;
}
