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

template <typename T1, typename T2 = Cell, typename T3 = rgb_matrix::Canvas>
class Maze {
 public:
  using Coord = std::tuple<unsigned int, unsigned int>;
  using CoordList = std::vector<Coord>;
  using Pixel = std::tuple<unsigned int, unsigned int, unsigned int>;
  using PixelRow = std::vector<Pixel>;
  using PixelMap = std::vector<PixelRow>;
  static const int distance_between_pixels = 2;
  bool generated;

 private:
  Pixel wall_color = {0, 0, 0};
  Pixel not_connected_color = {255, 255, 255};
  Pixel connected_color = {0, 255, 0};
  Pixel emphasized_color = {255, 0, 0};
  unsigned int height;
  unsigned int width;
  Grid<T2> grid;
  T1 generation_strategy;
  T3* canvas;
  PixelMap map;
  CoordList pixels_to_update;
  PixelMap initMap();
  void drawMap();
  Coord getCoordOfCellById(unsigned int);
  int getSlope(Coord, Coord);
  void updateConnectionInPixelMap(unsigned int, unsigned int);
  void updateCellInPixelMap(unsigned int);
  void drawMapUpdates();

 public:
  Maze(T3* c)
      : generated(false),
        height(c->height()),
        width(c->width()),
        grid(Grid<T2>(c->height() / distance_between_pixels,
                      c->width() / distance_between_pixels)),
        generation_strategy(T1(&grid)),
        canvas(c),
        map(initMap()) {
    drawMap();
  };

  float generateStep();
  PixelMap generatePixelMap();
  void updatePixelMap();
};
#include "maze_impl.h"
#endif
