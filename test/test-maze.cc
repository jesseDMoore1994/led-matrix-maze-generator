#include <chrono>
#include <iostream>

#include "cell.h"
#include "doctest.h"
#include "maze.h"

struct TestStrategy {
  Grid<Cell>* g;
  bool called_once;
  TestStrategy(Grid<Cell>* grid) : g(grid){};
  float step() {
    if (called_once) {
      throw GenerationCompleteException();
    }
    g->modifyConnection(0, 1, CONNECTED);
    Cell c;
    c.visited = true;
    c.emphasized = true;
    g->setCell(2, c);
    called_once = true;
    return 0.1;
  };
};

struct TestCanvas {
  int w = 64;
  int h = 64;
  int width() { return this->w; };
  int height() { return this->h; };
  std::vector<std::tuple<int, int, int, int, int> > pixel_calls;
  void SetPixel(int x, int y, int r, int g, int b) {
    this->pixel_calls.push_back(
        std::tuple<int, int, int, int, int>(x, y, r, g, b));
  };
  std::vector<std::tuple<int, int, int, int, int> > getPixelCalls() {
    return this->pixel_calls;
  }
  void clearPixelCalls() { this->pixel_calls.clear(); }
  std::vector<std::tuple<int, int, int, int, int> > getCallsForSpecificPixel(
      int tx, int ty) {
    std::vector<std::tuple<int, int, int, int, int> > matching;
    for (const auto call : this->pixel_calls) {
      auto const [x, y, r, g, b] = call;
      if (x == tx && y == ty) {
        matching.push_back(call);
      }
    }
    return matching;
  }
};

TEST_CASE("A maze can be created.") {
  std::cout << "(A maze can be created)\n";
  auto start = std::chrono::high_resolution_clock::now();
  TestCanvas* c = new TestCanvas;
  Maze<TestStrategy, int, Cell, TestCanvas> m(c);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "Time to create maze: " << duration.count() << "\n";

  SUBCASE("Maze draws the initial state on the canvas ") {
    std::cout << "  (Maze draws the initial state on the canvas)\n";
    auto calls = c->getPixelCalls();
    for (auto const& call : calls) {
      auto const [x, y, r, g, b] = call;
      if (x % 2 == 0 && y % 2 == 0) {
        CHECK(x / 2 < 32);
        CHECK(x / 2 >= 0);
        CHECK(y / 2 < 32);
        CHECK(y / 2 >= 0);
        CHECK(r == 255);
        CHECK(g == 255);
        CHECK(b == 255);
      }
    }
  }

  SUBCASE("Maze can return a pixel map format") {
    std::cout << "  (Maze can return a pixel map format)\n";
    start = std::chrono::high_resolution_clock::now();
    m.generatePixelMap();
    auto map = m.getPixelMap();
    end = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "  Time to generate pixel map: " << duration.count() << "\n";
    CHECK(map.size() == 64);
    for (auto const& row : map) {
      CHECK(row.size() == 64);
    }
  }


  SUBCASE("The maze can update just the pixels that need it") {
    std::cout << "  (The maze can update just the pixels that need it)\n";
    auto secs = m.generateStep();
    CHECK(secs == doctest::Approx(0.1).epsilon(0.1));
    start = std::chrono::high_resolution_clock::now();
    c->clearPixelCalls();
    m.updatePixelMap();
    end = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "  Time to update pixel map: " << duration.count() << "\n";

    SUBCASE("The intent is realized on the canvas") {
      std::cout << "    (The intent is realized on the canvas)\n";
      std::vector<std::tuple<int, int, int, int, int> > p =
          c->getCallsForSpecificPixel(0, 0);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 0);
      CHECK(std::get<3>(p[0]) == 255);
      CHECK(std::get<4>(p[0]) == 0);
      p = c->getCallsForSpecificPixel(0, 1);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 0);
      CHECK(std::get<3>(p[0]) == 255);
      CHECK(std::get<4>(p[0]) == 0);
      p = c->getCallsForSpecificPixel(0, 2);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 0);
      CHECK(std::get<3>(p[0]) == 255);
      CHECK(std::get<4>(p[0]) == 0);
      p = c->getCallsForSpecificPixel(0, 3);
      CHECK(p.size() == 0);
      p = c->getCallsForSpecificPixel(0, 4);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 255);
      CHECK(std::get<3>(p[0]) == 0);
      CHECK(std::get<4>(p[0]) == 0);
    }
  }

  SUBCASE("The maze can refresh to solving mode.") {
    std::cout << "  (The maze can refresh to solving mode.)\n";
    auto secs = m.generateStep();
    CHECK(secs == doctest::Approx(0.1));
    secs = m.generateStep();
    CHECK(secs == doctest::Approx(0.0));
    m.updatePixelMap();
    c->clearPixelCalls();
    start = std::chrono::high_resolution_clock::now();
    m.swapToSolvingMode();
    end = std::chrono::high_resolution_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "  Time to change connected color: " << duration.count() << "\n";

    SUBCASE("The intent is realized on the canvas") {
      std::cout << "    (The intent is realized on the canvas)\n";
      std::vector<std::tuple<int, int, int, int, int> > p =
          c->getCallsForSpecificPixel(0, 0);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 255);
      CHECK(std::get<3>(p[0]) == 255);
      CHECK(std::get<4>(p[0]) == 255);
      p = c->getCallsForSpecificPixel(0, 1);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 255);
      CHECK(std::get<3>(p[0]) == 255);
      CHECK(std::get<4>(p[0]) == 255);
      p = c->getCallsForSpecificPixel(0, 2);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 255);
      CHECK(std::get<3>(p[0]) == 255);
      CHECK(std::get<4>(p[0]) == 255);
      p = c->getCallsForSpecificPixel(0, 3);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 0);
      CHECK(std::get<3>(p[0]) == 0);
      CHECK(std::get<4>(p[0]) == 0);
      p = c->getCallsForSpecificPixel(0, 4);
      CHECK(p.size() == 1);
      CHECK(std::get<2>(p[0]) == 255);
      CHECK(std::get<3>(p[0]) == 0);
      CHECK(std::get<4>(p[0]) == 0);
    }
  }
  delete c;
}
