#include <string>

void Move();

void TurnLeft();

void PutBeeper();

void PickBeeper();

bool FrontIsClear();

bool FrontIsBlocked();

bool LeftIsClear();

bool LeftIsBlocked();

bool RightIsClear();

bool RightIsBlocked();

bool HasBeepersInBag();

bool NoBeepersInBag();

bool BeepersPresent();

bool NoBeepersPresent();

bool FacingNorth();

bool NotFacingNorth();

bool FacingEast();

bool NotFacingEast();

bool FacingSouth();

bool NotFacingSouth();

bool FacingWest();

bool NotFacingWest();

// For the instructor
void LoadWorld(std::string filename);
void Finish();
