#include <chrono>
#include <iostream>

#include "doctest.h"
#include "grid.h"

TEST_CASE("A grid can be created.") {
  std::cout << "(A grid can be created)\n";
  auto start = std::chrono::high_resolution_clock::now();
  Grid<bool> g(4, 4);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "Time to create grid(4x4): " << duration.count() << "\n";

  auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);
  auto row_col2 = std::make_tuple<unsigned int, unsigned int>(2, 3);

  SUBCASE("The connection status between two cells can be interrogated") {
    std::cout
        << "  (The connection status between two cells can be interrogated)\n";
    SUBCASE("That can be done using two cell ids") {
      std::cout << "    (That can be done using two cell ids)\n";
      start = std::chrono::high_resolution_clock::now();
      auto status = g.queryConnection(2, 3);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to query grid(4x4): " << duration.count() << "\n";
      CHECK(status == CONNECTABLE);
    }

    SUBCASE("Or it can be done with two row column pairs") {
      std::cout << "    (Or it can be done with two row column pairs)\n";
      start = std::chrono::high_resolution_clock::now();
      auto status = g.queryConnection(row_col1, row_col2);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to query grid(4x4): " << duration.count() << "\n";
      CHECK(status == CONNECTABLE);
    }
  }

  SUBCASE("Only Adjacent cells up, left, right, and down connectable") {
    std::cout
        << "  (Only Adjacent cells up, left, right, and down connectable)\n";
    /*
                     ---------------------
      (num_rows-1)  3 | 12 | 13 | 14 | 15 |
                      |----|----|----|----|
                    2 |  8 |  9 | 10 | 11 |
                      |----|----|----|----|
                    1 |  4 |  5 |  6 |  7 |
                      |----|----|----|----|
                    0 |  0 |  1 |  2 |  3 |
                      ---------------------
                         0    1    2    3
                                        (num_cols-1)
     * */
    CHECK(g.queryConnection(2, 1) == CONNECTABLE);
    CHECK(g.queryConnection(0, 2) == NOT_CONNECTABLE);
    CHECK(g.queryConnection(0, 4) == CONNECTABLE);
    CHECK(g.queryConnection(0, 5) == NOT_CONNECTABLE);
    CHECK(g.queryConnection(0, 15) == NOT_CONNECTABLE);

    CHECK(g.queryConnection(3, 4) == NOT_CONNECTABLE);
    CHECK(g.queryConnection(3, 2) == CONNECTABLE);
    CHECK(g.queryConnection(3, 7) == CONNECTABLE);

    CHECK(g.queryConnection(8, 7) == NOT_CONNECTABLE);
    CHECK(g.queryConnection(8, 9) == CONNECTABLE);
    CHECK(g.queryConnection(8, 12) == CONNECTABLE);

    CHECK(g.queryConnection(15, 11) == CONNECTABLE);
    CHECK(g.queryConnection(15, 13) == NOT_CONNECTABLE);
    CHECK(g.queryConnection(15, 14) == CONNECTABLE);
    CHECK(g.queryConnection(15, 10) == NOT_CONNECTABLE);
    CHECK(g.queryConnection(15, 0) == NOT_CONNECTABLE);
  }

  SUBCASE("The grid can connect two cells that are connectable") {
    std::cout << "  (The grid can connect two cells that are connectable)\n";
    auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);
    auto row_col2 = std::make_tuple<unsigned int, unsigned int>(2, 3);

    REQUIRE(g.queryConnection(0, 1) == CONNECTABLE);

    SUBCASE("They can be connected by id.") {
      std::cout << "    (They can be connected by id)\n";
      start = std::chrono::high_resolution_clock::now();
      g.modifyConnection(0, 1, CONNECTED);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to modify grid(4x4): " << duration.count() << "\n";
      CHECK(g.queryConnection(0, 1) == CONNECTED);
    }

    SUBCASE("They can be connected by row column pairs.") {
      std::cout << "    (They can be connected by row column pairs)\n";
      start = std::chrono::high_resolution_clock::now();
      g.modifyConnection(row_col1, row_col2, CONNECTED);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to modify grid(4x4): " << duration.count() << "\n";
      CHECK(g.queryConnection(row_col1, row_col2) == CONNECTED);
    }

    SUBCASE(
        "Attempting to connect two non connectable cells results in an "
        "exception") {
      std::cout << "    (Attempting to connect two non connectable cells "
                   "results in an exception)\n";
      bool exception_thrown = false;
      try {
        g.modifyConnection(0, 11, CONNECTED);
      } catch (const char* e) {
        exception_thrown = true;
      }
      CHECK(exception_thrown);
    }
  }

  SUBCASE("The grid can disconnect two cells that are connected") {
    std::cout << "  (The grid can disconnect two cells that are connected)\n";
    auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);
    auto row_col2 = std::make_tuple<unsigned int, unsigned int>(2, 3);

    g.modifyConnection(0, 1, CONNECTED);
    g.modifyConnection(row_col1, row_col2, CONNECTED);
    REQUIRE(g.queryConnection(0, 1) == CONNECTED);
    REQUIRE(g.queryConnection(row_col1, row_col2) == CONNECTED);

    SUBCASE("They can be disconnected by id.") {
      std::cout << "    (They can be disconnected by id)\n";
      g.modifyConnection(0, 1, CONNECTABLE);
      CHECK(g.queryConnection(0, 1) == CONNECTABLE);
    }

    SUBCASE("They can be disconnected by row column pairs.") {
      std::cout << "    (They can be disconnected by row column pairs)\n";
      g.modifyConnection(row_col1, row_col2, CONNECTABLE);
      CHECK(g.queryConnection(row_col1, row_col2) == CONNECTABLE);
    }

    SUBCASE(
        "Attempting to disconnect two non connected cells results in an "
        "exception") {
      std::cout << "    (Attempting to disconnect two non connected cells "
                   "results in an exception)\n";
      bool exception_thrown = false;
      try {
        g.modifyConnection(14, 15, CONNECTABLE);
      } catch (const char* e) {
        exception_thrown = true;
      }
      CHECK(exception_thrown);
    }
  }

  SUBCASE("A cells data can be retrieved from the grid") {
    std::cout << "  (A cells data can be retrieved from the grid)\n";
    auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);

    SUBCASE("They can retrieved by id.") {
      std::cout << "    (They can be retrieved by id)\n";
      start = std::chrono::high_resolution_clock::now();
      auto cell = g.getCell(0);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to get cell from grid(4x4): " << duration.count()
                << "\n";
      CHECK(!cell);
    }

    SUBCASE("Or they can be retrieved by row column pairs.") {
      std::cout << "    (They can be retrieved by row column pairs)\n";
      start = std::chrono::high_resolution_clock::now();
      auto cell = g.getCell(row_col1);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to get cell from grid(4x4): " << duration.count()
                << "\n";
      CHECK(!cell);
    }
  }

  SUBCASE("A cells data can be modified in the grid") {
    std::cout << "  (A cells data can be modified in the grid)\n";
    auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);

    SUBCASE("They can modified by id.") {
      std::cout << "    (They can be modified by id)\n";
      start = std::chrono::high_resolution_clock::now();
      g.setCell(0, true);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to set cell in grid(4x4): " << duration.count()
                << "\n";
      auto cell = g.getCell(0);
      CHECK(cell);
    }

    SUBCASE("Or they can be modified by row column pairs.") {
      std::cout << "    (They can be modified by row column pairs)\n";
      start = std::chrono::high_resolution_clock::now();
      g.setCell(row_col1, true);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to set cell in grid(4x4): " << duration.count()
                << "\n";
      auto cell = g.getCell(row_col1);
      CHECK(cell);
    }
  }
}

