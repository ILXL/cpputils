#include "karel.h"

#include <math.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

namespace karel {

enum Orientation {
  kNorth = 1,
  kEast = 2,
  kSouth = 3,
  kWest = 4,
};

class Cell {
 public:
  int GetNumBeepers() const { return num_beepers_; }
  void SetNumBeepers(int beepers) { num_beepers_ = beepers; }
  bool HasNorthWall() const { return north_wall_; }
  bool HasEastWall() const { return south_wall_; }
  bool HasSouthWall() const { return south_wall_; }
  bool HasWestWall() const { return west_wall_; }
  void SetHasNorthWall(bool has_wall) { north_wall_ = has_wall; }
  void SetHasEastWall(bool has_wall) { south_wall_ = has_wall; }
  void SetHasSouthWall(bool has_wall) { south_wall_ = has_wall; }
  void SetHasWestWall(bool has_wall) { west_wall_ = has_wall; }

 private:
  bool num_beepers_ = 0;
  bool north_wall_ = false;
  bool east_wall_ = false;
  bool south_wall_ = false;
  bool west_wall_ = false;
};

// ms for a long animation duration to show Karel.
const int kLongDuration = 500;

// ms for a short animation duration to show Karel.
const int kShortDuration = 50;

const int kDefaultDimen = 10;
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
  // Get the Robot singleton.

  static karel::Robot& PrivateGetInstance() {
    static Robot instance;
    return instance;
  }

  static karel::Robot& GetInstance() {
    karel::Robot& instance = PrivateGetInstance();
    instance.Initialize("");
    return instance;
  }

  static karel::Robot& InitializeInstance(std::string filename) {
    karel::Robot& instance = PrivateGetInstance();
    instance.Initialize(filename);
    return instance;
  }

  // Disallow copy and assign.
  Robot(const Robot&) = delete;
  karel::Robot& operator=(const Robot&) = delete;

  void Initialize(std::string filename) {
    // Ensure only intitialized once.
    if (initialized_) return;
    initialized_ = true;

    if (!filename.size()) {
      // No file. Default 10x10 blank world with no walls and no beepers.
      x_dimen_ = kDefaultDimen;
      y_dimen_ = kDefaultDimen;
      for (int i = 0; i < x_dimen_; i++) {
        world_.push_back(std::vector<Cell>());
        for (int j = 0; j < y_dimen_; j++) {
          world_[i].push_back(Cell());
        }
      }
      // Nearly infinite beepers.
      beeper_count_ = std::numeric_limits<int>::max();
      x_ = 0;
      y_ = y_dimen_ - 1;
    } else {
      std::fstream world_file;
      world_file.open(filename);
      if (!world_file.is_open()) {
        // TODO: Replace throwables with better error codes?
        throw std::string("Error opening file " + filename);
      }
      std::string line;

      const std::string dimension_prefix = "Dimension:";
      const std::string beeper_prefix = "Beeper:";
      const std::string wall_prefix = "Wall:";
      const std::string bag_prefix = "BeeperBag:";
      const std::string karel_prefix = "Karel:";
      const std::string speed_prefix = "Speed:";

      std::string line_prefix;
      char open_paren, comma, closed_paren;
      if (!(world_file >> line_prefix >> open_paren >> x_dimen_ >> comma >>
            y_dimen_ >> closed_paren)) {
        throw std::string("Could not find Dimension in first line");
      }
      if (line_prefix != dimension_prefix) {
        throw std::string("Could not find Dimension in first line");
      }
      for (int i = 0; i < x_dimen_; i++) {
        world_.push_back(std::vector<Cell>());
        for (int j = 0; j < y_dimen_; j++) {
          world_[i].push_back(Cell());
        }
      }

      // Read the rest of the file to get beepers and walls.
      // May need better error checking.
      while (world_file >> line_prefix) {
        if (line_prefix == wall_prefix) {
            std::cout << "wall " << std::endl;
        } else if (line_prefix == beeper_prefix) {
          // TODO
          std::cout << "beeper in cell " << std::endl;
        } else if (line_prefix == bag_prefix) {
          std::string beepers;
          world_file >> beepers;
          if (beepers == "INFINITY") {
            beeper_count_ = std::numeric_limits<int>::max();
          } else {
            beeper_count_ = stoi(beepers);
          }
          std::cout << "num beepers set to " << beeper_count_;
        } else if (line_prefix == karel_prefix) {
            std::cout << "karel_prefix " << std::endl;

            // set x, y and direction
        } else if (line_prefix == speed_prefix) {
           world_file >> speed_;
           std::cout << "Speed set to " << speed_;
        } else {
            std::cout << "Default?? " << line_prefix << std::endl;
            break;
        }
      }

      // if (!getline(world_file, line)) {
      //   throw std::string("File " + filename + " is empty.");
      // }
      // size_t dimension_index = line.find(dimension_prefix);
      // if (dimension_index != std::string::npos) {
      //   std::string dimension_suffix = line.substr(dimension_prefix.size());
      //
      // } else {
      //   // Error: first line should be dimension.
      //   throw std::string("Could not find Dimension in first line");
      // }

      // example worlds:
      // https://github.com/thisdotrob/karel-the-robot/tree/master/worlds

      // get walls and beepers from file (optional in file)

      // karel initial state
      x_ = 2;  // from file but map x + y and dimen_ - y
      y_ = y_dimen_ - 3;

      // get speed from file too.
      speed_ = 1.25;
      world_file.close();
    }

    image_.Initialize(x_dimen_ * pxPerCell, y_dimen_ * pxPerCell);

    DrawWorld();
    DrawRobot(x_ * pxPerCell + pxPerCell / 2, y_ * pxPerCell + pxPerCell / 2);
    Show(kLongDuration);
  }

