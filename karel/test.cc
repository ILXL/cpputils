#include "karel.h"

void KarelProgram() {
  Move();
  Move();
  TurnLeft();
  Move();
  Move();
  PickBeeper();
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
  //LoadWorld("worlds/CollectNewspaperKarel.w");
  // LoadWorld("worlds/8x8.w");
  LoadWorld("worlds/StoneMasonKarel.w");
  KarelProgram();
  Finish();
  return 0;
}
