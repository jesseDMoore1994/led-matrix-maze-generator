#include "doctest.h"
#include "graphics.h"
#include "grid.h"

TEST_CASE("A grid can be created and it is populated with cells.") {
  Grid g(32, 32);
  auto cells = g.getCells();
  CHECK(cells.size() == 32);
  for (unsigned int i = 0; i < 32; i++) {
    CHECK(cells[i].size() == 32);
  }
}

TEST_CASE("Cells can be get and set in the grid.") {
  Grid g(32, 32);
  auto cell = g.getCell(15, 15);
  CHECK(cell.get_grid_row() == 15);
  CHECK(cell.get_grid_column() == 15);

  cell.colorEnforce(rgb_matrix::Color(255, 0, 0));
  g.setCell(cell);

  CHECK(g.getCell(15, 15).getColor().r == 255);
  CHECK(g.getCell(15, 15).getColor().g == 0);
  CHECK(g.getCell(15, 15).getColor().b == 0);

  cell.noColorEnforce();
  g.setCell(cell);

  CHECK(g.getCell(15, 15).getColor().r == 255);
  CHECK(g.getCell(15, 15).getColor().g == 255);
  CHECK(g.getCell(15, 15).getColor().b == 255);
}

TEST_CASE("Cells can be get and set in the grid.") {
  Grid g(32, 32);
  auto cell = g.getCell(15, 15);
  CHECK(cell.get_grid_row() == 15);
  CHECK(cell.get_grid_column() == 15);

  cell.colorEnforce(rgb_matrix::Color(255, 0, 0));
  g.setCell(cell);

  CHECK(g.getCell(15, 15).getColor().r == 255);
  CHECK(g.getCell(15, 15).getColor().g == 0);
  CHECK(g.getCell(15, 15).getColor().b == 0);

  cell.noColorEnforce();
  g.setCell(cell);

  CHECK(g.getCell(15, 15).getColor().r == 255);
  CHECK(g.getCell(15, 15).getColor().g == 255);
  CHECK(g.getCell(15, 15).getColor().b == 255);
}

// TODO: find some statistical analysis that can be done to test this
TEST_CASE("Can get a cell at random.") {
  Grid g(32, 32);

  for(int i = 0; i < 100; i++) {
    auto cell = g.getRandomCell();
    unsigned int row = cell.get_grid_row();
    unsigned int column = cell.get_grid_column();
    CHECK(0 <= row);
    CHECK(row < 32);
    CHECK(0 <= column);
    CHECK(column < 32);
  }
}

TEST_CASE("Grid can return neighboring cells.") {
  Grid g(32, 32);

  // This cell should only have two neighbors
  auto cell = g.getCell(0, 0);
  auto neighbors = g.getNeighboringCells(cell);
  CHECK(neighbors.size() == 2);

  // This cell should only have two neighbors
  cell = g.getCell(31, 31);
  neighbors = g.getNeighboringCells(cell);
  CHECK(neighbors.size() == 2);

  // This cell should only have three neighbors
  cell = g.getCell(0, 13);
  neighbors = g.getNeighboringCells(cell);
  CHECK(neighbors.size() == 3);

  // This cell should have four neighbors
  cell = g.getCell(3, 13);
  neighbors = g.getNeighboringCells(cell);
  CHECK(neighbors.size() == 4);
}

