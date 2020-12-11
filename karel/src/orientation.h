// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef ORIENTATION_H
#define ORIENTATION_H

namespace karel {

/**
 * Defines an orientation in the grid using compass directions: north is
 * up, east is right, west is down, and south is left.
 */
enum Orientation {
  kNorth = 0,
  kEast = 1,
  kSouth = 2,
  kWest = 3,
};

/**
 * Defines a position and orientation in the world grid.
 */
struct PositionAndOrientation {
  int x = 0;
  int y = 0;
  Orientation orientation = Orientation::kNorth;
};

}  // namespace karel

#endif  // ORIENTATION_H
