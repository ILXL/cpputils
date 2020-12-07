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
// Test walls, loading maps with walls, not walking through walls
// Test error states
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
