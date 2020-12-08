// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "robot.h"

#include <math.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "../../graphics/image.h"
#include "cell.h"
#include "error.h"
#include "orientation.h"

// TODO: Save to image file (maybe for testing)
// document

namespace karel {

namespace {

// Default width and height in cells.
const int kDefaultDimen = 10;

// Milliseconds for a long animation duration to show Karel, used after each
// Karel action.
const int kLongDuration = 300;

// Milliseconds for a short animation duration to show Karel, used when Karel
// is moving.
const int kShortDuration = 30;

// Number of steps to take in the animation moving karel between cells.
const int kNumAnimationSteps = 10;

// Pixel constants.
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
const int kErrorFontSize = 20;
const int margin = 32;

// Color constants.
const graphics::Color eyeColor(50, 50, 50);
const graphics::Color karelColor(125, 125, 125);
const graphics::Color markColor(150, 150, 255);
const graphics::Color innerBeeperColor(172, 147, 194);
const graphics::Color limbColor(105, 105, 105);
const graphics::Color kWhite(255, 255, 255);
const graphics::Color kWallColor(50, 50, 50);
const graphics::Color kGridColor(220, 220, 220);
const graphics::Color kErrorColor(173, 0, 35);

}  // namespace

Robot::Robot() {}

// Singleton. Static.
karel::Robot& Robot::PrivateGetInstance() {
  static Robot instance;
  return instance;
}

// static.
karel::Robot& Robot::GetInstance(bool enable_graphics, bool force_initialize) {
  karel::Robot& instance = PrivateGetInstance();
  instance.Initialize("", enable_graphics, force_initialize);
  return instance;
}

// Get the Robot singleton and intialize it from a file.
// static.
karel::Robot& Robot::InitializeInstance(std::string filename,
                                        bool enable_graphics,
                                        bool force_initialize) {
  karel::Robot& instance = PrivateGetInstance();
  instance.Initialize(filename, enable_graphics, force_initialize);
  return instance;
}

void Robot::Initialize(std::string filename, bool enable_graphics,
                       bool force_initialize) {
  // Ensure only intitialized once unless |force_initialize| is true.
  if (initialized_ && !force_initialize) return;
  initialized_ = true;
  error_ = RobotError::kNoError;
  finished_ = false;

  enable_animations_ = enable_graphics;

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
    position_ = {0, kDefaultDimen - 1, Orientation::kEast};
  } else {
    std::fstream world_file;
    try {
      world_file.open(filename);
    } catch (std::ifstream::failure e) {
      throw std::string("Error opening file");
    }
    if (!world_file.is_open()) {
      // TODO: Replace throwables with better error codes?
      throw std::string("Error opening file");
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
    if (x_dimen_ < 1 || y_dimen_ < 1) {
      throw std::string(
          "Cannot load a world less than 1 cell wide or less than 1 cell "
          "tall");
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
        if (speed_ < 0) {
          throw std::string("Speed must be greater than 0");
        }
      } else {
        std::cout << "unexpected token in file: " << line_prefix << std::endl
                  << std::flush;
        break;
      }
    }
    world_file.close();
  }
  int min_width = 5 * pxPerCell + margin;
  image_.Initialize(std::max(x_dimen_ * pxPerCell + margin, min_width),
                    y_dimen_ * pxPerCell + margin);

  DrawWorld();
  DrawRobot(position_.x * pxPerCell + pxPerCell / 2,
            position_.y * pxPerCell + pxPerCell / 2);
  Show(kLongDuration);
}

void Robot::Move() {
  if (finished_) return;
  switch (position_.orientation) {
    case Orientation::kNorth:
      if (position_.y == 0) {
        Error(RobotError::kCannotMoveNorth);
      } else if (world_[position_.x][position_.y].HasNorthWall() ||
                 world_[position_.x][position_.y - 1].HasSouthWall()) {
        Error(RobotError::kCannotMoveNorth);
      } else {
        AnimateMove(position_.x, position_.y - 1);
      }
      break;
    case Orientation::kEast:
      if (position_.x == x_dimen_ - 1) {
        Error(RobotError::kCannotMoveEast);
      } else if (world_[position_.x][position_.y].HasEastWall() ||
                 world_[position_.x + 1][position_.y].HasWestWall()) {
        Error(RobotError::kCannotMoveEast);
      } else {
        AnimateMove(position_.x + 1, position_.y);
      }
      break;
    case Orientation::kSouth:
      if (position_.y == y_dimen_ - 1) {
        Error(RobotError::kCannotMoveSouth);
      } else if (world_[position_.x][position_.y].HasSouthWall() ||
                 world_[position_.x][position_.y + 1].HasNorthWall()) {
        Error(RobotError::kCannotMoveSouth);
      } else {
        AnimateMove(position_.x, position_.y + 1);
      }
      break;
    case Orientation::kWest:
      if (position_.x == 0) {
        Error(RobotError::kCannotMoveWest);
      } else if (world_[position_.x][position_.y].HasWestWall() ||
                 world_[position_.x - 1][position_.y].HasEastWall()) {
        Error(RobotError::kCannotMoveWest);
      } else {
        AnimateMove(position_.x - 1, position_.y);
      }
      break;
  }
}

