// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "karel.h"

#include "src/robot.h"

void LoadWorld(std::string filename) {
  try {
    karel::Robot& r = karel::Robot::InitializeInstance(filename);
  } catch (std::string error) {
    std::cout << "Error loading world file " << filename << ": \n" << error << std::endl;
    exit(1);
  }
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