TEST_CASE("A larger grid can be created.") {
  std::cout << "(A larger grid can be created)\n";
  auto start = std::chrono::high_resolution_clock::now();
  Grid<bool> g(32, 32);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "Time to create grid(32x32): " << duration.count() << "\n";

  auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);
  auto row_col2 = std::make_tuple<unsigned int, unsigned int>(2, 3);

  SUBCASE("The connection status between two cells can be interrogated") {
    std::cout
        << "  (The connection status between two cells can be interrogated)\n";
    SUBCASE("That can be done using two cell ids") {
      std::cout << "    (That can be done using two cell ids)\n";
      start = std::chrono::high_resolution_clock::now();
      auto status = g.queryConnection(2, 3);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to query grid(32x32): " << duration.count()
                << "\n";
      CHECK(status == CONNECTABLE);
    }

    SUBCASE("Or it can be done with two row column pairs") {
      std::cout << "    (Or it can be done with two row column pairs)\n";
      start = std::chrono::high_resolution_clock::now();
      auto status = g.queryConnection(row_col1, row_col2);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to query grid(32x32): " << duration.count()
                << "\n";
      CHECK(status == CONNECTABLE);
    }
  }

  SUBCASE("The grid can connect two cells that are connectable") {
    std::cout << "  (The grid can connect two cells that are connectable)\n";
    auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);
    auto row_col2 = std::make_tuple<unsigned int, unsigned int>(2, 3);

    REQUIRE(g.queryConnection(0, 1) == CONNECTABLE);

    SUBCASE("They can be connected by id.") {
      std::cout << "    (They can be connected by id)\n";
      start = std::chrono::high_resolution_clock::now();
      g.modifyConnection(0, 1, CONNECTED);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to modify grid(32x32): " << duration.count()
                << "\n";
      CHECK(g.queryConnection(0, 1) == CONNECTED);

      SUBCASE(
          "The connection is added to the recently modified connections list") {
        std::cout << "      (the connection is added to the reccently modified "
                     "connections list)\n";
        start = std::chrono::high_resolution_clock::now();
        auto modified = g.getRecentlyModifiedConnections();
        end = std::chrono::high_resolution_clock::now();
        duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "      Time to get recently modified connections in "
                     "grid(32x32): "
                  << duration.count() << "\n";
        CHECK(modified.size() == 2);
        CHECK(modified.at(0) == std::tuple<unsigned int, unsigned int>({0, 1}));
        CHECK(modified.at(1) == std::tuple<unsigned int, unsigned int>({1, 0}));
        SUBCASE(
            "Subsequent retrievals of recently modified connections are "
            "empty") {
          std::cout << "        (Subsequent retrievals of recently modified "
                       "connections are empty)\n";
          modified = g.getRecentlyModifiedConnections();
          CHECK(modified.size() == 0);
        }
      }
    }

    SUBCASE("They can be connected by row column pairs.") {
      std::cout << "    (They can be connected by row column pairs)\n";
      start = std::chrono::high_resolution_clock::now();
      g.modifyConnection(row_col1, row_col2, CONNECTED);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to modify grid(32x32): " << duration.count()
                << "\n";
      CHECK(g.queryConnection(row_col1, row_col2) == CONNECTED);

      SUBCASE(
          "The connection is added to the recently modified connections list") {
        std::cout << "      (the connection is added to the reccently modified "
                     "connections list)\n";
        start = std::chrono::high_resolution_clock::now();
        auto modified = g.getRecentlyModifiedConnections();
        end = std::chrono::high_resolution_clock::now();
        duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "      Time to get recently modified connections in "
                     "grid(32x32): "
                  << duration.count() << "\n";
        CHECK(modified.size() == 2);
        auto cell_id1 = g.getIdFromRowCol(row_col1);
        auto cell_id2 = g.getIdFromRowCol(row_col2);
        auto t1 = std::tuple<unsigned int, unsigned int>(cell_id1, cell_id2);
        auto t2 = std::tuple<unsigned int, unsigned int>(cell_id2, cell_id1);
        CHECK(modified.at(0) == t1);
        CHECK(modified.at(1) == t2);
        SUBCASE(
            "Subsequent retrievals of recently modified connections are "
            "empty") {
          std::cout << "        (Subsequent retrievals of recently modified "
                       "connections are empty)\n";
          modified = g.getRecentlyModifiedConnections();
          CHECK(modified.size() == 0);
        }
      }
    }
  }

  SUBCASE("A cells data can be retrieved from the grid") {
    std::cout << "  (A cells data can be retrieved from the grid)\n";
    auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);

    SUBCASE("They can retrieved by id.") {
      std::cout << "    (They can be retrieved by id)\n";
      start = std::chrono::high_resolution_clock::now();
      auto cell = g.getCell(0);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to get cell from grid(32x32): " << duration.count()
                << "\n";
      CHECK(!cell);
    }

    SUBCASE("Or they can be retrieved by row column pairs.") {
      std::cout << "    (They can be retrieved by row column pairs)\n";
      start = std::chrono::high_resolution_clock::now();
      auto cell = g.getCell(row_col1);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to get cell from grid(32x32): " << duration.count()
                << "\n";
      CHECK(!cell);
    }
  }

  SUBCASE("A cells data can be modified in the grid") {
    std::cout << "  (A cells data can be modified in the grid)\n";
    auto row_col1 = std::make_tuple<unsigned int, unsigned int>(2, 2);

    SUBCASE("They can modified by id.") {
      std::cout << "    (They can be modified by id)\n";
      start = std::chrono::high_resolution_clock::now();
      g.setCell(0, true);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to set cell in grid(32x32): " << duration.count()
                << "\n";
      auto cell = g.getCell(0);
      CHECK(cell);

      SUBCASE("The cell is added to the recently modified cell list") {
        std::cout << "      (the cell is added to the reccently modified cell "
                     "list)\n";
        start = std::chrono::high_resolution_clock::now();
        auto modified = g.getRecentlyModifiedCells();
        end = std::chrono::high_resolution_clock::now();
        duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout
            << "      Time to get recently modified cells in grid(32x32): "
            << duration.count() << "\n";
        CHECK(modified.size() == 1);
        CHECK(modified.at(0) == 0);
        SUBCASE("Subsequent retrievals of recently modified cells are empty") {
          std::cout << "        (Subsequent retrievals of recently modified "
                       "cells are empty)\n";
          modified = g.getRecentlyModifiedCells();
          CHECK(modified.size() == 0);
        }
      }
    }

    SUBCASE("Or they can be modified by row column pairs.") {
      std::cout << "    (They can be modified by row column pairs)\n";
      start = std::chrono::high_resolution_clock::now();
      g.setCell(row_col1, true);
      auto cell_id = g.getIdFromRowCol(row_col1);
      end = std::chrono::high_resolution_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      std::cout << "    Time to set cell in grid(32x32): " << duration.count()
                << "\n";
      auto cell = g.getCell(row_col1);
      CHECK(cell);

      SUBCASE("The cell is added to the recently cells modified list") {
        std::cout << "      (the cell is added to the reccently modified cells "
                     "list)\n";
        start = std::chrono::high_resolution_clock::now();
        auto modified = g.getRecentlyModifiedCells();
        end = std::chrono::high_resolution_clock::now();
        duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout
            << "      Time to get recently modified cells in grid(32x32): "
            << duration.count() << "\n";
        CHECK(modified.size() == 1);
        CHECK(modified.at(0) == cell_id);
        SUBCASE("Subsequent retrievals of recently modified cells are empty") {
          std::cout << "        (Subsequent retrievals of recently modified "
                       "cells are empty)\n";
          modified = g.getRecentlyModifiedCells();
          CHECK(modified.size() == 0);
        }
      }
    }
  }
}
