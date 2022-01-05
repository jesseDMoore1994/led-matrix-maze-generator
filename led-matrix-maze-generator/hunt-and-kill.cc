#include "hunt-and-kill.h"

#include <time.h>
#include <unistd.h>

#include <exception>

#include "maze.h"

const char *WalkNotSuccessful::what() const throw() {
  return "Could not successfully find an unvisited neighboring cell";
}

const char *HuntNotSuccessful::what() const throw() {
  return "Could not successfully find an unvisited cell in the grid";
}

std::vector<std::vector<rgb_matrix::Color> > HuntAndKillStrategy::walk() {
  if (!this->g->hasAtLeastOneNeighboringCellWithVisitedStatus(
          this->current_cell, false)) {
    throw WalkNotSuccessful();
  }

  // decide the next direction at random from the unvisited cells around
  // this one.
  std::shared_ptr<Cell> next = this->g->getRandomNeighboringCellWithVisitedStatus(
      this->current_cell, false);
  //
  // connect current cell to neighbor and update it
  this->g->connectCells(this->current_cell, next);

  // move current cell to neighbor and update it
  this->current_cell = next;
  this->current_cell->visit();

  // draw the grid for the update in walk
  return this->g->getPixelMap();
}

std::vector<std::vector<rgb_matrix::Color> > HuntAndKillStrategy::hunt() {
  auto findTargetCell = [&]() {
    auto cells = this->g->getCells();
    for (std::size_t i = 0; i < cells.size(); i++) {
      for (std::size_t j = 0; j < cells[i].size(); j++) {
        auto cell = this->g->getCell(i, j);
        if (cell->isVisited()) {
          continue;
        }
        if (!this->g->hasAtLeastOneNeighboringCellWithVisitedStatus(cell,
                                                                    true)) {
          continue;
        }
        return this->g->getCell(i, j);
      }
    }
    throw HuntNotSuccessful();
  };

  // get target cell and direction by which to connect to the
  // existing maze.
  std::shared_ptr<Cell> target = findTargetCell();
  std::shared_ptr<Cell> next = this->g->getRandomNeighboringCellWithVisitedStatus(target, true);
  this->g->connectCells(target, next);

  // update target cell with new color to show it was found in the hunt
  target->colorEnforce(rgb_matrix::Color(255, 0, 0));
  target->visit();
  this->current_cell = target;

  return this->g->getPixelMap();
}

std::vector<std::vector<rgb_matrix::Color> > HuntAndKillStrategy::step() {
  if (this->current_cell->getColorEnforce()) {
    this->current_cell->noColorEnforce();
  }

  try {
    auto map = walk();
    usleep(1 * 1000);  // wait a little to slow down things.
    return map;
  } catch (WalkNotSuccessful &e) {
    auto map = hunt();
    usleep(1 * 1000000);  // wait a little to slow down things.
    return map;
  }
}

/* -- STRATEGY FUNCTIONS FOR MAZE GENERATION -- */
// Hunt and kill strategy which takes a grid object
// and generates a Maze
std::vector<std::vector<rgb_matrix::Color> > HuntAndKillStrategy::step_all() {
  while (true) {
    try {
      auto map = step();
    } catch (HuntNotSuccessful &e) {
      break;
    }
  }
  return this->g->getPixelMap();
}
