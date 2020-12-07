// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "orientation.h"

#ifndef ERROR_H
#define ERROR_H

namespace karel {

enum RobotError {
  kNoError = 0,
  kCannotMoveNorth,
  kCannotMoveEast,
  kCannotMoveSouth,
  kCannotMoveWest,
  kCannotPutBeeper,
  kCannotPickBeeper,
};

}  // namespace karel

#endif
