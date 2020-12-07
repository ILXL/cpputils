// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "karel.h"

void KarelProgram() {
  Move();
  Move();
  Move();
  Move();
  Move();
  PutBeeper();
  PickBeeper();
  TurnLeft();
  Move();
  PutBeeper();
  TurnLeft();
  Move();
  TurnLeft();
  TurnLeft();
}

int main() {
  LoadWorld("src/test/worlds/1x8.w");
  KarelProgram();
  Finish();
  return 0;
}
