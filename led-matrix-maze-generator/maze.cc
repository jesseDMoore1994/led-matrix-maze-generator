#include "maze.h"
#include <exception>
#include <time.h>
#include <unistd.h>



extern volatile bool interrupt_received;


/* -- USER DEFINED EXCEPTIONS --*/
struct HuntNotSuccessful : public std::exception
{
  const char* what () const throw ()
  {
    return "Could not successfully find an unvisited cell";
  }
};

/* -- STRATEGY FUNCTIONS FOR MAZE GENERATION -- */
// Hunt and kill strategy which takes a grid object
// and generates a Maze
void huntAndKill(Grid* g) {
  auto walk = [&] (Cell cell) {
    // create current cell from passed in cell
    Cell current_cell = cell;

    // while the current cell in question does not have unvisited neighbors.
    while (current_cell.hasNeighborsWithVisitedStatus(false)) {
      if (interrupt_received) {
        return;
      }

      // decide the next direction at random from the unvisited cells around this one.
      direction dir = current_cell.getRandomDirectionWithVisitedStatus(false);
      //
      // connect current cell to neighbor and update it
      current_cell.connectNeighbor(dir);
      g->setCell(current_cell);
      // move current cell to neighbor and update it
      current_cell = current_cell.getNeighbor(dir);
      current_cell.visit();
      g->setCell(current_cell);

      // draw the grid for the update in walk
      g->draw();
      usleep(1 * 1000);  // wait a little to slow down things.
    }
  };

  auto hunt = [&] () {
    auto findTargetCell = [&] () {
      auto cells = g->getCells();
      for (std::size_t i = 0; i < cells.size(); i++) {
        for (std::size_t j = 0; j < cells[i].size(); j++) {
          if (cells[i][j].isVisited())
            continue;
          if (!cells[i][j].hasNeighborsWithVisitedStatus(true))
            continue;
          return g->getCell(i, j);
        }
      }
      throw HuntNotSuccessful();
    };

    // get target cell and direction by which to connect to the
    // existing maze.
    Cell c = findTargetCell();
    direction dir = c.getRandomDirectionWithVisitedStatus(true);

    // update target cell
    c.colorEnforce(255, 0, 0);
    c.visit();
    c.connectNeighbor(dir);
    g->setCell(c);

    // draw the grid to show the new target cell
    g->draw();
    usleep(1 * 1000000);  // wait a little to slow down things.

    // turn off the color override
    c.noColorEnforce();
    g->setCell(c);
    return c;

  };

  auto cell = g->getRandomCell();
  cell.visit();
  g->setCell(cell);
  while(true) {
    try {
      if (interrupt_received)
        return;
      walk(cell);
      cell = hunt();
    }
    catch (HuntNotSuccessful& e) {
      break;
    }
  }
}

Maze::Maze(rgb_matrix::Canvas* canvas, unsigned int height, unsigned int width)
 : grid(canvas, height, width) {}

void Maze::generate() {
  huntAndKill(&this->grid);
}
