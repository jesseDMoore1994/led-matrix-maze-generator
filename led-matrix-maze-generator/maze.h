#ifndef MAZE_H
#define MAZE_H
#include <algorithm>
#include <exception>
#include <tuple>

#include "cell.h"
#include "grid.h"
#include "led-matrix.h"
#include "maze-exceptions.h"

struct SlopeDNEException : public std::exception {
  const char* what() const throw();
};

template <typename T1, typename T2, typename T3 = Cell,
          typename T4 = rgb_matrix::Canvas>
class Maze {
 public:
  using Coord = std::tuple<unsigned int, unsigned int>;
  using CoordList = std::vector<Coord>;
  using Pixel = std::tuple<unsigned int, unsigned int, unsigned int>;
  using PixelRow = std::vector<Pixel>;
  using PixelMap = std::vector<PixelRow>;
  static const int distance_between_pixels = 2;
  bool generated;
  bool solved;

 private:
  Pixel wall_color = {0, 0, 0};
  Pixel not_connected_color = {255, 255, 255};
  Pixel connected_color = {0, 255, 0};
  Pixel emphasized_color = {255, 0, 0};
  Pixel active_color = {0, 255, 0};
  Pixel dead_color = {255, 0, 0};
  Pixel complete_color = {255, 0, 0};
  unsigned int height;
  unsigned int width;
  Grid<T3> grid;
  T1 generation_strategy;
  T2 solving_strategy;
  T4* canvas;
  PixelMap map;
  CoordList pixels_to_update;
  PixelMap initMap();
  void drawMap();
  Coord getCoordOfCellById(unsigned int);
  int getSlope(Coord, Coord);
  void updateConnectionInPixelMap(unsigned int, unsigned int);
  void updateCellInPixelMap(unsigned int);
  void drawMapUpdates();
  void changeConnectedColor(Pixel);
  void resetCellData();

 public:
  Maze(T4* c)
      : generated(false),
        height(c->height()),
        width(c->width()),
        grid(Grid<T3>(c->height() / distance_between_pixels,
                      c->width() / distance_between_pixels)),
        generation_strategy(T1(&grid)),
        solving_strategy(T2(&grid)),
        canvas(c),
        map(initMap()) {
    drawMap();
  };

  float generateStep();
  float solveStep();
  void generatePixelMap();
  PixelMap getPixelMap();
  void updatePixelMap();
  void swapToSolvingMode();
};
#include "maze_impl.h"
#endif
