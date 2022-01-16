const char* SlopeDNEException::what() const throw () {
  return  "Line is vertical, slope does not exist.";
}

template <typename T1, typename T2, typename T3>
typename Maze<T1, T2, T3>::PixelMap Maze<T1, T2, T3>::initMap() {
  // first set everything as wall color
  Maze<T1, T2, T3>::PixelMap map(
      height, Maze<T1, T2, T3>::PixelRow(width, Maze<T1, T2, T3>::wall_color));

  // go through and set each pixel corresponding to a cell as the not connected
  // color
  for (unsigned int i = 0; i < this->grid.num_cells; i++) {
    const auto [current_row, current_col] = this->grid.getRowColFromId(i);
    unsigned int current_x_pos =
        current_row * Maze<T1, T2, T3>::distance_between_pixels;
    unsigned int current_y_pos =
        current_col * Maze<T1, T2, T3>::distance_between_pixels;
    map[current_x_pos][current_y_pos] = Maze<T1, T2, T3>::not_connected_color;
  }
  return map;
}

template <typename T1, typename T2, typename T3>
void Maze<T1, T2, T3>::drawMap() {
  // for each pixel in the map, set it on the canvas
  for (unsigned int i = 0; i < this->height; i++) {
    for (unsigned int j = 0; j < this->width; j++) {
      const auto [pixel_r, pixel_g, pixel_b] = this->map[i][j];
      this->canvas->SetPixel(i, j, pixel_r, pixel_g, pixel_b);
    }
  }
}

template <typename T1, typename T2, typename T3>
typename Maze<T1, T2, T3>::Coord Maze<T1, T2, T3>::getCoordOfCellById(
    unsigned int id) {
  const auto [row, col] = this->grid.getRowColFromId(id);
  unsigned int x = row * Maze<T1, T2, T3>::distance_between_pixels;
  unsigned int y = col * Maze<T1, T2, T3>::distance_between_pixels;
  return Maze<T1, T2, T3>::Coord(x, y);
}

template <typename T1, typename T2, typename T3>
int Maze<T1, T2, T3>::getSlope(typename::Maze<T1, T2, T3>::Coord a,
    typename::Maze<T1, T2, T3>::Coord b) {
  const auto [x1, y1] = a;
  const auto [x2, y2] = b;
  if (x2-x1 == 0) {
    throw SlopeDNEException();
  }
  return ((y2-y1)/(x2-x1));
}

template <typename T1, typename T2, typename T3>
void Maze<T1, T2, T3>::updateConnectionInPixelMap(unsigned int p1, unsigned int p2) {
  const auto p1Coord = this->getCoordOfCellById(p1);
  const auto [p1_x, p1_y] = p1Coord;
  const auto p2Coord = this->getCoordOfCellById(p2);
  const auto [p2_x, p2_y] = p2Coord;
  const auto status = this->grid.queryConnection(p1, p2);
  Pixel draw_color;
  if (status == CONNECTED) {
    draw_color = Maze<T1, T2, T3>::connected_color;
  } else {
    draw_color = Maze<T1, T2, T3>::wall_color;
  }

  // try to get the slope between the two points for drawing.
  // because we are in a grid, and cells can only be connected to their
  // neighbors, slope is either 0 or DNE, but in the future I may want
  // to connect to say, the upper left node, or lower right. instead of
  // just up, left, down, and right.
  try {
    // y = mx + b
    // y - mx = b
    int slope = this->getSlope(p1Coord, p2Coord);
    int b = p1_y - (slope*p1_x);
    for (unsigned int x = p1_x; x <= p2_x; x++) {
      int y = (slope*x) + b;
      this->map[x][y] = draw_color;
      Coord c(x, y);
      this->pixels_to_update.push_back(c);
    }
  } catch (SlopeDNEException &e) {
    //if slope does not exist, its a vertical line, so set x constant and move y
    int x = p1_x;
    int y1, y2;
    if (p1_y < p2_y) {
      y1 = p1_y;
      y2 = p2_y;
    } else {
      y1 = p2_y;
      y2 = p1_y;
    }
    for (int y = y1; y <= y2; y++) {
      this->map[x][y] = draw_color;
      Coord c(x, y);
      this->pixels_to_update.push_back(c);
    }
  }
}

template <typename T1, typename T2, typename T3>
void Maze<T1, T2, T3>::updateCellInPixelMap(unsigned int p) {
  const auto coord = this->getCoordOfCellById(p);
  this->pixels_to_update.push_back(coord);
  const auto [x, y] = coord;
  if (this->grid.getCell(p).emphasized) {
    this->map[x][y] = Maze<T1, T2, T3>::emphasized_color;
    return;
  }
  auto connected = this->grid.getCellIdsMatching(p, CONNECTED);
  if(connected.size() > 0) {
    this->map[x][y] = Maze<T1, T2, T3>::connected_color;
  }
  else {
    this->map[x][y] = Maze<T1, T2, T3>::not_connected_color;
  }
}

template <typename T1, typename T2, typename T3>
void Maze<T1, T2, T3>::drawMapUpdates() {
    auto compareCoords = [&] (Maze<T1, T2, T3>::Coord a, Maze<T1, T2, T3>::Coord b) {
      auto const [x1, y1] = a;
      auto const [x2, y2] = b;
      return (x1 == x2 && y1 == y2);
    };
    std::sort(this->pixels_to_update.begin(), this->pixels_to_update.end());
    Maze<T1, T2, T3>::CoordList::iterator i;
    i = std::unique(this->pixels_to_update.begin(),
            this->pixels_to_update.begin() + this->pixels_to_update.size(),
            compareCoords);
    this->pixels_to_update.resize(std::distance(this->pixels_to_update.begin(),
            i));
    for(auto pixel : this->pixels_to_update) {
        auto const [x, y] = pixel;
        auto const [r, g, b] = this->map[x][y];
        this->canvas->SetPixel(x, y, r, g, b);
    }
    this->pixels_to_update.clear();
}

template <typename T1, typename T2, typename T3>
typename Maze<T1, T2, T3>::PixelMap Maze<T1, T2, T3>::generatePixelMap() {
  for (unsigned int i = 0; i < this->grid.num_cells; i++) {
    for (unsigned int j = i; j < this->grid.num_cells; j++) {
      const auto status = grid.queryConnection(i, j);
      if (status != CONNECTED) {
        continue;
      }
      this->updateConnectionInPixelMap(i, j);
    }
  }
  for (unsigned int i = 0; i < this->grid.num_cells; i++) {
    this->updateCellInPixelMap(i);
  }
  return map;
}

template <typename T1, typename T2, typename T3>
float Maze<T1, T2, T3>::generateStep() {
  if (this->generated) {
    throw GenerationCompleteException();
  }

  try {
    return this->generation_strategy.step();
  } catch (GenerationCompleteException& e) {
    this->generated = true;
    return 0;
  }
}

template <typename T1, typename T2, typename T3>
void Maze<T1, T2, T3>::updatePixelMap() {
  auto modified_connections = this->grid.getRecentlyModifiedConnections();
  for (auto & conn : modified_connections) {
    const auto [id1, id2] = conn;
    this->updateConnectionInPixelMap(id1, id2);
  }
  auto modified_cells = this->grid.getRecentlyModifiedCells();
  for (auto & cell : modified_cells) {
    this->updateCellInPixelMap(cell);
  }
  this->drawMapUpdates();
}
