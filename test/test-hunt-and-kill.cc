#include <time.h>

#include <algorithm>
#include <iostream>

#include "cell.h"
#include "doctest.h"
#include "hunt-and-kill.h"

TEST_CASE("The hunt and kill strategy can be created.") {
  srand(time(NULL));
  Grid<Cell> g(32, 32);
  HuntAndKillStrategy<Cell> strat(&g);

  SUBCASE(
      "current cell is initialized to a rando value and marked as visited") {
    CHECK(strat.current_cell >= 0);
    CHECK(strat.current_cell < 32 * 32);
    CHECK(g.getCell(strat.current_cell).visited);
  }

  SUBCASE("calling walk will eventually lead to a state where you can't walk") {
    std::vector<unsigned int> visited_cells;
    bool can_walk = true;
    visited_cells.push_back(strat.current_cell);
    while (can_walk) {
      try {
        strat.walk();
        visited_cells.push_back(strat.current_cell);
      } catch (CantWalkException& e) {
        can_walk = false;
      }
    }
    CHECK(!can_walk);

    SUBCASE(
        "The last cell is surrounded by visited nodes, and no cell is visited "
        "twice.") {
      int size_before_unique = visited_cells.size();

      std::vector<unsigned int>::iterator iter;
      iter = std::unique(visited_cells.begin(),
                         visited_cells.begin() + visited_cells.size());
      visited_cells.resize(std::distance(visited_cells.begin(), iter));
      CHECK(size_before_unique == visited_cells.size());
      CHECK(visited_cells.size() > 0);

      auto cells_that_are_connectable =
          g.getCellIdsMatching(strat.current_cell, CONNECTABLE);
      for (const auto& cell : cells_that_are_connectable) {
        CHECK(g.getCell(cell).visited);
      }
    }

    SUBCASE(
        "calling hunt finds a cell that is not visited but connectable to one "
        "that is and connects it.") {
      strat.hunt();
      auto cells_that_are_connected =
          g.getCellIdsMatching(strat.current_cell, CONNECTED);
      CHECK(cells_that_are_connected.size() == 1);
      CHECK(g.getCell(cells_that_are_connected[0]).visited);
      CHECK(std::find(visited_cells.begin(), visited_cells.end(),
                      cells_that_are_connected[0]) != visited_cells.end());
    }
  }

  SUBCASE(
      "A user can call step until a generation complete exception is thrown") {
    std::vector<unsigned int> visited_cells;
    bool generated = false;
    visited_cells.push_back(strat.current_cell);
    while (!generated) {
      try {
        strat.step();
        visited_cells.push_back(strat.current_cell);
      } catch (GenerationCompleteException& e) {
        generated = true;
      }
    }
    CHECK(generated);
    CHECK(visited_cells.size() == g.num_cells);
  }
}
