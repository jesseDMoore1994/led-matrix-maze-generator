#include <time.h>

#include <algorithm>
#include <iostream>

#include "cell.h"
#include "doctest.h"
#include "depth-first-search.h"
#include "hunt-and-kill.h"
#include "maze-exceptions.h"

TEST_CASE("The depth first search strategy can be created.") {
  std::cout << "(The depth first search strategy can be created)\n";
  srand(time(NULL));
  Grid<Cell> g(32, 32);
  HuntAndKillStrategy<Cell> gen_strat(&g);
  bool generated = false;
  while (!generated) {
    try {
      auto secs = gen_strat.step();
    } catch (GenerationCompleteException &e) {
      generated = true;
    }
  }

  for (unsigned int i = 0; i < g.num_cells; i++) {
    g.setCell(i, Cell());
  }

  DepthFirstSearchStrategy<Cell> solve_strat(&g);

  SUBCASE("The starting cell and the goal cell are different cells") {
    std::cout << "  (The starting cell and the goal cell are different cells)\n";
    CHECK(solve_strat.starting_cell != solve_strat.goal_cell);
  }

  SUBCASE("Traversing to a new node adds it to the current path") {
    std::cout << "  (Traversing to a new node adds it to the current path)\n";
    CHECK(solve_strat.current_path.size() == 1);
    auto secs = solve_strat.step();
    CHECK(solve_strat.current_path.size() == 2);
  }

  SUBCASE("When the current cell has no connected unvisited nighbors, backtracking starts") {
    std::cout << "  (When the current cell has no connected unvisited nighbors, backtracking starts)\n";
    std::vector<unsigned int> connected_and_unvisited;
    std::vector<unsigned int> path_as_list;

    // step until we hit a dead end
    do {
      connected_and_unvisited.clear();
      auto secs = solve_strat.step();
      path_as_list.push_back(solve_strat.current_cell);
      auto connected = g.getCellIdsMatching(solve_strat.current_cell, CONNECTED);
      for (auto const & cell : connected) {
        if (!g.getCell(cell).visited) {
          connected_and_unvisited.push_back(cell);
        }
      }
    } while(connected_and_unvisited.size() > 0);

    auto size_before = solve_strat.current_path.size();
    auto secs = solve_strat.step();
    auto size_after = solve_strat.current_path.size();
    CHECK((size_before - 1) == size_after);
    auto end_of_path = solve_strat.current_path.top();
    CHECK(end_of_path == path_as_list.rbegin()[1]);
  }

  SUBCASE("Eventually, the goal cell is found.") {
    std::cout << "  (Eventually, the goal cell is found)\n";
    bool goal_found = false;
    std::vector<unsigned int> path_as_list;

    // step until we hit the goal
    do {
      path_as_list.push_back(solve_strat.current_cell);
      auto secs = solve_strat.step();
      if (solve_strat.current_cell == solve_strat.goal_cell) {
        path_as_list.push_back(solve_strat.current_cell);
        goal_found = true;
      }
    } while(!goal_found);

    CHECK(goal_found);
    SUBCASE("The final path is backtracked until there is nothing left") {
      bool complete = false;
      while(!complete) {
        try {
          auto secs = solve_strat.step();
        } catch (AlreadySolvedException& e) {
          complete = true;
        }
      }

      CHECK(solve_strat.current_path.size() == 1);
    }
  }

}
