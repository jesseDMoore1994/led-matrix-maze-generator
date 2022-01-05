#include <iostream>
#include <memory>

#include "cell.h"
#include "connection.h"
#include "doctest.h"
#include "graphics.h"

TEST_CASE("Cells can be created and queried.") {
  Cell c(0, 0, 0, 0);
  CHECK(c.get_x_position() == 0);
  CHECK(c.get_y_position() == 0);
  CHECK(c.get_grid_row() == 0);
  CHECK(c.get_grid_column() == 0);
}

TEST_CASE("Cells can be visited and then and it is reflected.") {
  Cell c(0, 0, 0, 0);
  REQUIRE(!c.isVisited());

  c.visit();
  CHECK(c.isVisited());
}

TEST_CASE("A Cell is considered equivalent if it matches positionally.") {
  Cell a(0, 0, 0, 0);
  Cell b(0, 0, 0, 0);
  Cell c(0, 0, 0, 1);

  CHECK(a == b);
  CHECK(b != c);
  CHECK(a != c);
}

TEST_CASE("A Cell manages its own color, unless there's an override.") {
  Cell a(0, 0, 0, 0);

  // By default a cell is white when not connected.
  CHECK(a.getColor().r == 255);
  CHECK(a.getColor().g == 255);
  CHECK(a.getColor().b == 255);

  a.colorEnforce(rgb_matrix::Color(255, 128, 33));

  // Default should be overwritten.
  CHECK(a.getColor().r == 255);
  CHECK(a.getColor().g == 128);
  CHECK(a.getColor().b == 33);

  a.noColorEnforce();

  // Default should come back.
  CHECK(a.getColor().r == 255);
  CHECK(a.getColor().g == 255);
  CHECK(a.getColor().b == 255);
}

TEST_CASE("A Connection can be created with two cells.") {
  auto a = std::make_shared<Cell>(0, 0, 0, 0);
  auto b = std::make_shared<Cell>(2, 0, 1, 0);
  REQUIRE(a != b);

  Connection c(a, b);
}

TEST_CASE("A Connection can be interrogated about its endpoints.") {
  auto a = std::make_shared<Cell>(0, 0, 0, 0);
  auto b = std::make_shared<Cell>(2, 0, 1, 0);
  REQUIRE(a != b);

  Connection c(a, b);

  CHECK(a == c.getA());
  CHECK(b == c.getB());
  CHECK(b == c.other(a));
  CHECK(a == c.other(b));

  CHECK(a->connectionExists(Up));
  CHECK(!a->connectionExists(Down));
  CHECK(!a->connectionExists(Left));
  CHECK(!a->connectionExists(Right));
  CHECK(a->getConnectedNeighbor(Up) == b);
  CHECK(!b->connectionExists(Up));
  CHECK(b->connectionExists(Down));
  CHECK(!b->connectionExists(Left));
  CHECK(!b->connectionExists(Right));
  CHECK(b->getConnectedNeighbor(Down) == a);
}

TEST_CASE("A Connected cell sets itself to green.") {
  auto a = std::make_shared<Cell>(0, 0, 0, 0);
  auto b = std::make_shared<Cell>(2, 0, 1, 0);
  REQUIRE(a != b);

  Connection c(a, b);

  CHECK(a->getColor().r == 0);
  CHECK(a->getColor().g == 255);
  CHECK(a->getColor().b == 0);

  CHECK(b->getColor().r == 0);
  CHECK(b->getColor().g == 255);
  CHECK(b->getColor().b == 0);
}

TEST_CASE(
    "Attempting to get a connection on a cell in a direction that does not "
    "exist throws an error.") {
  auto a = std::make_shared<Cell>(0, 0, 0, 0);
  auto b = std::make_shared<Cell>(2, 0, 1, 0);
  REQUIRE(a != b);

  Connection c(a, b);

  bool exception_thrown = false;
  try {
    a.getConnectedNeighbor(Left);
  } catch (CellException& e) {
    exception_thrown = true;
  }

  CHECK(exception_thrown);
}

TEST_CASE(
    "Attempting to add a connection on a cell in a direction that already "
    "exists throws an error.") {
  auto a = std::make_shared<Cell>(0, 0, 0, 0);
  auto b = std::make_shared<Cell>(2, 0, 1, 0);
  auto c = std::make_shared<Cell>(2, 0, 1, 0);
  REQUIRE(a != b);

  Connection d(a, b);

  bool exception_thrown = false;
  try {
    Connection e(a, c);
  } catch (CellException& f) {
    exception_thrown = true;
  }

  CHECK(exception_thrown);
}
