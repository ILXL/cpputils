# C++ Utils

Libraries for student C++ code available under the MIT license.

## Graphics: Image library

``graphics/image.h`` can be used to create, view, load and save images. Example:

![example fractal tree](graphics/test/example_fractal_tree.png)

``graphics/image_event.h`` allows for interactive graphical programs with animation and mouse event handling.

### Usage

Learn more about how to use the C++ Utils graphics class in this [interactive tutorial](https://lab.cs50.io/ILXL-guides/intro-to-graphics).

## Karel the Robot in C++

Karel the Robot is a gentle introductory programming language created by [Professor Richard Pattis](https://www.ics.uci.edu/~pattis/) in [*Karel the Robot: A Gentle Introduction to The Art of Programming*](https://www.google.com/books/edition/_/ghcZAQAAIAAJ?hl=en&gbpv=1) and implemented in C++ here with his permission. The Karel language emphasizes logic while hiding tricky syntax, allowing anyone to begin making exciting graphical programs with very little background.

![karel the robot screenshot](karel/src/test/karel.png)

Karel lives in a two-dimensional grid and has a position and an orientation (north, east, south or west). Each cell of the grid may contain one or more beepers, or no beepers at all. Karel has a bag of beepers (which may be empty). Cells may be separated by walls.

Karel has four actions: they can move forward, turn left, put down a beeper or pick up a beeper. In addition, Karel is able to check state of the area around themselves. Karel cannot move through walls or off the edge of the world.

### Accessibility

Karel programs may export the world to CSV after each action. This may be helpful to screen-reader users.

Karel programs may prompt before each robot action. This may be helpful for users who need to control the speed of execution.

### Usage

To create Karel the Robot programs in C++, include ``karel/karel.h``.

Learn more about how to use the C++ Utils Karel the Robot functions in this [interactive tutorial](https://lab.cs50.io/ILXL-guides/intro-to-karel)

## For developers and instructors

Read more about developing and including C++ Utils and unittesting assignments in the [wiki](https://github.com/ILXL/cpputils/wiki).
