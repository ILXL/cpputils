// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "../../karel.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../../graphics/image.h"
#include "../cell.h"
#include "../error.h"
#include "../orientation.h"
#include "../robot.h"

using karel::Cell;
using karel::Orientation;
using karel::Robot;
using karel::RobotError;

// Test TODOs:
// Test loading 1 and many beepers.
// Test loading world where karel has finite beepers.
// Test error states
// Test loading bad files and helpful output
// Test stops taking action after reaching an error state.
// Test loading beepers
// Test putting/picking beepers not at (1, 1)

bool IsBasicallyInfinite(int number) {
  return number > std::numeric_limits<int>::max() / 2;
}

void CellIsEmptyWithNoWalls(const Cell& cell) {
  ASSERT_EQ(0, cell.GetNumBeepers());
  ASSERT_FALSE(cell.HasNorthWall());
  ASSERT_FALSE(cell.HasEastWall());
  ASSERT_FALSE(cell.HasSouthWall());
  ASSERT_FALSE(cell.HasWestWall());
}

class KarelTest : public testing::Test {
 public:
  void SetUp() override {
    // TODO.
  }
};

TEST_F(KarelTest, GetsKarelInstance) {
  Robot& r = Robot::GetInstance(/* enable animations */ false,
                                /* force initialize */ true);
}

TEST_F(KarelTest, DefaultWorld) {
  Robot& r = Robot::GetInstance(/* enable animations */ false,
                                /* force initialize */ true);

  // Default world is 10x10.
  ASSERT_EQ(10, r.GetWorldWidth());
  ASSERT_EQ(10, r.GetWorldHeight());

  // Default world has no beepers and no walls.
  for (int i = 1; i <= 10; i++) {
    for (int j = 1; j <= 10; j++) {
      CellIsEmptyWithNoWalls(r.GetCell(i, j));
    }
  }

  // Default Karel is in bottom left with infinite beepers.
  ASSERT_EQ(1, r.GetXPosition());
  ASSERT_EQ(1, r.GetYPosition());
  ASSERT_EQ(Orientation::kEast, r.GetOrientation());
  ASSERT_TRUE(IsBasicallyInfinite(r.GetNumBeepersInBag()));
}

TEST_F(KarelTest, LoadsWorld) {
  Robot& r =
      Robot::InitializeInstance("worlds/2x1.w", /* enable animations */ false,
                                /* force initialize */ true);
  ASSERT_EQ(2, r.GetWorldWidth());
  ASSERT_EQ(1, r.GetWorldHeight());
  CellIsEmptyWithNoWalls(r.GetCell(1, 1));
  CellIsEmptyWithNoWalls(r.GetCell(2, 1));

  ASSERT_EQ(1, r.GetXPosition());
  ASSERT_EQ(1, r.GetYPosition());
  ASSERT_EQ(Orientation::kEast, r.GetOrientation());
  ASSERT_TRUE(IsBasicallyInfinite(r.GetNumBeepersInBag()));
}

TEST_F(KarelTest, LoadsWorldWithOuterWalls) {
  Robot& r =
      Robot::InitializeInstance("worlds/outer_walls.w", /* enable animations */ false,
                                /* force initialize */ true);
  ASSERT_EQ(8, r.GetWorldWidth());
  ASSERT_EQ(8, r.GetWorldHeight());

  // Cell at (3, 5) has only north wall.
  EXPECT_TRUE(r.GetCell(3, 5).HasNorthWall());
  // Cell at (2, 6) has only east wall.
  EXPECT_TRUE(r.GetCell(2, 6).HasEastWall());
  // Cell at (3, 7) has only south wall
  EXPECT_TRUE(r.GetCell(3, 7).HasSouthWall());
  // Cell at (4, 6) has only west wall.
  EXPECT_TRUE(r.GetCell(4, 6).HasWestWall());

  ASSERT_EQ(Orientation::kEast, r.GetOrientation());
  ASSERT_EQ(3, r.GetXPosition());
  ASSERT_EQ(6, r.GetYPosition());
}

TEST_F(KarelTest, LoadsWorldWithInnerWalls) {
  Robot& r =
      Robot::InitializeInstance("worlds/inner_walls.w", /* enable animations */ false,
                                /* force initialize */ true);
  // Cell at (3, 2) has all four walls.
  const Cell& c = r.GetCell(3, 2);
  EXPECT_TRUE(c.HasNorthWall());
  EXPECT_TRUE(c.HasEastWall());
  EXPECT_TRUE(c.HasSouthWall());
  EXPECT_TRUE(c.HasWestWall());

  ASSERT_EQ(Orientation::kEast, r.GetOrientation());
  ASSERT_EQ(3, r.GetXPosition());
  ASSERT_EQ(2, r.GetYPosition());
}

