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
// Test loading bad files and helpful output
// Test CSV output: walls and beepers

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

TEST(KarelTest, GetsKarelInstance) {
  Robot& r = Robot::GetInstance(/* enable animations */ false,
                                /* force initialize */ true);
}

TEST(KarelTest, DefaultWorld) {
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
  // Facing east, so the right is blocked.
  EXPECT_FALSE(RightIsClear());
  EXPECT_TRUE(RightIsBlocked());
  EXPECT_TRUE(LeftIsClear());
  EXPECT_FALSE(LeftIsBlocked());
}

TEST(KarelTest, LoadsWorld) {
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

  ASSERT_TRUE(FrontIsClear());
  ASSERT_FALSE(FrontIsBlocked());
  EXPECT_FALSE(RightIsClear());
  EXPECT_TRUE(RightIsBlocked());
  EXPECT_FALSE(LeftIsClear());
  EXPECT_TRUE(LeftIsBlocked());
}

TEST(KarelTest, LoadsWorldWithInfinityBeepers) {
  // INFINITY vs INFINITE
  Robot& r = Robot::InitializeInstance("worlds/8x1.w",
                                       /* enable animations */ false,
                                       /* force initialize */ true);
  ASSERT_TRUE(IsBasicallyInfinite(r.GetNumBeepersInBag()));
}

TEST(KarelTest, LoadsWorldWithNoBeepersInBag) {
  Robot& r = Robot::InitializeInstance("worlds/beepers.w",
                                       /* enable animations */ false,
                                       /* force initialize */ true);
  EXPECT_EQ(0, r.GetNumBeepersInBag());
}

TEST(KarelTest, LoadsWorldWithFiniteBeepers) {
  Robot& r = Robot::InitializeInstance("worlds/inner_walls.w",
                                       /* enable animations */ false,
                                       /* force initialize */ true);
  EXPECT_EQ(42, r.GetNumBeepersInBag());
}

TEST(KarelTest, LoadsWorldWithBeepersInCells) {
  Robot& r = Robot::InitializeInstance("worlds/beepers.w",
                                       /* enable animations */ false,
                                       /* force initialize */ true);
  EXPECT_EQ(1, r.GetCell(2, 1).GetNumBeepers());
  EXPECT_EQ(2, r.GetCell(3, 1).GetNumBeepers());
  EXPECT_EQ(3, r.GetCell(3, 2).GetNumBeepers());
  EXPECT_EQ(4, r.GetCell(4, 4).GetNumBeepers());
}

