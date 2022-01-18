#ifndef GRID_H
#define GRID_H
#include <tuple>
#include <vector>

// ConnectionStatus variables are entries in the Grid's adjacency matrix.
enum ConnectionStatus {
  NOT_CONNECTABLE,  // The two cells are not adjacent and cannot be connected.
  CONNECTABLE,      // The two cells are adjacent but not currently connected.
  CONNECTED,        // The two cells are adjacent and connected.
  PATH_ACTIVE,      // The connection is being considered as part of a solution.
  PATH_DEAD,        // The connection is not part of a solution.
  PATH_COMPLETE,    // The connection is confirmed to be part of the solution.
  CONNECTION_STATUS_ERR  // An error has occurred if this is used.
};

/* Grid Overview
 *
 * A grid is defined with a number of rows and a number of columns,
 * everything else can be inferred from that.
 *
 * num_rows is the number of rows in the grid.
 * num_cols is the number of columns in the grid.
 *
 *
 * A grid is constructed from bottom left to top right.
 * For Example, a grid with 4 rows and 4 columns.
 *
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
 *
 * Ids can be mapped to and from row column format for convenience.
 * For example.
 *
 *   cell 5 above can be mapped to row number by dividing by num_cols.
 *   cell 5 row = 5/4 = 1
 *
 *   likewise the column can be determined by taking the modulo of the
 *   id and num_cols.
 *   cell 5 col = 5%4 = 1
 *
 * Each of these cells is then tracked in a modified adjacency matrix
 * which utilizes the enumerated type above rather than a traditional
 * boolean type in order to maintain more state information.
 *
 * The adjacency matrix is an NxN matrix where N is the number of cells.
 * Like this:

                    15 | 0 0                1  0
                    14 | 0 0                0  1
                     . | . .      .  . .
                     . | . .      .  . .
                     . | . .      .  . .
                     1 | 1 0      .  . .    0  0
                     0 | 0 1                0  0
                     -------------------------
                         0 1       ...     14 15

 * In the example above, the entries are a numerical representation of the
 * enumerated type.
 *
    CANNOT_BE_CONNECTED      = 0
    CONNECTABLE              = 1
    CONNECTED                = 2
    PATH_ACTIVE,             = 3
    PATH_DEAD,               = 4
    PATH_COMPLETE,           = 5
    CONNECTION_STATUS_ERR    = 6
 * */

template <class T>
class Grid {
 public:
  // number of rows and columns in the grid.
  unsigned int num_rows;
  unsigned int num_cols;
  unsigned int num_cells;

 private:
  using CellListFmt = std::vector<T>;
  using IdListFmt = std::vector<unsigned int>;
  using ConnectionListFmt = std::vector<std::tuple<unsigned int, unsigned int>>;
  using RowColFmt = std::tuple<unsigned int, unsigned int>;
  using MatrixFmt = std::vector<std::vector<ConnectionStatus>>;

  // state_matrix is the adjacency matrix described above.
  MatrixFmt state_matrix;
  // list of cell data.
  CellListFmt cells;
  // this is a helper function which aids in constructing the state matrix.
  MatrixFmt createStateMatrix(unsigned int, unsigned int);
  // list of cell ids that have been modifed since last call.
  IdListFmt recently_modified_cells;
  // list of connection id pairs that have been modified since last call.
  ConnectionListFmt recently_modified_connections;

 public:
  // A grid is created with a number of rows and columns,
  // from which a state matrix and cells are generated.
  Grid(unsigned int num_rows, unsigned int num_cols)
      : num_rows(num_rows),
        num_cols(num_cols),
        num_cells(num_rows * num_cols),
        state_matrix(this->createStateMatrix(num_rows, num_cols)),
        cells({CellListFmt(num_rows * num_cols, T())}),
        recently_modified_cells(IdListFmt()){};

  // and gives back an id.
  unsigned int getIdFromRowCol(RowColFmt);
  // this is a helper function which takes a id and gives back
  // a row column format.
  RowColFmt getRowColFromId(unsigned int);
  // query the connection status of two ids
  ConnectionStatus queryConnection(unsigned int, unsigned int);
  ConnectionStatus queryConnection(RowColFmt, RowColFmt);
  // modify the connection status of two ids
  void modifyConnection(unsigned int, unsigned int, ConnectionStatus);
  void modifyConnection(RowColFmt, RowColFmt, ConnectionStatus);
  // retrieve a cell.
  T getCell(unsigned int);
  T getCell(RowColFmt);
  // set a cell
  void setCell(unsigned int, T);
  void setCell(RowColFmt, T);
  // get cell ids matching from target id matching ConnectionStatus
  IdListFmt getCellIdsMatching(unsigned int, ConnectionStatus);
  // get list of cell ids that have been modifed since last call.
  IdListFmt getRecentlyModifiedCells();
  // get list of connections that have been modifed since last call.
  ConnectionListFmt getRecentlyModifiedConnections();
};

#include "grid_impl.h"
#endif