void Robot::TurnLeft() {
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

void Robot::PutBeeper() {
  if (finished_) return;
  if (!HasBeepersInBag()) {
    Error(RobotError::kCannotPutBeeper);
    return;
  }
  beeper_count_--;
  Cell& cell = world_[position_.x][position_.y];
  cell.SetNumBeepers(cell.GetNumBeepers() + 1);
  DrawWorld();
  DrawRobot();
  Show(kLongDuration);
}

void Robot::PickBeeper() {
  if (finished_) return;
  if (!BeepersPresent()) {
    Error(RobotError::kCannotPickBeeper);
    return;
  }
  Cell& cell = world_[position_.x][position_.y];
  cell.SetNumBeepers(cell.GetNumBeepers() - 1);
  beeper_count_++;
  DrawWorld();
  DrawRobot();
  Show(kLongDuration);
}

bool Robot::HasBeepersInBag() const { return beeper_count_ > 0; }

bool Robot::BeepersPresent() const {
  return world_[position_.x][position_.y].GetNumBeepers() > 0;
}

bool Robot::FrontIsClear() const {
  return DirectionIsClear(position_.orientation);
}

bool Robot::LeftIsClear() const {
  return DirectionIsClear((Orientation)(((int)position_.orientation + 1) % 4));
}

bool Robot::RightIsClear() const {
  return DirectionIsClear((Orientation)(((int)position_.orientation - 1) % 4));
}

bool Robot::FacingNorth() const {
  return position_.orientation == Orientation::kNorth;
}

bool Robot::FacingEast() const {
  return position_.orientation == Orientation::kEast;
}

bool Robot::FacingSouth() const {
  return position_.orientation == Orientation::kSouth;
}

bool Robot::FacingWest() const {
  return position_.orientation == Orientation::kWest;
}

void Robot::Finish() {
  if (finished_) return;
  finished_ = true;
  if (enable_animations_) {
    image_.ShowUntilClosed("Karel's World");
  }
}

Orientation Robot::GetOrientation() const { return position_.orientation; }

int Robot::GetXPosition() const { return position_.x + 1; }

int Robot::GetYPosition() const { return y_dimen_ - position_.y; }

int Robot::GetNumBeepersInBag() const { return beeper_count_; }

const Cell& Robot::GetCell(int x, int y) const {
  return world_[x - 1][y_dimen_ - y];
}

int Robot::GetWorldWidth() const { return x_dimen_; }

int Robot::GetWorldHeight() const { return y_dimen_; }

RobotError Robot::GetError() const { return error_; }

void Robot::Show(int duration) {
  if (finished_) return;
  if (enable_animations_) {
    image_.ShowForMs(duration * speed_, "Karel's World");
  }
}

void Robot::Error(RobotError error) {
  error_ = error;
  std::string message = "Error: ";
  switch (error) {
    case kNoError:
      // This shouldn't happen.
      return;
    case kCannotMoveNorth:
      message += " Cannot move north";
      break;
    case kCannotMoveEast:
      message += "  Cannot move east";
      break;
    case kCannotMoveSouth:
      message += " Cannot move south";
      break;
    case kCannotMoveWest:
      message += "  Cannot move west";
      break;
    case kCannotPickBeeper:
      message += "Cannot pick beeper\n(No beepers present)";
      break;
    case kCannotPutBeeper:
      message += " Cannot put beeper\n(No beepers in bag)";
      break;
  }
  std::cout << message << std::endl << std::flush;
  int approx_width = 25 * kErrorFontSize / 4;
  int text_x = std::max(2, image_.GetWidth() / 2 - approx_width);
  int text_y = image_.GetHeight() / 2 - kErrorFontSize / 2;
  image_.DrawText(text_x - 2, text_y - 2, message, kErrorFontSize, kWhite);
  image_.DrawText(text_x + 2, text_y - 2, message, kErrorFontSize, kWhite);
  image_.DrawText(text_x - 2, text_y + 2, message, kErrorFontSize, kWhite);
  image_.DrawText(text_x + 2, text_y + 2, message, kErrorFontSize, kWhite);
  image_.DrawText(text_x, text_y, message, kErrorFontSize, kErrorColor);
  Finish();
}

bool Robot::DirectionIsClear(Orientation orientation) const {
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

void Robot::DrawWorld() {
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
                        x_center + line_size, y_center + line_size, kWallColor,
                        beeperSize);
        image_.DrawLine(x_center - inner_line_size, y_center - inner_line_size,
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

void Robot::DrawRobot() {
  // Center in the cell.
  DrawRobot(position_.x * pxPerCell + pxPerCell / 2,
            position_.y * pxPerCell + pxPerCell / 2);
}

// pixel_x and pixel_y are the center of the cell in pixels.
void Robot::DrawRobot(int pixel_x, int pixel_y) {
  image_.DrawRectangle(pixel_x - robotSize / 2, pixel_y - robotSize / 2,
                       robotSize, robotSize, karelColor);
  switch (position_.orientation) {
    case Orientation::kNorth:
      image_.DrawCircle(pixel_x,
                        pixel_y - robotSize / 2 + eyeSize / 2 + eyeOffset,
                        eyeSize, kWhite);
      image_.DrawCircle(pixel_x, pixel_y + eyeOffset, eyeSize, kWhite);
      image_.DrawCircle(pixel_x, pixel_y - robotSize / 2 + eyeSize / 2, eyeSize,
                        eyeColor);
      image_.DrawLine(pixel_x + robotSize / 2, pixel_y - legLength,
                      pixel_x + robotSize / 2 + legLength, pixel_y - legLength,
                      limbColor, limbWidth);
      image_.DrawLine(pixel_x + robotSize / 2, pixel_y + legLength,
                      pixel_x + robotSize / 2 + legLength, pixel_y + legLength,
                      limbColor, limbWidth);
      break;
    case Orientation::kEast:
      image_.DrawCircle(pixel_x - eyeOffset, pixel_y, eyeSize, kWhite);
      image_.DrawCircle(pixel_x + robotSize / 2 - eyeSize / 2 - eyeOffset,
                        pixel_y, eyeSize, kWhite);
      image_.DrawCircle(pixel_x + robotSize / 2 - eyeSize / 2, pixel_y, eyeSize,
                        eyeColor);
      image_.DrawLine(pixel_x - legLength, pixel_y + robotSize / 2,
                      pixel_x - legLength, pixel_y + robotSize / 2 + legLength,
                      limbColor, limbWidth);
      image_.DrawLine(pixel_x + legLength, pixel_y + robotSize / 2,
                      pixel_x + legLength, pixel_y + robotSize / 2 + legLength,
                      limbColor, limbWidth);
      break;
    case Orientation::kSouth:
      image_.DrawCircle(pixel_x,
                        pixel_y + robotSize / 2 - eyeSize / 2 - eyeOffset,
                        eyeSize, kWhite);
      image_.DrawCircle(pixel_x, pixel_y - eyeOffset, eyeSize, kWhite);
      image_.DrawCircle(pixel_x, pixel_y + robotSize / 2 - eyeSize / 2, eyeSize,
                        eyeColor);
      image_.DrawLine(pixel_x - robotSize / 2, pixel_y - legLength,
                      pixel_x - robotSize / 2 - legLength, pixel_y - legLength,
                      limbColor, limbWidth);
      image_.DrawLine(pixel_x - robotSize / 2, pixel_y + legLength,
                      pixel_x - robotSize / 2 - legLength, pixel_y + legLength,
                      limbColor, limbWidth);
      break;
    case Orientation::kWest:
      image_.DrawCircle(pixel_x + eyeOffset, pixel_y, eyeSize, kWhite);
      image_.DrawCircle(pixel_x - robotSize / 2 + eyeSize / 2 + eyeOffset,
                        pixel_y, eyeSize, kWhite);
      image_.DrawCircle(pixel_x - robotSize / 2 + eyeSize / 2, pixel_y, eyeSize,
                        eyeColor);
      image_.DrawLine(pixel_x - legLength, pixel_y - robotSize / 2,
                      pixel_x - legLength, pixel_y - robotSize / 2 - legLength,
                      limbColor, limbWidth);
      image_.DrawLine(pixel_x + legLength, pixel_y - robotSize / 2,
                      pixel_x + legLength, pixel_y - robotSize / 2 - legLength,
                      limbColor, limbWidth);
      break;
  }
  image_.DrawCircle(pixel_x, pixel_y, eyeSize, eyeColor);
}

void Robot::AnimateMove(int next_x, int next_y) {
  int steps = enable_animations_ ? kNumAnimationSteps : 0;
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
PositionAndOrientation Robot::ParsePosition(std::fstream& file) const {
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
PositionAndOrientation Robot::ParsePositionAndOrientation(
    std::fstream& file) const {
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

}  // namespace karel
