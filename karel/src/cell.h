#include "orientation.h"

#ifndef CELL_H
#define CELL_H

namespace karel {

// Class representing a cell in Karel's world, including a count of beepers
// and whether or not there are walls.
class Cell {
 public:
  int GetNumBeepers() const { return num_beepers_; }
  void SetNumBeepers(int beepers) { num_beepers_ = beepers; }
  bool HasNorthWall() const { return north_wall_; }
  bool HasEastWall() const { return east_wall_; }
  bool HasSouthWall() const { return south_wall_; }
  bool HasWestWall() const { return west_wall_; }
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
  bool num_beepers_ = 0;
  bool north_wall_ = false;
  bool east_wall_ = false;
  bool south_wall_ = false;
  bool west_wall_ = false;
};

}  // namespace karel

#endif  // CELL_H
