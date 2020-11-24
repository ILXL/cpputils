#include "karel.h"

void KarelProgram() {
  Move();
  Move();
  Move();
  TurnLeft();
  Move();
  Move();
  TurnLeft();
  // Move();
  // Move();
  // TurnLeft();
  // Move();
  // TurnLeft();
}

int main() {
  //LoadFromFile("file.kar");
  KarelProgram();
  Finish();
  return 0;
}
