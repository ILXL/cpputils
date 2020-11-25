#include "karel.h"

#include <iostream>
#include <vector>
#include <math.h>

namespace karel {

enum Orientation {
  kNorth,
  kEast,
  kSouth,
  kWest,
};

class Cell {
 public:
  int GetNumBeepers() { return num_beepers_; }
  void SetNumBeepers(int beepers) { num_beepers_ = beepers; }
  // bool HasNorthWall() { return north_wall_; }
  // bool HasSouthWall() { return south_wall_; }
  // bool HasSouthWall() { return south_wall_; }
  // bool HasWestWall() { return west_wall_; }
 private:
  bool num_beepers_ = 0;
  // bool north_wall_ = false;
  // bool east_wall_ = false;
  // bool south_wall_ = false;
  // bool west_wall_ = false;
};

const int dimen = 10;
const int pxPerCell = 50;
const int markSize = 10;
const int robotSize = 30;
const int beeperSize = 30;
const int eyeSize = 4;
const int legLength = 6;
const int limbWidth = 5;
const graphics::Color eyeColor(50, 50, 50);
const graphics::Color karelColor(125, 125, 125);
const graphics::Color markColor(150, 150, 255);
const graphics::Color beeperColor(172, 147, 194);
const graphics::Color limbColor(105, 105, 105);
const graphics::Color kWhite(255, 255, 255);

class Robot {
 public:
  static karel::Robot& GetInstance() {
    static Robot instance;
    instance.Initialize();
    instance.Show();
    return instance;
  }

  // Disallow copy and assign.
  Robot(const Robot&) = delete;
  karel::Robot& operator=(const Robot&) = delete;

  void Initialize() {
    image_.Initialize(dimen * pxPerCell, dimen * pxPerCell);
    for (int i = 0; i < dimen; i++) {
      world_.push_back(std::vector<Cell>());
      for (int j = 0; j < dimen; j++) {
        world_[i].push_back(Cell());
      }
    }
    world_[5][3].SetNumBeepers(1);
    world_[2][7].SetNumBeepers(1);
    DrawWorld();
    DrawRobot(x_ * pxPerCell + pxPerCell / 2, y_ * pxPerCell + pxPerCell / 2);
    image_.ShowForMs(500, "Karel's World");
  }

  void Show() { image_.Show("Karel's World"); }

  void Move() {
    switch (orientation_) {
      case Orientation::kNorth:
        if (y_ == 0) {
          // Cannot move any further. draw an error.
        } else {
          AnimateMove(x_, y_ - 1);
        }
        break;
      case Orientation::kEast:
        if (x_ == dimen - 1) {
        } else {
          AnimateMove(x_ + 1, y_);
        }
        break;
      case Orientation::kSouth:
        if (y_ == dimen - 1) {
        } else {
          AnimateMove(x_, y_ + 1);
        }
        break;
      case Orientation::kWest:
        if (x_ == 0) {
        } else {
          AnimateMove(x_ - 1, y_);
        }
        break;
    }
  }

  void TurnLeft() {
    switch (orientation_) {
      case Orientation::kNorth:
        orientation_ = Orientation::kWest;
        break;
      case Orientation::kEast:
        orientation_ = Orientation::kNorth;
        break;
      case Orientation::kSouth:
        orientation_ = Orientation::kEast;
        break;
      case Orientation::kWest:
        orientation_ = Orientation::kSouth;
        break;
    }
    DrawWorld();
    DrawRobot();
    // todo make this 500 a param or refactor this line as a fn.
    image_.ShowForMs(500, "Karel's World");
  }

  void Finish() { image_.ShowUntilClosed("Karel's World"); }

 private:
  Robot() {}

  void DrawWorld() {
    image_.DrawRectangle(0, 0, dimen * pxPerCell, dimen * pxPerCell, kWhite);
    for (int i = 0; i < dimen; i++) {
      for (int j = 0; j < dimen; j++) {
        int x_center = i * pxPerCell + pxPerCell / 2;
        int y_center = j * pxPerCell + pxPerCell / 2;
        image_.DrawLine(x_center - markSize / 2, y_center,
                        x_center + markSize / 2, y_center, markColor, 3);
        image_.DrawLine(x_center, y_center - markSize / 2, x_center,
                        y_center + markSize / 2, markColor, 3);
        // trig!
        double dimen = sqrt((beeperSize / 2) * (beeperSize / 2) / 2);
        if (world_[i][j].GetNumBeepers() > 0) {
          image_.DrawLine(x_center - dimen,
                          y_center - dimen, x_center + dimen, y_center + dimen,
                                beeperColor, beeperSize);
          image_.DrawLine(x_center - dimen,
                          y_center - dimen, x_center +dimen, y_center + dimen,
                                beeperColor, beeperSize);
        }
        // TODO: Check for walls and draw them here in a thick darker grey.
      }
    }
    for (int i = 0; i < dimen; i++) {
      image_.DrawLine(0, i * pxPerCell, 499, i * pxPerCell,
                      graphics::Color(200, 200, 200));
      image_.DrawLine(i * pxPerCell, 0, i * pxPerCell, 499,
                      graphics::Color(200, 200, 200));
    }
  }

