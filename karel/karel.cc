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
    std::cout << "Error loading world file " << filename << ": \n"
              << error << std::endl;
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

bool HasBeepersInBag() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.HasBeepersInBag();
}

bool NoBeepersInBag() { return !HasBeepersInBag(); }

bool BeepersPresent() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.BeepersPresent();
}

bool NoBeepersPresent() { return !BeepersPresent(); }

bool FrontIsClear() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FrontIsClear();
}

bool FrontIsBlocked() { return !FrontIsClear(); }

bool LeftIsClear() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.LeftIsClear();
}

bool LeftIsBlocked() { return !LeftIsClear(); }

bool RightIsClear() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.RightIsClear();
}

bool RightIsBlocked() { return !RightIsClear(); }

bool FacingNorth() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FacingNorth();
}

bool NotFacingNorth() { return !FacingNorth(); }

bool FacingEast() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FacingEast();
}

bool NotFacingEast() { return !FacingEast(); }

bool FacingSouth() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FacingSouth();
}

bool NotFacingSouth() { return !FacingSouth(); }

bool FacingWest() {
  karel::Robot& r = karel::Robot::GetInstance();
  return r.FacingWest();
}

bool NotFacingWest() { return !FacingWest(); }

void Finish() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.Finish();
}

void EnableCSVOutput() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.EnableCSVOutput();
}

void EnablePromptBeforeAction() {
  karel::Robot& r = karel::Robot::GetInstance();
  r.EnablePromptBeforeAction();
}
