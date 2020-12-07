// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <math.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "../../graphics/image.h"
#include "cell.h"
#include "error.h"
#include "orientation.h"

#ifndef ROBOT_H
#define ROBOT_H

namespace karel {

class Robot {
 public:
  /**
   * Get the Robot singleton. Initializes it with default values if it isn't
   * already initialized. Set |enable_graphics| to false for testing, which
   * will disable animations. Use |force_initialize| for testing which resets
   * the singleton state.
   */
  static karel::Robot& GetInstance(bool enable_graphics = true, bool force_initialize = false);

  /**
   * Get the Robot singleton and intialize it from a file. Set |enable_graphics|
   * to false for testing, which will disable animations. Use |force_initialize| for testing which resets
   * the singleton state.
   */
  static karel::Robot& InitializeInstance(std::string filename,
                                          bool enable_graphics = true, bool force_initialize = false);

  // Disallow copy and assign.
  Robot(const Robot&) = delete;
  karel::Robot& operator=(const Robot&) = delete;

  /**
   * Move Karel forward one step. Results in an error if Karel cannot move
   * forward.
   */
  void Move();

  void TurnLeft();

  void PutBeeper();

  void PickBeeper();

  bool HasBeepersInBag() const;

  bool NoBeepersInBag() const;

  bool BeepersPresent() const;

  bool NoBeepersPresent() const;

  bool FrontIsClear() const;

  bool FrontIsBlocked() const;

  bool LeftIsClear() const;

  bool LeftIsBlocked() const;

  bool RightIsClear() const;

  bool RightIsBlocked() const;

  bool FacingNorth() const;

  bool NotFacingNorth() const;

  bool FacingEast() const;

  bool NotFacingEast() const;

  bool FacingSouth() const;

  bool NotFacingSouth() const;

  bool FacingWest() const;

  bool NotFacingWest() const;

  void Finish();

  //
  // Methods for tests. Tests should access Robot with GetInstance
  // and may inspect its state with the following methods.
  //

  /**
   * Gets the orientation that Karel is facing. May be simpler to use this in
   * tests than Facing* functions.
   */
  Orientation GetOrientation() const;

  /**
   * Gets the X position with respect to the grid coordinates. 1 is the
   * left-most cell.
   */
  int GetXPosition() const;

  /**
   * Gets the Y position with respect to the grid coordinates. 1 is the bottom
   * cell.
   */
  int GetYPosition() const;

  /**
   * Gets the total number of beepers in Karel's bag.
   */
  int GetNumBeepersInBag() const;

  /**
   * Gets the current grid. Takes in grid coordinates, where (1, 1) is the
   * bottom left cell.
   */
  const Cell& GetCell(int x, int y) const;

  /**
   * Gets the width of the current world.
   */
  int GetWorldWidth() const;

  /**
   * Gets the height of the current world.
   */
  int GetWorldHeight() const;

  /**
   * Returns the robot's current error state or kNoError if it has none.
   */
  RobotError GetError() const;

 private:
  // Private constructor: Robot can only be accessed with GetInstance.
  Robot();

  // Singleton.
  static karel::Robot& PrivateGetInstance();

  /**
   * Loads a Karel world from a file, or if |filename| is the empty string loads
   * a default world with Karel in a default position.
   */
  void Initialize(std::string filename, bool enable_graphics, bool force_initialize);

  void Show(int duration);

  void Error(RobotError error);

  bool DirectionIsClear(Orientation orientation) const;

  void DrawWorld();

  void DrawRobot();

  /**
   * Draws Karel at a location on the image. |pixel_x| and |pixel_y| are the
   * center of the cell in pixels.
   */
  void DrawRobot(int pixel_x, int pixel_y);

  void AnimateMove(int next_x, int next_y);

  // Note that the orientation is not populated. This is just a helper to
  // get the x and y position from a file with the next items in the stream
  // being (x, y)
  PositionAndOrientation ParsePosition(std::fstream& file) const;

  // Helper to get orientation of the form, `(x, y) direction`, for example,
  // (3, 7) East
  // Direction may be lower-case or have an uppercase first letter.
  PositionAndOrientation ParsePositionAndOrientation(std::fstream& file) const;

  // Whether animations are enabled. They should probably be disabled for
  // testing.
  bool enable_animations_ = true;

  // Speed multiplier for animation.
  double speed_ = 1.0;

  // Underlying image.
  graphics::Image image_;

  // Grid dimensions.
  int x_dimen_;
  int y_dimen_;

  // Karel's position and orientation.
  PositionAndOrientation position_;

  // Karel's beeper bag.
  int beeper_count_ = 0;

  // The cells in the world.
  std::vector<std::vector<Cell>> world_;

  // Whether the world has been initialized. It will not re-initialize.
  bool initialized_ = false;
  bool finished_ = false;
  RobotError error_ = RobotError::kNoError;
};

}  // namespace karel

#endif  // ROBOT_H