  void DrawRobot() {
    // Center in the cell.
    DrawRobot(x_ * pxPerCell + pxPerCell / 2, y_ * pxPerCell + pxPerCell / 2);
  }

  // pixel_x and pixel_y are the center of the cell in pixels.
  void DrawRobot(int pixel_x, int pixel_y) {
    image_.DrawRectangle(pixel_x - robotSize / 2, pixel_y - robotSize / 2,
                         robotSize, robotSize, karelColor);
    switch (orientation_) {
      case Orientation::kNorth:
        image_.DrawCircle(pixel_x, pixel_y - robotSize / 2 + eyeSize / 2 + 1,
                          eyeSize, kWhite);
        image_.DrawCircle(pixel_x, pixel_y + 1, eyeSize, kWhite);
        image_.DrawCircle(pixel_x, pixel_y - robotSize / 2 + eyeSize / 2,
                          eyeSize, eyeColor);
        image_.DrawLine(pixel_x + robotSize / 2, pixel_y - legLength,
                        pixel_x + robotSize / 2 + legLength,
                        pixel_y - legLength, limbColor, limbWidth);
        image_.DrawLine(pixel_x + robotSize / 2, pixel_y + legLength,
                        pixel_x + robotSize / 2 + legLength,
                        pixel_y + legLength, limbColor, limbWidth);
        break;
      case Orientation::kEast:
        image_.DrawCircle(pixel_x - 1, pixel_y, eyeSize, kWhite);
        image_.DrawCircle(pixel_x + robotSize / 2 - eyeSize / 2 - 1, pixel_y,
                          eyeSize, kWhite);
        image_.DrawCircle(pixel_x + robotSize / 2 - eyeSize / 2, pixel_y,
                          eyeSize, eyeColor);
        image_.DrawLine(
            pixel_x - legLength, pixel_y + robotSize / 2, pixel_x - legLength,
            pixel_y + robotSize / 2 + legLength, limbColor, limbWidth);
        image_.DrawLine(
            pixel_x + legLength, pixel_y + robotSize / 2, pixel_x + legLength,
            pixel_y + robotSize / 2 + legLength, limbColor, limbWidth);
        break;
      case Orientation::kSouth:
        image_.DrawCircle(pixel_x, pixel_y + robotSize / 2 - eyeSize / 2 - 1,
                          eyeSize, kWhite);
        image_.DrawCircle(pixel_x, pixel_y - 1, eyeSize, kWhite);
        image_.DrawCircle(pixel_x, pixel_y + robotSize / 2 - eyeSize / 2,
                          eyeSize, eyeColor);
        image_.DrawLine(pixel_x - robotSize / 2, pixel_y - legLength,
                        pixel_x - robotSize / 2 - legLength,
                        pixel_y - legLength, limbColor, limbWidth);
        image_.DrawLine(pixel_x - robotSize / 2, pixel_y + legLength,
                        pixel_x - robotSize / 2 - legLength,
                        pixel_y + legLength, limbColor, limbWidth);
        break;
      case Orientation::kWest:
        image_.DrawCircle(pixel_x + 1, pixel_y, eyeSize, kWhite);
        image_.DrawCircle(pixel_x - robotSize / 2 + eyeSize / 2 + 1, pixel_y,
                          eyeSize, kWhite);
        image_.DrawCircle(pixel_x - robotSize / 2 + eyeSize / 2, pixel_y,
                          eyeSize, eyeColor);
        image_.DrawLine(
            pixel_x - legLength, pixel_y - robotSize / 2, pixel_x - legLength,
            pixel_y - robotSize / 2 - legLength, limbColor, limbWidth);
        image_.DrawLine(
            pixel_x + legLength, pixel_y - robotSize / 2, pixel_x + legLength,
            pixel_y - robotSize / 2 - legLength, limbColor, limbWidth);
        break;
    }
    image_.DrawCircle(pixel_x, pixel_y, eyeSize, eyeColor);
  }

  void AnimateMove(int next_x, int next_y) {
    int steps = 10;
    for (int i = 1; i <= steps; i++) {
      DrawWorld();
      double fraction = i * 1.0 / steps;
      double x = x_ * (1 - fraction) + next_x * fraction;
      double y = y_ * (1 - fraction) + next_y * fraction;
      DrawRobot(x * pxPerCell + pxPerCell / 2, y * pxPerCell + pxPerCell / 2);
      image_.ShowForMs(60, "Karel's World");
    }
    x_ = next_x;
    y_ = next_y;
    image_.ShowForMs(500, "Karel's World");
  }

  graphics::Image image_;
  int x_ = 0;
  int y_ = 9;
  Orientation orientation_ = Orientation::kEast;
  std::vector<std::vector<Cell>> world_;
};

}  // namespace karel

void Move() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.Move();
}

void TurnLeft() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.TurnLeft();
}

void PutBeeper() { karel::Robot& r = karel::Robot::GetInstance(); }

void PickBeeper() { karel::Robot& r = karel::Robot::GetInstance(); }

void Finish() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.Finish();
}

bool FrontIsClear() {
  karel::Robot& r = karel::Robot::GetInstance();
  return false;
}

bool BeepersPresent() {
  karel::Robot& r = karel::Robot::GetInstance();
  return false;
}