TEST_F(KarelTest, CannotMoveThroughOuterWalls) {
  for (int i = 0; i < 4; i++) {
    Robot& r =
        Robot::InitializeInstance("worlds/outer_walls.w",
                                  /* enable animations */ false,
                                  /* force initialize */ true);
    ASSERT_EQ(RobotError::kNoError, r.GetError());
    for (int j = 0; j < i; j++) {
      TurnLeft();
    }
    EXPECT_FALSE(FrontIsClear());
    EXPECT_TRUE(FrontIsBlocked());
    EXPECT_FALSE(RightIsClear());
    EXPECT_TRUE(RightIsBlocked());
    EXPECT_FALSE(LeftIsClear());
    EXPECT_TRUE(LeftIsBlocked());
    Move();
    if (i == 0) {
      ASSERT_TRUE(FacingEast());
      EXPECT_EQ(RobotError::kCannotMoveEast, r.GetError());
    } else if (i == 1) {
      ASSERT_TRUE(FacingNorth());
      EXPECT_EQ(RobotError::kCannotMoveNorth, r.GetError());
    } else if (i == 2) {
      ASSERT_TRUE(FacingWest());
      EXPECT_EQ(RobotError::kCannotMoveWest, r.GetError());
    } else {
      ASSERT_TRUE(FacingSouth());
      EXPECT_EQ(RobotError::kCannotMoveSouth, r.GetError());
    }
  }
}

TEST_F(KarelTest, CannotMoveThroughInnerWalls) {
  for (int i = 0; i < 4; i++) {
    Robot& r =
        Robot::InitializeInstance("worlds/inner_walls.w",
                                  /* enable animations */ false,
                                  /* force initialize */ true);
    ASSERT_EQ(RobotError::kNoError, r.GetError());
    for (int j = 0; j < i; j++) {
      TurnLeft();
    }
    Move();
    EXPECT_FALSE(FrontIsClear());
    EXPECT_TRUE(FrontIsBlocked());
    EXPECT_FALSE(RightIsClear());
    EXPECT_TRUE(RightIsBlocked());
    EXPECT_FALSE(LeftIsClear());
    EXPECT_TRUE(LeftIsBlocked());
    if (i == 0) {
      ASSERT_TRUE(FacingEast());
      EXPECT_EQ(RobotError::kCannotMoveEast, r.GetError());
    } else if (i == 1) {
      ASSERT_TRUE(FacingNorth());
      EXPECT_EQ(RobotError::kCannotMoveNorth, r.GetError());
    } else if (i == 2) {
      ASSERT_TRUE(FacingWest());
      EXPECT_EQ(RobotError::kCannotMoveWest, r.GetError());
    } else {
      ASSERT_TRUE(FacingSouth());
      EXPECT_EQ(RobotError::kCannotMoveSouth, r.GetError());
    }
  }
}

TEST_F(KarelTest, CannotMoveThroughWorldEdges) {
  for (int i = 0; i < 4; i++) {
    Robot& r =
        Robot::InitializeInstance("worlds/2x1.w",
                                  /* enable animations */ false,
                                  /* force initialize */ true);
    ASSERT_EQ(RobotError::kNoError, r.GetError());
    for (int j = 0; j < i; j++) {
      TurnLeft();
    }
    if (FacingEast()) {
      Move();
      Move();
      EXPECT_EQ(RobotError::kCannotMoveEast, r.GetError());
      EXPECT_EQ(2, r.GetXPosition());
      EXPECT_EQ(1, r.GetYPosition());
    } else if (FacingNorth()) {
      Move();
      EXPECT_EQ(RobotError::kCannotMoveNorth, r.GetError());
    } else if (FacingWest()) {
      Move();
      EXPECT_EQ(RobotError::kCannotMoveWest, r.GetError());
    } else if (FacingSouth()) {
      Move();
      EXPECT_EQ(RobotError::kCannotMoveSouth, r.GetError());
    }
  }

}

TEST_F(KarelTest, PutsAndPicksBeeper) {
  // Default world has no beepers, and Karel has infinite.
  Robot& r = Robot::GetInstance(/* enable animations */ false,
                                /* force initialize */ true);
  ASSERT_FALSE(BeepersPresent());
  ASSERT_EQ(0, r.GetCell(1, 1).GetNumBeepers());
  int count = 10;
  for (int i = 1; i <= count; i++) {
    PutBeeper();
    ASSERT_TRUE(BeepersPresent());
    EXPECT_EQ(i, r.GetCell(1, 1).GetNumBeepers());
  }
  for (int i = count - 1; i >= 0; i--) {
    PickBeeper();
    if (i > 0) {
      ASSERT_TRUE(BeepersPresent());
    } else {
      ASSERT_FALSE(BeepersPresent());
    }
    EXPECT_EQ(i, r.GetCell(1, 1).GetNumBeepers());
  }
}