  void Show(int duration) {
    image_.ShowForMs(duration / speed_, "Karel's World");
  }

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
        if (x_ == x_dimen_ - 1) {
        } else {
          AnimateMove(x_ + 1, y_);
        }
        break;
      case Orientation::kSouth:
        if (y_ == y_dimen_ - 1) {
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
    Show(kLongDuration);
  }

  void PutBeeper() {
    if (!HasBeepersInBag()) {
      // TODO error
      return;
    }
    beeper_count_--;
    world_[x_][y_].SetNumBeepers(world_[x_][y_].GetNumBeepers() + 1);
    DrawWorld();
    DrawRobot();
    Show(kLongDuration);
  }

  void PickBeeper() {
    int count = world_[x_][y_].GetNumBeepers();
    if (count < 1) {
      // TODO error
      return;
    }
    world_[x_][y_].SetNumBeepers(count - 1);
    beeper_count_++;
    DrawWorld();
    DrawRobot();
    Show(kLongDuration);
  }

  bool HasBeepersInBag() { return beeper_count_ > 0; }

  void Finish() { image_.ShowUntilClosed("Karel's World"); }

 private:
  Robot() {}

  void DrawWorld() {
    image_.DrawRectangle(0, 0, x_dimen_ * pxPerCell, y_dimen_ * pxPerCell,
                         kWhite);
    for (int i = 0; i < x_dimen_; i++) {
      for (int j = 0; j < y_dimen_; j++) {
        int x_center = i * pxPerCell + pxPerCell / 2;
        int y_center = j * pxPerCell + pxPerCell / 2;
        image_.DrawLine(x_center - markSize / 2, y_center,
                        x_center + markSize / 2, y_center, markColor, 3);
        image_.DrawLine(x_center, y_center - markSize / 2, x_center,
                        y_center + markSize / 2, markColor, 3);
        // trig!
        double line_size = sqrt((beeperSize / 2) * (beeperSize / 2) / 2);
        if (world_[i][j].GetNumBeepers() > 0) {
          image_.DrawLine(x_center - line_size, y_center - line_size,
                          x_center + line_size, y_center + line_size,
                          beeperColor, beeperSize);
        }
        // TODO: Check for walls and draw them here in a thick darker grey.
      }
    }
    for (int i = 0; i < y_dimen_; i++) {
      // Draw horizontal lines.
      image_.DrawLine(0, i * pxPerCell, pxPerCell * x_dimen_ - 1, i * pxPerCell,
                      graphics::Color(200, 200, 200));
    }
    for (int i = 0; i < x_dimen_; i++) {
      // Draw vertical lines.
      image_.DrawLine(i * pxPerCell, 0, i * pxPerCell, pxPerCell * y_dimen_ - 1,
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
      Show(kShortDuration);
    }
    x_ = next_x;
    y_ = next_y;
    Show(kLongDuration);
  }

  graphics::Image image_;
  int x_dimen_ = kDefaultDimen;
  int y_dimen_ = kDefaultDimen;
  int x_ = 0;
  int y_ = y_dimen_ - 1;
  Orientation orientation_ = Orientation::kEast;
  int beeper_count_ = 0;
  std::vector<std::vector<Cell>> world_;
  bool initialized_ = false;
  double speed_ = 1.0;
};

}  // namespace karel

void LoadWorld(std::string filename) {
  karel::Robot& r = karel::Robot::InitializeInstance(filename);
}

void Move() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.Move();
}

void TurnLeft() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.TurnLeft();
}

void PutBeeper() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.PutBeeper();
}

void PickBeeper() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.PickBeeper();
}

void Finish() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.Finish();
}

bool FrontIsClear() {
  karel::Robot& r = karel::Robot::GetInstance();
  return false;
}

bool HasBeepersInBag() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.HasBeepersInBag();
}
