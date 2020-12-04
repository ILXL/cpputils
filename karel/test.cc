#include "karel.h"

void KarelProgram() {
  TurnLeft();
  Move();
  PickBeeper();
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
}

int main() {
  LoadWorld("worlds/CollectNewspaperKarel.w");
  KarelProgram();
  Finish();
  return 0;
}
