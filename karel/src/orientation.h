// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef ORIENTATION_H
#define ORIENTATION_H

namespace karel {

enum Orientation {
  kNorth = 0,
  kEast = 1,
  kSouth = 2,
  kWest = 3,
};

struct PositionAndOrientation {
  int x = 0;
  int y = 0;
  Orientation orientation = Orientation::kNorth;
};

}  // namespace karel

#endif  // ORIENTATION_H
