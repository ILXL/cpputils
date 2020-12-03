#include "karel.h"

void KarelProgram() {
  // Move();
  // Move();
  // TurnLeft();
  // Move();
  // Move();
  // PickBeeper();
  // Move();
  // PutBeeper();
  // PickBeeper();
  // TurnLeft();
  // Move();
  // PutBeeper();
  // TurnLeft();
  // Move();
}

int main() {
  LoadWorld("worlds/8x1.w");
  KarelProgram();
  Finish();
  return 0;
}
