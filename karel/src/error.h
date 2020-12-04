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