TEST(KarelTest, LoadsWorldWithOuterWalls) {
  Robot& r = Robot::InitializeInstance("worlds/outer_walls.w",
                                       /* enable animations */ false,
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

TEST(KarelTest, LoadsWorldWithInnerWalls) {
  Robot& r = Robot::InitializeInstance("worlds/inner_walls.w",
                                       /* enable animations */ false,
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

TEST(KarelTest, CannotMoveThroughOuterWalls) {
  for (int i = 0; i < 4; i++) {
    Robot& r = Robot::InitializeInstance("worlds/outer_walls.w",
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

TEST(KarelTest, CannotMoveThroughInnerWalls) {
  for (int i = 0; i < 4; i++) {
    Robot& r = Robot::InitializeInstance("worlds/inner_walls.w",
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

TEST(KarelTest, CannotMoveThroughWorldEdges) {
  for (int i = 0; i < 4; i++) {
    Robot& r = Robot::InitializeInstance("worlds/2x1.w",
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

TEST(KarelTest, PutsAndPicksBeeper) {
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

TEST(KarelTest, PicksManyBeepers) {
  Robot& r = Robot::InitializeInstance("worlds/beepers.w",
                                       /* enable animations */ false,
                                       /* force initialize */ true);
  EXPECT_FALSE(BeepersPresent());
  EXPECT_TRUE(NoBeepersInBag());
  EXPECT_EQ(0, r.GetNumBeepersInBag());

  Move();
  // One beeper.
  EXPECT_TRUE(BeepersPresent());
  EXPECT_FALSE(NoBeepersPresent());

  PickBeeper();
  ASSERT_EQ(RobotError::kNoError, r.GetError());
  EXPECT_TRUE(HasBeepersInBag());
  EXPECT_FALSE(NoBeepersInBag());
  EXPECT_EQ(1, r.GetNumBeepersInBag());
  EXPECT_FALSE(BeepersPresent());
  EXPECT_TRUE(NoBeepersPresent());

  Move();
  // Two beepers.
  EXPECT_TRUE(BeepersPresent());
  EXPECT_FALSE(NoBeepersPresent());

  PickBeeper();
  EXPECT_TRUE(BeepersPresent());
  EXPECT_FALSE(NoBeepersPresent());

  PickBeeper();
  ASSERT_EQ(RobotError::kNoError, r.GetError());
  EXPECT_TRUE(HasBeepersInBag());
  EXPECT_FALSE(NoBeepersInBag());
  EXPECT_EQ(3, r.GetNumBeepersInBag());
  EXPECT_FALSE(BeepersPresent());
  EXPECT_TRUE(NoBeepersPresent());

  TurnLeft();
  Move();
  // Three beepers.
  EXPECT_TRUE(BeepersPresent());
  EXPECT_FALSE(NoBeepersPresent());

  PickBeeper();
  EXPECT_TRUE(BeepersPresent());
  EXPECT_FALSE(NoBeepersPresent());

  PickBeeper();
  EXPECT_TRUE(BeepersPresent());
  EXPECT_FALSE(NoBeepersPresent());

  PickBeeper();
  ASSERT_EQ(RobotError::kNoError, r.GetError());
  EXPECT_TRUE(HasBeepersInBag());
  EXPECT_FALSE(NoBeepersInBag());
  EXPECT_EQ(6, r.GetNumBeepersInBag());
  EXPECT_FALSE(BeepersPresent());
  EXPECT_TRUE(NoBeepersPresent());
}

TEST(KarelTest, CannotPickMissingBeeper) {
  Robot& r = Robot::GetInstance(/* enable animations */ false,
                                /* force initialize */ true);
  ASSERT_EQ(RobotError::kNoError, r.GetError());
  PickBeeper();
  ASSERT_EQ(RobotError::kCannotPickBeeper, r.GetError());
}

TEST(KarelTest, CannotPutWhenBeeperBagEmpty) {
  Robot& r = Robot::InitializeInstance("worlds/beepers.w",
                                       /* enable animations */ false,
                                       /* force initialize */ true);
  ASSERT_EQ(RobotError::kNoError, r.GetError());
  PutBeeper();
  ASSERT_EQ(RobotError::kCannotPutBeeper, r.GetError());
}

TEST(KarelTest, TurnsLeft) {
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

TEST(KarelTest, MovesEastAndWest) {
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

TEST(KarelTest, MovesNorthAndSouth) {
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

TEST(KarelTest, DoesNotTakeActionAfterErrorState) {
  Robot& r =
      Robot::InitializeInstance("worlds/beepers.w", /* enable graphics */ false,
                                /* force initialize */ true);
  EXPECT_EQ(Orientation::kEast, r.GetOrientation());
  EXPECT_EQ(1, r.GetXPosition());
  EXPECT_EQ(1, r.GetYPosition());
  EXPECT_FALSE(BeepersPresent());
  EXPECT_EQ(0, r.GetNumBeepersInBag());
  EXPECT_EQ(RobotError::kNoError, r.GetError());

  // Create an error by picking a non-existant beeper.
  PickBeeper();
  EXPECT_EQ(RobotError::kCannotPickBeeper, r.GetError());
  EXPECT_EQ(Orientation::kEast, r.GetOrientation());
  EXPECT_EQ(1, r.GetXPosition());
  EXPECT_EQ(1, r.GetYPosition());
  EXPECT_FALSE(BeepersPresent());
  // Didn't pick either.
  EXPECT_EQ(0, r.GetNumBeepersInBag());

  // Now it cannot move or turn and the error state stays as it was before.
  Move();
  EXPECT_EQ(RobotError::kCannotPickBeeper, r.GetError());
  EXPECT_EQ(Orientation::kEast, r.GetOrientation());
  EXPECT_EQ(1, r.GetXPosition());
  EXPECT_EQ(1, r.GetYPosition());

  TurnLeft();
  EXPECT_EQ(RobotError::kCannotPickBeeper, r.GetError());
  EXPECT_EQ(Orientation::kEast, r.GetOrientation());
  EXPECT_EQ(1, r.GetXPosition());
  EXPECT_EQ(1, r.GetYPosition());

  PutBeeper();
  EXPECT_EQ(RobotError::kCannotPickBeeper, r.GetError());
  EXPECT_FALSE(BeepersPresent());
  EXPECT_EQ(0, r.GetNumBeepersInBag());
}

TEST(KarelTest, SavesWorldBmp) {
  Robot& r = Robot::GetInstance(/* enable graphics */ false,
                                /* force initialize */ true);
  std::string name = "test_world.bmp";
  std::ifstream stream(name.c_str());
  ASSERT_FALSE(stream.good());
  r.SaveWorldBmp(name);
  graphics::Image image;
  image.Load(name);
  ASSERT_TRUE(image.GetWidth() > 0);
  ASSERT_TRUE(image.GetHeight() > 0);
  remove(name.c_str());
}

TEST(KarelTest, PromptsBetweenActionsWhenSet) {
  std::streambuf* original = std::cin.rdbuf();
  std::istringstream stream("c\nc\n");
  std::cin.rdbuf(stream.rdbuf());

  std::streambuf* original_out = std::cout.rdbuf();
  std::stringstream stream_out;
  std::cout.rdbuf(stream_out.rdbuf());

  Robot& r = Robot::InitializeInstance("worlds/2x1.w",
                                       /* enable graphics */ false,
                                       /* force initialize */ true);
  Move();
  TurnLeft();
  TurnLeft();
  EXPECT_EQ(2, r.GetXPosition());
  EXPECT_EQ(Orientation::kWest, r.GetOrientation());
  EXPECT_THAT(
      stream_out.str(),
      Not(testing::MatchesRegex(".*Enter any character to continue.*Enter any "
                                "character to continue.*")));

  EnablePromptBeforeAction();
  Move();
  TurnLeft();
  EXPECT_EQ(1, r.GetXPosition());
  EXPECT_EQ(Orientation::kSouth, r.GetOrientation());
  EXPECT_THAT(stream_out.str(),
              testing::MatchesRegex(".*Enter any character to continue.*Enter "
                                    "any character to continue.*"));

  std::cin.rdbuf(original);
  std::cout.rdbuf(original_out);
}

TEST(KarelTest, PromptsBetweenActionsWhenCSVOutputSet) {
  std::streambuf* original = std::cin.rdbuf();
  std::istringstream stream("ada\nlovelace\n");
  std::cin.rdbuf(stream.rdbuf());

  std::streambuf* original_out = std::cout.rdbuf();
  std::stringstream stream_out;
  std::cout.rdbuf(stream_out.rdbuf());

  Robot& r = Robot::InitializeInstance("worlds/2x1.w",
                                       /* enable graphics */ false,
                                       /* force initialize */ true);
  Move();
  TurnLeft();
  TurnLeft();
  EXPECT_EQ(2, r.GetXPosition());
  EXPECT_EQ(Orientation::kWest, r.GetOrientation());
  EXPECT_THAT(
      stream_out.str(),
      Not(testing::MatchesRegex(".*Enter any character to continue.*Enter any "
                                "character to continue.*")));

  EnableCSVOutput();
  Move();
  TurnLeft();
  EXPECT_EQ(1, r.GetXPosition());
  EXPECT_EQ(Orientation::kSouth, r.GetOrientation());
  EXPECT_THAT(stream_out.str(),
              testing::MatchesRegex(".*Enter any character to continue.*Enter "
                                    "any character to continue.*"));

  std::cin.rdbuf(original);
  std::cout.rdbuf(original_out);
  remove("karel.csv");
}

TEST(KarelTest, GeneratesCSVOutput) {
  Robot& r = Robot::InitializeInstance("worlds/2x1.w",
                                       /* enable graphics */ false,
                                       /* force initialize */ true);
  std::ifstream stream("karel.csv");
  ASSERT_FALSE(stream.good());

  EnableCSVOutput();
  // Should immediately write the world to a CSV file.
  stream = std::ifstream("karel.csv");
  ASSERT_TRUE(stream.good());

  // The first line of the CSV is the world with karel in the first cell.
  std::string line;
  ASSERT_TRUE(std::getline(stream, line));
  ASSERT_TRUE(line.size() > 0);
  ASSERT_EQ(line, "\"ke o (1,1)\",,\"o (2,1)\",");

  remove("karel.csv");
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
