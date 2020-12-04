#include "karel.h"

#include <math.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "../graphics/image.h"

// TODO: Finish shouldn't showuntilclosed in testing.
// Testing should have a non-graphical way to go.
// Save to file
// document
// phew

namespace karel {

enum Orientation {
  kNorth = 1,
  kEast = 2,
  kSouth = 3,
  kWest = 4,
};

struct PositionAndOrientation {
  int x = 0;
  int y = 0;
  Orientation orientation = Orientation::kNorth;
};

// Class representing a cell in Karel's world, including a count of beepers
// and whether or not there are walls.
class Cell {
 public:
  int GetNumBeepers() const { return num_beepers_; }
  void SetNumBeepers(int beepers) { num_beepers_ = beepers; }
  bool HasNorthWall() const { return north_wall_; }
  bool HasEastWall() const { return east_wall_; }
  bool HasSouthWall() const { return south_wall_; }
  bool HasWestWall() const { return west_wall_; }
  void AddWall(Orientation wall_orientation) {
    switch (wall_orientation) {
      case kNorth:
        north_wall_ = true;
        break;
      case kEast:
        east_wall_ = true;
        break;
      case kSouth:
        south_wall_ = true;
        break;
      case kWest:
        west_wall_ = true;
        break;
    }
  }

 private:
  bool num_beepers_ = 0;
  bool north_wall_ = false;
  bool east_wall_ = false;
  bool south_wall_ = false;
  bool west_wall_ = false;
};

// ms for a long animation duration to show Karel.
const int kLongDuration = 300;

// ms for a short animation duration to show Karel.
const int kShortDuration = 30;

const int kDefaultDimen = 10;
const int pxPerCell = 50;
const int markSize = 10;
const int robotSize = 30;
const int beeperSize = 30;
const int eyeSize = 4;
const int eyeOffset = 2;
const int legLength = 6;
const int limbWidth = 5;
const int kWallThickness = 3;
const int fontSize = 16;
const int margin = 32;
const graphics::Color eyeColor(50, 50, 50);
const graphics::Color karelColor(125, 125, 125);
const graphics::Color markColor(150, 150, 255);
const graphics::Color innerBeeperColor(172, 147, 194);
const graphics::Color limbColor(105, 105, 105);
const graphics::Color kWhite(255, 255, 255);
const graphics::Color kWallColor(50, 50, 50);
const graphics::Color kGridColor(220, 220, 220);
const graphics::Color kErrorColor(200, 0, 50);

class Robot {
 public:
  // Get the Robot singleton.
  static karel::Robot& GetInstance() {
    karel::Robot& instance = PrivateGetInstance();
    instance.Initialize("");
    return instance;
  }

  // Get the Robot singleton and intialize it from a file.
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
      position_.x = 0;
      position_.y = y_dimen_ - 1;
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
      // TODO: May need better error checking.
      while (world_file >> line_prefix) {
        if (line_prefix == wall_prefix) {
          PositionAndOrientation wall = ParsePositionAndOrientation(world_file);
          world_[wall.x][wall.y].AddWall(wall.orientation);
        } else if (line_prefix == beeper_prefix) {
          PositionAndOrientation beeper = ParsePosition(world_file);
          int count;
          if (!(world_file >> count)) {
            throw std::string("Error reading Beeper count");
          }
          world_[beeper.x][beeper.y].SetNumBeepers(count);
        } else if (line_prefix == bag_prefix) {
          std::string beepers;
          if (!(world_file >> beepers)) {
            throw std::string("Error reading quantity for BeeperBag");
          }
          if (beepers == "INFINITY" || beepers == "INFINITE") {
            beeper_count_ = std::numeric_limits<int>::max();
          } else {
            try {
              beeper_count_ = stoi(beepers);
            } catch (std::invalid_argument& e) {
              throw std::string("Unknown BeeperBag quanity, " + beepers);
            }
          }
        } else if (line_prefix == karel_prefix) {
          position_ = ParsePositionAndOrientation(world_file);
        } else if (line_prefix == speed_prefix) {
          if (!(world_file >> speed_)) {
            throw std::string("Error reading Speed");
          }
        } else {
          std::cout << "unexpected token in file: " << line_prefix << std::endl
                    << std::flush;
          break;
        }
      }
      world_file.close();
    }
    image_.Initialize(x_dimen_ * pxPerCell + margin,
                      y_dimen_ * pxPerCell + margin);

