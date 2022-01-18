template <typename T>
unsigned int DepthFirstSearchStrategy<T>::init_starting_cell() {
  unsigned int starting_cell = rand() % this->g->num_cells;
  auto cell = this->g->getCell(starting_cell);
  cell.visited = true;
  this->g->setCell(starting_cell, cell);
  this->current_path.push(starting_cell);
  return starting_cell;
}

template <typename T>
unsigned int DepthFirstSearchStrategy<T>::init_goal_cell() {
  unsigned int goal_cell;
  do {
    goal_cell = rand() % this->g->num_cells;
  } while(goal_cell == this->starting_cell);
  auto cell = this->g->getCell(goal_cell);
  cell.target = true;
  this->g->setCell(goal_cell, cell);
  return goal_cell;
}

template <typename T>
float DepthFirstSearchStrategy<T>::dfs() {
    auto const marchOn = [&] (std::vector<unsigned int> connected) {
      // get the next cell
      auto next = connected.at(0);
      // set the path between current and next active
      this->g->modifyConnection(this->current_cell, next, PATH_ACTIVE);
      // set the next cell to current
      this->current_cell = next;
      // modify it to be visited
      auto curr = this->g->getCell(this->current_cell);
      curr.visited = true;
      this->g->setCell(this->current_cell, curr);
      // add the new current cell to the path
      this->current_path.push(this->current_cell);
      if ( this->current_cell == this->goal_cell ) {
        this->goal_found = true;
      }
    };
    auto const backtrack = [&] (ConnectionStatus status) {
      if (this->current_path.size() <= 1) {
        throw BacktrackException();
      }
      // remove the current cell from the path
      this->current_path.pop();
      // get the cell previous to this one on the path.
      auto next = this->current_path.top();
      // the link between the previous cell and this one is now status
      this->g->modifyConnection(next, this->current_cell, status);
      // move the current cell to the previous cell.
      this->current_cell = next;
    };

    auto const getConnectedAndUnvisited = [&] () {
      auto connected = this->g->getCellIdsMatching(this->current_cell, CONNECTED);
      std::vector<unsigned int> connected_and_unvisited;
      for (auto const & cell: connected) {
        if (!this->g->getCell(cell).visited) {
          connected_and_unvisited.push_back(cell);
        }
      }
      return connected_and_unvisited;
    };

    if(goal_found) {
      backtrack(PATH_COMPLETE);
      return 0.5;
    }

    auto potential_traversals = getConnectedAndUnvisited();
    if(potential_traversals.size() != 0) {
      marchOn(potential_traversals);
      return 1;
    }

    backtrack(PATH_DEAD);
    return 1;
}

template <typename T>
float DepthFirstSearchStrategy<T>::step() {
    try {
      return dfs();
    } catch( BacktrackException &e ) {
      if (goal_found) {
        throw AlreadySolvedException();
      }
      throw e;
    }
}
