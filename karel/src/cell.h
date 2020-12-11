// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "orientation.h"

#ifndef CELL_H
#define CELL_H

namespace karel {

/**
 * Class representing a cell in Karel's world, including a count of beepers
 * and whether or not there are walls.
 */
class Cell {
 public:
  int GetNumBeepers() const { return num_beepers_; }
  void SetNumBeepers(int beepers) { num_beepers_ = beepers; }

  bool HasNorthWall() const { return north_wall_; }
  bool HasEastWall() const { return east_wall_; }
  bool HasSouthWall() const { return south_wall_; }
  bool HasWestWall() const { return west_wall_; }

  /**
   * Adds a wall on the given side of the cell. For example, a north wall would
   * be at the top of the cell, and an east wall would be on the right of the
   * cell.
   */
  void AddWall(Orientation wall_orientation) {
    switch (wall_orientation) {
      case kNorth:
        north_wall_ = true;
        break;
      case kEast:
        east_wall_ = true;
        break;
      case kSouth:
        south_wall_ = true;
        break;
      case kWest:
        west_wall_ = true;
        break;
    }
  }

 private:
  int num_beepers_ = 0;
  bool north_wall_ = false;
  bool east_wall_ = false;
  bool south_wall_ = false;
  bool west_wall_ = false;
};

}  // namespace karel

#endif  // CELL_H