    DrawWorld();
    DrawRobot(position_.x * pxPerCell + pxPerCell / 2,
              position_.y * pxPerCell + pxPerCell / 2);
    Show(kLongDuration);
  }

  void Show(int duration) {
    if (finished_) return;
    image_.ShowForMs(duration / speed_, "Karel's World");
  }

  void Move() {
    if (finished_) return;
    switch (position_.orientation) {
      case Orientation::kNorth:
        if (position_.y == 0) {
          Error("Cannot move north");
        } else if (world_[position_.x][position_.y].HasNorthWall() ||
                   world_[position_.x][position_.y - 1].HasSouthWall()) {
          Error("Cannot move north");
        } else {
          AnimateMove(position_.x, position_.y - 1);
        }
        break;
      case Orientation::kEast:
        if (position_.x == x_dimen_ - 1) {
          Error("Cannot move east");
        } else if (world_[position_.x][position_.y].HasEastWall() ||
                   world_[position_.x + 1][position_.y].HasWestWall()) {
          Error("Cannot move east");
        } else {
          AnimateMove(position_.x + 1, position_.y);
        }
        break;
      case Orientation::kSouth:
        if (position_.y == y_dimen_ - 1) {
          Error("Cannot move south");
        } else if (world_[position_.x][position_.y].HasSouthWall() ||
                   world_[position_.x][position_.y + 1].HasNorthWall()) {
          Error("Cannot move south");
        } else {
          AnimateMove(position_.x, position_.y + 1);
        }
        break;
      case Orientation::kWest:
        if (position_.x == 0) {
          Error("Cannot move west");
        } else if (world_[position_.x][position_.y].HasWestWall() ||
                   world_[position_.x - 1][position_.y].HasEastWall()) {
          Error("Cannot move west");
        } else {
          AnimateMove(position_.x - 1, position_.y);
        }
        break;
    }
  }

  void TurnLeft() {
    if (finished_) return;
    switch (position_.orientation) {
      case Orientation::kNorth:
        position_.orientation = Orientation::kWest;
        break;
      case Orientation::kEast:
        position_.orientation = Orientation::kNorth;
        break;
      case Orientation::kSouth:
        position_.orientation = Orientation::kEast;
        break;
      case Orientation::kWest:
        position_.orientation = Orientation::kSouth;
        break;
    }
    DrawWorld();
    DrawRobot();
    Show(kLongDuration);
  }

  void PutBeeper() {
    if (finished_) return;
    if (!HasBeepersInBag()) {
      Error("No beepers left in bag");
      return;
    }
    beeper_count_--;
    world_[position_.x][position_.y].SetNumBeepers(
        world_[position_.x][position_.y].GetNumBeepers() + 1);
    DrawWorld();
    DrawRobot();
    Show(kLongDuration);
  }

  void PickBeeper() {
    if (finished_) return;
    if (!BeepersPresent()) {
      Error("No beeper to pick up");
      return;
    }
    world_[position_.x][position_.y].SetNumBeepers(
        world_[position_.x][position_.y].GetNumBeepers() - 1);
    beeper_count_++;
    DrawWorld();
    DrawRobot();
    Show(kLongDuration);
  }

  bool HasBeepersInBag() { return beeper_count_ > 0; }

  bool NoBeepersInBag() { return !HasBeepersInBag(); }

  bool BeepersPresent() {
    return world_[position_.x][position_.y].GetNumBeepers() > 0;
  }

  bool NoBeepersPresent() { return !BeepersPresent(); }

  bool FrontIsClear() { return DirectionIsClear(position_.orientation); }

  bool FrontIsBlocked() { return !DirectionIsClear(position_.orientation); }

  bool LeftIsClear() {
    return DirectionIsClear(
        (Orientation)(((int)position_.orientation + 1) % 4));
  }

  bool LeftIsBlocked() {
    return !DirectionIsClear(
        (Orientation)(((int)position_.orientation + 1) % 4));
  }

  bool RightIsClear() {
    return DirectionIsClear(
        (Orientation)(((int)position_.orientation - 1) % 4));
  }

  bool RightIsBlocked() {
    return !DirectionIsClear(
        (Orientation)(((int)position_.orientation - 1) % 4));
  }

  bool FacingNorth() { return position_.orientation == Orientation::kNorth; }

  bool NotFacingNorth() { return !FacingNorth(); }

  bool FacingEast() { return position_.orientation == Orientation::kEast; }

  bool NotFacingEast() { return !FacingEast(); }

  bool FacingSouth() { return position_.orientation == Orientation::kSouth; }

  bool NotFacingSouth() { return !FacingSouth(); }

  bool FacingWest() { return position_.orientation == Orientation::kWest; }

  bool NotFacingWest() { return !FacingWest(); }

  void Finish() {
    if (finished_) return;
    finished_ = true;
    image_.ShowUntilClosed("Karel's World");
  }

 private:
  Robot() {}

  // Singleton.
  static karel::Robot& PrivateGetInstance() {
    static Robot instance;
    return instance;
  }

  // TODO: Take an error code instead of a message so that it's easier to
  // test.
  void Error(std::string message) {
    std::cout << message << std::endl << std::flush;
    image_.DrawText(image_.GetWidth() / 2, image_.GetHeight() / 2, message,
                    fontSize, kErrorColor);
    Finish();
  }

  bool DirectionIsClear(Orientation orientation) {
    switch (orientation) {
      case Orientation::kNorth:
        if (position_.y == 0) {
          return false;
        } else if (world_[position_.x][position_.y].HasNorthWall() ||
                   world_[position_.x][position_.y - 1].HasSouthWall()) {
          return false;
        } else {
          return true;
        }
        break;
      case Orientation::kEast:
        if (position_.x == x_dimen_ - 1) {
          return false;
        } else if (world_[position_.x][position_.y].HasEastWall() ||
                   world_[position_.x + 1][position_.y].HasWestWall()) {
          return false;
        } else {
          return true;
        }
        break;
      case Orientation::kSouth:
        if (position_.y == y_dimen_ - 1) {
          return false;
        } else if (world_[position_.x][position_.y].HasSouthWall() ||
                   world_[position_.x][position_.y + 1].HasNorthWall()) {
          return false;
        } else {
          return true;
        }
        break;
      case Orientation::kWest:
        if (position_.x == 0) {
          return false;
        } else if (world_[position_.x][position_.y].HasWestWall() ||
                   world_[position_.x - 1][position_.y].HasEastWall()) {
          return false;
        } else {
          return true;
        }
        break;
    }
  }

  void DrawWorld() {
    image_.DrawRectangle(0, 0, x_dimen_ * pxPerCell, y_dimen_ * pxPerCell,
                         kWhite);
    for (int i = 0; i <= y_dimen_; i++) {
      // Draw horizontal lines and indexes.
      int x = pxPerCell * x_dimen_ - 1;
      int y = i * pxPerCell;
      image_.DrawLine(0, y, x, y, kGridColor, kWallThickness);
      if (i < y_dimen_) {
        image_.DrawText(x + fontSize / 2, y + (pxPerCell - fontSize) / 2,
                        std::to_string(i + 1), fontSize, kWallColor);
      }
    }
    for (int i = 0; i <= x_dimen_; i++) {
      // Draw vertical lines and indexes.
      int x = i * pxPerCell;
      int y = pxPerCell * y_dimen_ - 1;
      image_.DrawLine(x, 0, x, y, kGridColor, kWallThickness);
      if (i < x_dimen_) {
        image_.DrawText(x + (pxPerCell - fontSize) / 2, y + fontSize / 2,
                        std::to_string(i + 1), fontSize, kWallColor);
      }
    }
    for (int i = 0; i < x_dimen_; i++) {
      for (int j = 0; j < y_dimen_; j++) {
        int x_center = i * pxPerCell + pxPerCell / 2;
        int y_center = j * pxPerCell + pxPerCell / 2;
        // Draw the little plus at the center of the cell.
        image_.DrawLine(x_center - markSize / 2, y_center,
                        x_center + markSize / 2, y_center, markColor,
                        kWallThickness);
        image_.DrawLine(x_center, y_center - markSize / 2, x_center,
                        y_center + markSize / 2, markColor, kWallThickness);
        Cell& cell = world_[i][j];
        if (cell.GetNumBeepers() > 0) {
          // Draw the beepers. Beepers are stacked so you can't tell if there's
          // more than one in a stack.
          // trig to get diamonds from thick lines!
          double line_size = sqrt((beeperSize / 2) * (beeperSize / 2) / 2);
          int inner_beeper_size = beeperSize - kWallThickness * 2;
          double inner_line_size =
              sqrt((inner_beeper_size / 2) * (inner_beeper_size / 2) / 2);
          image_.DrawLine(x_center - line_size, y_center - line_size,
                          x_center + line_size, y_center + line_size,
                          kWallColor, beeperSize);
          image_.DrawLine(
              x_center - inner_line_size, y_center - inner_line_size,
              x_center + inner_line_size, y_center + inner_line_size,
              innerBeeperColor, inner_beeper_size);
        }
        // Draw the walls.
        if (cell.HasNorthWall()) {
          image_.DrawLine(i * pxPerCell, j * pxPerCell, (i + 1) * pxPerCell - 1,
                          j * pxPerCell, kWallColor, kWallThickness);
        }
        if (cell.HasSouthWall()) {
          image_.DrawLine(i * pxPerCell, (j + 1) * pxPerCell - 1,
                          (i + 1) * pxPerCell - 1, (j + 1) * pxPerCell - 1,
                          kWallColor, kWallThickness);
        }
        if (cell.HasWestWall()) {
          image_.DrawLine(i * pxPerCell, j * pxPerCell, i * pxPerCell,
                          (j + 1) * pxPerCell - 1, kWallColor, kWallThickness);
        }
        if (cell.HasEastWall()) {
          image_.DrawLine((i + 1) * pxPerCell - 1, j * pxPerCell,
                          (i + 1) * pxPerCell - 1, (j + 1) * pxPerCell - 1,
                          kWallColor, kWallThickness);
        }
      }
    }
  }

  void DrawRobot() {
    // Center in the cell.
    DrawRobot(position_.x * pxPerCell + pxPerCell / 2,
              position_.y * pxPerCell + pxPerCell / 2);
  }

  // pixel_x and pixel_y are the center of the cell in pixels.
  void DrawRobot(int pixel_x, int pixel_y) {
    image_.DrawRectangle(pixel_x - robotSize / 2, pixel_y - robotSize / 2,
                         robotSize, robotSize, karelColor);
    switch (position_.orientation) {
      case Orientation::kNorth:
        image_.DrawCircle(pixel_x,
                          pixel_y - robotSize / 2 + eyeSize / 2 + eyeOffset,
                          eyeSize, kWhite);
        image_.DrawCircle(pixel_x, pixel_y + eyeOffset, eyeSize, kWhite);
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
        image_.DrawCircle(pixel_x - eyeOffset, pixel_y, eyeSize, kWhite);
        image_.DrawCircle(pixel_x + robotSize / 2 - eyeSize / 2 - eyeOffset,
                          pixel_y, eyeSize, kWhite);
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
        image_.DrawCircle(pixel_x,
                          pixel_y + robotSize / 2 - eyeSize / 2 - eyeOffset,
                          eyeSize, kWhite);
        image_.DrawCircle(pixel_x, pixel_y - eyeOffset, eyeSize, kWhite);
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
        image_.DrawCircle(pixel_x + eyeOffset, pixel_y, eyeSize, kWhite);
        image_.DrawCircle(pixel_x - robotSize / 2 + eyeSize / 2 + eyeOffset,
                          pixel_y, eyeSize, kWhite);
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
      double x = position_.x * (1 - fraction) + next_x * fraction;
      double y = position_.y * (1 - fraction) + next_y * fraction;
      DrawRobot(x * pxPerCell + pxPerCell / 2, y * pxPerCell + pxPerCell / 2);
      Show(kShortDuration);
    }
    position_.x = next_x;
    position_.y = next_y;
    Show(kLongDuration);
  }

  // Note that the orientation is not populated. This is just a helper to
  // get the x and y position from a file with the next items in the stream
  // being (x, y)
  PositionAndOrientation ParsePosition(std::fstream& file) {
    char open_paren, comma, closed_paren;
    int x, y;
    if (!(file >> open_paren >> x >> comma >> y >> closed_paren)) {
      throw std::string("Error reading position");
    }
    PositionAndOrientation result;
    // Convert y in the file to y on-screen. In the file, (1, 1) is the
    // bottom left corner.
    // In Karel coordinates, that's (0, y_dimen - 1).
    result.y = y_dimen_ - y;
    result.x = x - 1;
    return result;
  }

  // Helper to get orientation of the form, `(x, y) direction`, for example,
  // (3, 7) East
  // Direction may be lower-case or have an uppercase first letter.
  PositionAndOrientation ParsePositionAndOrientation(std::fstream& file) {
    PositionAndOrientation result = ParsePosition(file);
    std::string direction;
    if (!(file >> direction)) {
      throw std::string("Error reading orientation");
    }
    // Ensure the first character is lower cased.
    direction[0] = tolower(direction[0]);
    if (direction == "north") {
      result.orientation = Orientation::kNorth;
    } else if (direction == "east") {
      result.orientation = Orientation::kEast;
    } else if (direction == "south") {
      result.orientation = Orientation::kSouth;
    } else if (direction == "west") {
      result.orientation = Orientation::kWest;
    } else {
      throw std::string("Unknown orientation " + direction);
    }
    return result;
  }

  // Underlying image.
  graphics::Image image_;

  // Grid dimensions.
  int x_dimen_ = kDefaultDimen;
  int y_dimen_ = kDefaultDimen;

  // Karel's position and orientation.
  PositionAndOrientation position_ = {0, y_dimen_ - 1, Orientation::kEast};

  // Karel's beeper bag.
  int beeper_count_ = 0;

  // The cells in the world.
  std::vector<std::vector<Cell>> world_;

  // Whether the world has been initialized. It will not re-initialize.
  bool initialized_ = false;
  bool finished_ = false;

  // Speed multiplier for animation.
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
  return r.FrontIsClear();
}

bool FrontIsBlocked() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FrontIsBlocked();
}

bool LeftIsClear() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.LeftIsClear();
}

bool LeftIsBlocked() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.LeftIsBlocked();
}

bool RightIsClear() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.RightIsClear();
}

bool RightIsBlocked() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.RightIsBlocked();
}

bool HasBeepersInBag() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.HasBeepersInBag();
}

bool NoBeepersInBag() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.NoBeepersInBag();
}

bool BeepersPresent() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.BeepersPresent();
}

bool NoBeepersPresent() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.NoBeepersPresent();
}

bool FacingNorth() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FacingNorth();
}

bool NotFacingNorth() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.NotFacingNorth();
}

bool FacingEast() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FacingEast();
}

bool NotFacingEast() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.NotFacingEast();
}

bool FacingSouth() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FacingSouth();
}

bool NotFacingSouth() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.NotFacingSouth();
}

bool FacingWest() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FacingWest();
}

bool NotFacingWest() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.NotFacingWest();
}
