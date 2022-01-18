template <class T>
typename Grid<T>::MatrixFmt Grid<T>::createStateMatrix(unsigned int num_rows,
                                                       unsigned int num_cols) {
  // adjacency matrix is an NxN matrix of cell ids where N
  // is the number of cells.
  std::vector<std::vector<ConnectionStatus>> matrix(
      this->num_cells,
      std::vector<ConnectionStatus>(this->num_cells, NOT_CONNECTABLE));

  // begin the process of marking neighboring cells as connectable
  for (unsigned int id = 0; id < this->num_cells; id++) {
    auto [current_row, current_col] = getRowColFromId(id);

    // row is above 0, so it has a neighbor beneath it.
    if (current_row > 0) {
      RowColFmt down_row_col = std::make_tuple<unsigned int, unsigned int>(
          current_row - 1, std::move(current_col));
      unsigned int id_down = getIdFromRowCol(down_row_col);
      matrix[id][id_down] = CONNECTABLE;
      matrix[id_down][id] = CONNECTABLE;
    }

    // column is above 0, so it has a neighbor to the left of it.
    if (current_col > 0) {
      RowColFmt left_row_col = std::make_tuple<unsigned int, unsigned int>(
          std::move(current_row), current_col - 1);
      unsigned int id_left = getIdFromRowCol(left_row_col);
      matrix[id][id_left] = CONNECTABLE;
      matrix[id_left][id] = CONNECTABLE;
    }

    // row is less than max addressable row (num_rows-1),
    // so it has a neighbor above it.
    if (current_row < num_rows - 1) {
      RowColFmt up_row_col = std::make_tuple<unsigned int, unsigned int>(
          current_row + 1, std::move(current_col));
      unsigned int id_up = getIdFromRowCol(up_row_col);
      matrix[id][id_up] = CONNECTABLE;
      matrix[id_up][id] = CONNECTABLE;
    }

    // col is less than max addressable column (num_cols-1),
    // so it has a neighbor to the right of it.
    if (current_col < num_cols - 1) {
      RowColFmt right_row_col = std::make_tuple<unsigned int, unsigned int>(
          std::move(current_row), current_col + 1);
      unsigned int id_right = getIdFromRowCol(right_row_col);
      matrix[id][id_right] = CONNECTABLE;
      matrix[id_right][id] = CONNECTABLE;
    }
  }
  return matrix;
}

template <class T>
unsigned int Grid<T>::getIdFromRowCol(Grid::RowColFmt row_col) {
  auto [row, col] = row_col;
  return (row * this->num_cols) + col;
};

template <class T>
typename Grid<T>::RowColFmt Grid<T>::getRowColFromId(unsigned int id) {
  auto getRowFromId = [&](unsigned int id) { return id / num_cols; };
  auto getColFromId = [&](unsigned int id) { return id % num_cols; };
  return std::make_tuple<unsigned int, unsigned int>(getRowFromId(id),
                                                     getColFromId(id));
};

template <class T>
ConnectionStatus Grid<T>::queryConnection(unsigned int id_a,
                                          unsigned int id_b) {
  return this->state_matrix[id_a][id_b];
}

template <class T>
ConnectionStatus Grid<T>::queryConnection(Grid<T>::RowColFmt pair_a,
                                          Grid<T>::RowColFmt pair_b) {
  return queryConnection(getIdFromRowCol(pair_a), getIdFromRowCol(pair_b));
}

template <class T>
void Grid<T>::modifyConnection(unsigned int id_a, unsigned int id_b,
                               ConnectionStatus next_status) {
  if (next_status == NOT_CONNECTABLE || next_status == CONNECTION_STATUS_ERR) {
    throw "Attempted to set a status which is not allowed for external use.";
  }
  if (this->state_matrix[id_a][id_b] != CONNECTABLE &&
      next_status == CONNECTED) {
    throw "Attempted to connect a non-connectable state.";
  }
  if (this->state_matrix[id_a][id_b] != CONNECTED &&
      next_status == CONNECTABLE) {
    throw "Attempted to disconnect from a non-connected state.";
  }
  if (this->state_matrix[id_a][id_b] < CONNECTED &&
      this->state_matrix[id_a][id_b] >= PATH_ACTIVE &&
      next_status == PATH_ACTIVE) {
    throw "Can only move into active consideration by being connected.";
  }
  if (this->state_matrix[id_a][id_b] < PATH_ACTIVE &&
      this->state_matrix[id_a][id_b] >= CONNECTION_STATUS_ERR &&
      next_status == PATH_DEAD) {
    throw "Can only be a dead path if it was alive.";
  }
  if (this->state_matrix[id_a][id_b] != PATH_ACTIVE &&
      next_status == PATH_COMPLETE) {
    throw "Only active paths can be part of the solution.";
  }
  this->recently_modified_connections.push_back(
      std::tuple<unsigned int, unsigned int>({id_a, id_b}));
  this->recently_modified_connections.push_back(
      std::tuple<unsigned int, unsigned int>({id_b, id_a}));
  this->recently_modified_cells.push_back(id_a);
  this->recently_modified_cells.push_back(id_b);
  this->state_matrix[id_a][id_b] = next_status;
  this->state_matrix[id_b][id_a] = next_status;
}

template <class T>
void Grid<T>::modifyConnection(Grid<T>::RowColFmt pair_a,
                               Grid<T>::RowColFmt pair_b,
                               ConnectionStatus next_status) {
  modifyConnection(getIdFromRowCol(pair_a), getIdFromRowCol(pair_b),
                   next_status);
}

template <class T>
T Grid<T>::getCell(unsigned int id) {
  return this->cells.at(id);
}

template <class T>
T Grid<T>::getCell(Grid<T>::RowColFmt pair) {
  return getCell(getIdFromRowCol(pair));
}

template <class T>
void Grid<T>::setCell(unsigned int id, T cell) {
  this->recently_modified_cells.push_back(id);
  this->cells.at(id) = cell;
}

template <class T>
void Grid<T>::setCell(Grid<T>::RowColFmt pair, T cell) {
  setCell(getIdFromRowCol(pair), cell);
}

template <class T>
typename Grid<T>::IdListFmt Grid<T>::getCellIdsMatching(
    unsigned int id, ConnectionStatus status) {
  std::vector<unsigned int> matching;
  for (unsigned int i = 0; i < this->num_cells; i++) {
    if (this->state_matrix[id][i] == status) {
      matching.push_back(i);
    }
  }
  return matching;
}

template <class T>
typename Grid<T>::IdListFmt Grid<T>::getRecentlyModifiedCells() {
  auto recent = this->recently_modified_cells;
  this->recently_modified_cells.clear();
  return recent;
}

template <class T>
typename Grid<T>::ConnectionListFmt Grid<T>::getRecentlyModifiedConnections() {
  auto recent = this->recently_modified_connections;
  this->recently_modified_connections.clear();
  return recent;
}
