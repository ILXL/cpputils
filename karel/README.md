# Karel the Robot in C++

TODO: Port README to a ILXL guide and the CPPUtils wiki.

## History

Karel the Robot is a gentle introductory programming language created by [Dr. Richard Pattis](https://www.ics.uci.edu/~pattis/) in [*Karel the Robot: A Gentle Introduction to The Art of Programming*](https://www.google.com/books/edition/_/ghcZAQAAIAAJ?hl=en&gbpv=1). The Karel language emphasizes logic while hiding tricky syntax, allowing students to begin making exciting graphical programs with very little background.

Karel is named "after the Czech playwright Karel Čapek, whose 1923 play R.U.R. (Rossum’s Universal Robots) gave the word robot to the English language." ([Karel Reader](https://web.stanford.edu/class/archive/cs/cs106a/cs106a.1194/karelReader/en/chapter1.html))

This C++ version of Karel the Robot was created with permission from Dr. Pattis.

## Overview

Karel lives in a two-dimensional grid where (1, 1) is at the bottom left corner. Karel has a position and an orientation in the grid (north, east, south or west).

Each square of the grid may contain one or more beepers, or no beepers at all. Karel has a bag of beepers which may be empty or may contain many or even infinite beepers.

Karel can move one cell and turn left. Karel can put down a beeper or pick up a beeper. In addition, Karel is able to check state of the area around themselves.

Cells may be separated by walls. Karel cannot move through walls or off the edge of the world.

Karel has four actions:

```cpp
void Move();
void TurnLeft();
void PutBeeper();
void PickBeeper();
```

Karel has several booleans for getting robot state:
```cpp
bool FrontIsClear();
bool FrontIsBlocked();
bool LeftIsClear();
bool LeftIsBlocked();
bool RightIsClear();
bool RightIsBlocked();
bool HasBeepersInBag();
bool NoBeepersInBag();
bool BeepersPresent();
bool NoBeepersPresent();
bool FacingNorth();
bool NotFacingNorth();
bool FacingEast();
bool NotFacingEast();
bool FacingSouth();
bool NotFacingSouth();
bool FacingWest();
bool NotFacingWest();
```

``robot.h`` provides a Karel the Robot implementation in C++. This is functional programming: simply ``#include "cpputils/karel/karel.h"`` and call the Karel functions in the global namespace.

### Building

Compile (from the directory containing cpputils) with:

``clang++ -std=c++17 main.cc cpputils/karel/karel.cc cpputils/karel/src/robot.cc cpputils/graphics/image.cc -o main -lm -lX11 -lpthread``

A few more flags are needed in Mac.

## For instructors

Instructors should call ``LoadWorld`` before student code and ``Finish`` after.

```cpp
void LoadWorld(std::string filename);
void Finish();
```

### World files

Same as Stanford 106A. Dimension must be the first line.

### Unit testing

See ``src/test/karel_unittest.cc``

Must get robot instance with enable animations set to false -- so unittest shouldn't run over code that does Setup (and maybe not Finish either). Should just run over student functions.

### Example CS1 course structure with Karel

first weeks:
`make build`
karel:
  logic
  vars (ints)
  loops & ifs
  functions  --> TurnRight() { TurnLeft(); TurnLeft(); TurnLeft(); }

later weeks:
clang++ main.cc -o main
doubles
vectors
strings
