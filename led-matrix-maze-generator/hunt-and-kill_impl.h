const char* CantWalkException::what() const throw() {
  return "Cannot walk further, all connectable cells from here have been "
         "visited";
}

const char* HuntFailedException::what() const throw() {
  return "No cell found in the hunt, must be finished with generation.";
}

template <typename T>
unsigned int HuntAndKillStrategy<T>::init_current_cell() {
  unsigned int starting_cell = rand() % this->g->num_cells;
  auto cell = this->g->getCell(starting_cell);
  cell.visited = true;
  this->g->setCell(starting_cell, cell);
  return starting_cell;
}

template <typename T>
void HuntAndKillStrategy<T>::walk() {
  auto not_visited_and_connectable =
      [&](std::vector<unsigned int> connectable) {
        std::vector<unsigned int> matching;
        for (auto& cell : connectable) {
          if (!this->g->getCell(cell).visited) {
            matching.push_back(cell);
          }
        }
        return matching;
      };

  auto cell = this->g->getCell(this->current_cell);
  if (cell.emphasized) {
    cell.emphasized = false;
    this->g->setCell(this->current_cell, cell);
  }

  auto connectable_and_unvisited_cells = not_visited_and_connectable(
      this->g->getCellIdsMatching(this->current_cell, CONNECTABLE));

  if (connectable_and_unvisited_cells.size() == 0) {
    throw CantWalkException();
  }

  unsigned int choice = rand() % connectable_and_unvisited_cells.size();
  unsigned int next_cell = connectable_and_unvisited_cells.at(choice);
  this->g->modifyConnection(this->current_cell, next_cell, CONNECTED);
  this->current_cell = next_cell;

  auto curr = this->g->getCell(this->current_cell);
  curr.visited = true;

  this->g->setCell(this->current_cell, curr);
}

template <typename T>
void HuntAndKillStrategy<T>::hunt() {
  auto visited_and_connectable = [&](std::vector<unsigned int> connectable) {
    std::vector<unsigned int> matching;
    for (auto& cell : connectable) {
      if (this->g->getCell(cell).visited) {
        matching.push_back(cell);
      }
    }
    return matching;
  };

  unsigned int id;
  std::vector<unsigned int> connectable_and_visited_cells;
  for (id = 0; id < this->g->num_cells; id++) {
    if (this->g->getCell(id).visited) {
      // cell is visited, cannot be a candidate
      continue;
    }

    connectable_and_visited_cells =
        visited_and_connectable(this->g->getCellIdsMatching(id, CONNECTABLE));
    if (connectable_and_visited_cells.size() == 0) {
      // cell is not visited, but has no connectable cells that are visited.
      continue;
    }

    // cell is not visited, and can be connected to a visited cell, id found.
    break;
  }

  if (id >= this->g->num_cells) {
    // no suitable cell was found
    throw HuntFailedException();
  }
  unsigned int choice = rand() % connectable_and_visited_cells.size();
  unsigned int visited_cell_to_connect_to =
      connectable_and_visited_cells.at(choice);
  this->g->modifyConnection(id, visited_cell_to_connect_to, CONNECTED);
  this->current_cell = id;

  auto curr = this->g->getCell(this->current_cell);
  curr.visited = true;
  curr.emphasized = true;
  this->g->setCell(this->current_cell, curr);
}

template <typename T>
float HuntAndKillStrategy<T>::step() {
  try {
    this->walk();
    return 0.000001;
  } catch (CantWalkException& e) {
    try {
      this->hunt();
      return 1;
    } catch (HuntFailedException& e) {
      throw GenerationCompleteException();
    }
  }
}
