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
  //LoadFromFile("file.kar");
  KarelProgram();
  Finish();
  return 0;
}