TEST_F(KarelTest, TurnsLeft) {
  Robot& r = Robot::GetInstance(/* enable animations */ false,
                                /* force initialize */ true);
  int count = 10;
  // Starts facing East.
  for (int i = 0; i < count; i++) {
    if (i % 4 == 0) {
      // Facing east.
      ASSERT_EQ(Orientation::kEast, r.GetOrientation());
      EXPECT_FALSE(FacingNorth());
      EXPECT_TRUE(NotFacingNorth());
      EXPECT_TRUE(FacingEast());
      EXPECT_FALSE(NotFacingEast());
      EXPECT_FALSE(FacingSouth());
      EXPECT_TRUE(NotFacingSouth());
      EXPECT_FALSE(FacingWest());
      EXPECT_TRUE(NotFacingWest());
    } else if (i % 4 == 1) {
      // Facing north.
      ASSERT_EQ(Orientation::kNorth, r.GetOrientation());
      EXPECT_TRUE(FacingNorth());
      EXPECT_FALSE(NotFacingNorth());
      EXPECT_FALSE(FacingEast());
      EXPECT_TRUE(NotFacingEast());
      EXPECT_FALSE(FacingSouth());
      EXPECT_TRUE(NotFacingSouth());
      EXPECT_FALSE(FacingWest());
      EXPECT_TRUE(NotFacingWest());
    } else if (i % 4 == 2) {
      // Facing west.
      ASSERT_EQ(Orientation::kWest, r.GetOrientation());
      EXPECT_FALSE(FacingNorth());
      EXPECT_TRUE(NotFacingNorth());
      EXPECT_FALSE(FacingEast());
      EXPECT_TRUE(NotFacingEast());
      EXPECT_FALSE(FacingSouth());
      EXPECT_TRUE(NotFacingSouth());
      EXPECT_TRUE(FacingWest());
      EXPECT_FALSE(NotFacingWest());
    } else {
      // Facing south.
      ASSERT_EQ(Orientation::kSouth, r.GetOrientation());
      EXPECT_FALSE(FacingNorth());
      EXPECT_TRUE(NotFacingNorth());
      EXPECT_FALSE(FacingEast());
      EXPECT_TRUE(NotFacingEast());
      EXPECT_TRUE(FacingSouth());
      EXPECT_FALSE(NotFacingSouth());
      EXPECT_FALSE(FacingWest());
      EXPECT_TRUE(NotFacingWest());
    }
    TurnLeft();
  }
}

TEST_F(KarelTest, MovesEastAndWest) {
  Robot& r =
      Robot::InitializeInstance("worlds/8x1.w", /* enable animations */ false,
                                /* force initialize */ true);
  ASSERT_EQ(8, r.GetWorldWidth());
  ASSERT_EQ(1, r.GetWorldHeight());
  for (int i = 2; i <= 8; i++) {
    Move();
    ASSERT_EQ(i, r.GetXPosition());
    EXPECT_EQ(1, r.GetYPosition());
    ASSERT_EQ(RobotError::kNoError, r.GetError());
  }
  TurnLeft();
  TurnLeft();
  for (int i = 7; i > 0; i--) {
    Move();
    ASSERT_EQ(i, r.GetXPosition());
    EXPECT_EQ(1, r.GetYPosition());
    ASSERT_EQ(RobotError::kNoError, r.GetError());
  }
}

TEST_F(KarelTest, MovesNorthAndSouth) {
  Robot& r =
      Robot::InitializeInstance("worlds/1x8.w", /* enable animations */ false,
                                /* force initialize */ true);
  ASSERT_EQ(1, r.GetWorldWidth());
  ASSERT_EQ(8, r.GetWorldHeight());
  TurnLeft();  // Face north.
  for (int i = 2; i <= 8; i++) {
    Move();
    ASSERT_EQ(1, r.GetXPosition());
    EXPECT_EQ(i, r.GetYPosition());
    ASSERT_EQ(RobotError::kNoError, r.GetError());
  }
  TurnLeft();
  TurnLeft();
  for (int i = 7; i > 0; i--) {
    Move();
    ASSERT_EQ(1, r.GetXPosition());
    EXPECT_EQ(i, r.GetYPosition());
    ASSERT_EQ(RobotError::kNoError, r.GetError());
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