TEST_CASE("Grid can return neighboring cells based on visitation status.") {
  Grid g(32, 32);

  // This cell should only have two neighbors (both initialized to not visited)
  auto cell = g.getCell(0, 0);
  auto neighbors = g.getNeighboringCellsWithVisitedStatus(cell, false);
  CHECK(neighbors.size() == 2);
  // This cell should only have two neighbors (both initialized to not visited)
  neighbors = g.getNeighboringCellsWithVisitedStatus(cell, true);
  CHECK(neighbors.size() == 0);
  CHECK(g.hasAtLeastOneNeighboringCellWithVisitedStatus(cell, false));
  CHECK(!g.hasAtLeastOneNeighboringCellWithVisitedStatus(cell, true));

  // add some visited neighbors to 15, 15
  cell = g.getCell(15, 16);
  cell.visit();
  g.setCell(cell);

  cell = g.getCell(15, 14);
  cell.visit();
  g.setCell(cell);

  cell = g.getCell(14, 15);
  cell.visit();
  g.setCell(cell);

  // 15, 15 should have 3 visited neighbors and 1 unvisited
  cell = g.getCell(15, 15);
  neighbors = g.getNeighboringCellsWithVisitedStatus(cell, false);
  CHECK(neighbors.size() == 1);
  neighbors = g.getNeighboringCellsWithVisitedStatus(cell, true);
  CHECK(neighbors.size() == 3);
  CHECK(g.hasAtLeastOneNeighboringCellWithVisitedStatus(cell, false));
  CHECK(g.hasAtLeastOneNeighboringCellWithVisitedStatus(cell, true));

  cell = g.getCell(16, 15);
  cell.visit();
  g.setCell(cell);

  // 15, 15 should have 4 visited neighbors and 0 unvisited
  cell = g.getCell(15, 15);
  neighbors = g.getNeighboringCellsWithVisitedStatus(cell, false);
  CHECK(neighbors.size() == 0);
  neighbors = g.getNeighboringCellsWithVisitedStatus(cell, true);
  CHECK(neighbors.size() == 4);
  CHECK(!g.hasAtLeastOneNeighboringCellWithVisitedStatus(cell, false));
  CHECK(g.hasAtLeastOneNeighboringCellWithVisitedStatus(cell, true));
}

// TODO: find some better strategy for testing this
TEST_CASE("Can get a cell at random matching the requested visitation status.") {
  Grid g(32, 32);

  // This cell should only have two neighbors (both initialized to not visited)
  auto cell = g.getCell(0, 0);
  bool exception_thrown = false;
  try {
    g.getRandomNeighboringCellWithVisitedStatus(cell, true);
  } catch( const char* e ) {
    exception_thrown = true;
  }
  CHECK(exception_thrown);

  // add some visited neighbors to 15, 15
  cell = g.getCell(15, 16);
  cell.visit();
  g.setCell(cell);

  cell = g.getCell(15, 14);
  cell.visit();
  g.setCell(cell);

  cell = g.getCell(14, 15);
  cell.visit();
  g.setCell(cell);

  // 15, 15 should have 3 visited neighbors and 1 unvisited
  cell = g.getCell(15, 15);
  for(int i = 0; i < 100; i++) {
    auto next = g.getRandomNeighboringCellWithVisitedStatus(cell, true);
    CHECK(next.isVisited());
    CHECK(next.get_grid_row() != 16);
  }

  for(int i = 0; i < 100; i++) {
    auto next = g.getRandomNeighboringCellWithVisitedStatus(cell, false);
    CHECK(!next.isVisited());
    CHECK(next.get_grid_row() == 16);
    CHECK(next.get_grid_column() == 15);
  }
}

TEST_CASE("The grid is capable of connecting two cells.") {
  Grid g(32, 32);
  auto cell_a = g.getCell(15, 15);
  auto cell_b = g.getCell(15, 16);

  g.connectCells(cell_a, cell_b);
  auto cell_c = g.getCell(15, 15);
  auto cell_d = g.getCell(15, 16);

  CHECK(g.getConnections().size() == 1);

  CHECK(cell_a == cell_c);
  CHECK(cell_b == cell_d);
  CHECK(cell_c.connectionExists(Right));
  CHECK(cell_d.connectionExists(Left));
}

TEST_CASE("The grid can return a pixel map representation of itself.") {
  Grid g(32, 32);
  auto cell_a = g.getCell(15, 15);
  auto cell_b = g.getCell(15, 16);

  g.connectCells(cell_a, cell_b);
  auto pixel_map = g.getPixelMap();

  CHECK(pixel_map[30][30].r == 0);
  CHECK(pixel_map[30][30].g == 255);
  CHECK(pixel_map[30][30].b == 0);

  CHECK(pixel_map[30][31].r == 0);
  CHECK(pixel_map[30][31].g == 255);
  CHECK(pixel_map[30][31].b == 0);

  CHECK(pixel_map[30][32].r == 0);
  CHECK(pixel_map[30][32].g == 255);
  CHECK(pixel_map[30][32].b == 0);
}
