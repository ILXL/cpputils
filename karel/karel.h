// Copyright 2020 Paul Salvador Inventado and Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <string>

/////////////////////////////////////////////////////////
// Methods for core Karel functionality                //
// These methods allow interaction with Karel.         //
/////////////////////////////////////////////////////////

/**
 * Move Karel forward one step. Results in an error if Karel cannot move
 * forward because of a wall or an edge.
 */
void Move();

/**
 * Turns Karel to the left.
 */
void TurnLeft();

/**
 * Places a beeper from Karel's beeper bag onto the current cell where Karel
 * is. Results in an error if Karel has no beepers left in their bag.
 */
void PutBeeper();

/**
 * Picks a beeper from the cell where Karel is standing and places it in
 * their beeper bag. Results in an error if there are no beepers in Karel's
 * current cell.
 */
void PickBeeper();

/**
 * Returns true if Karel has beepers in their bag, or false otherwise.
 */
bool HasBeepersInBag();

/**
 * Returns true if Karel has no beepers in their bag, or true otherwise.
 */
bool NoBeepersInBag();

/**
 * Returns true if Karel is standing on a cell with at least one beeper, or
 * false otherwise.
 */
bool BeepersPresent();

/**
 * Returns true if Karel is standing on a cell no beepers at all, or
 * false otherwise.
 */
bool NoBeepersPresent();

/**
 * Returns true if there is no wall nor edge in front of Karel and they could
 * move forward, or false otherwise.
 */
bool FrontIsClear();

/**
 * Returns true if there is a wall or edge in front of Karel or false otherwise.
 */
bool FrontIsBlocked();

/**
 * Returns true if there is no wall nor edge directly to Karel's left, or false
 * otherwise.
 */
bool LeftIsClear();

/**
 * Returns true if there is a wall or edge directly to Karel's left, or false
 * otherwise.
 */
bool LeftIsBlocked();

/**
 * Returns true if there is no wall nor edge directly to Karel's right, or false
 * otherwise.
 */
bool RightIsClear();

/**
 * Returns true if there is a wall or edge directly to Karel's right or false
 * otherwise.
 */
bool RightIsBlocked();

/**
 * Returns true if Karel is facing north or false otherwise.
 */
bool FacingNorth();

/**
 * Returns true if Karel is not facing north or false otherwise.
 */
bool NotFacingNorth();

/**
 * Returns true if Karel is facing east or false otherwise.
 */
bool FacingEast();

/**
 * Returns true if Karel is not facing east or false otherwise.
 */
bool NotFacingEast();

/**
 * Returns true if Karel is facing south or false otherwise.
 */
bool FacingSouth();

/**
 * Returns true if Karel is facing south or false otherwise.
 */
bool NotFacingSouth();

/**
 * Returns true if Karel is facing west or false otherwise.
 */
bool FacingWest();

/**
 * Returns true if Karel is not facing west or false otherwise.
 */
bool NotFacingWest();

/////////////////////////////////////////////////////////////////////////////
// Methods for setting up a Karel program.                                 //
// Instructors may use these methods wrapping student's Karel commands.    //
/////////////////////////////////////////////////////////////////////////////

/**
 * Loads a Karel world from a file.
 * If this is not the first Karel function called a default world will be
 * created instead and this will have no effect.
 */
void LoadWorld(std::string filename);

/**
 * Completes a Karel program. Continues to show the image but will not
 * do any more actions.
 */
void Finish();

/////////////////////////////////////////////////////////////////////////////
// Methods for improving the accessibility of Karel                        //
/////////////////////////////////////////////////////////////////////////////

/**
 * Enables Karel CSV output. This will print Karel's world to a CSV between
 * each action, and prompt to continue to the next action. May be used by
 * screen-reader users to inspect Karel's world.
 */
void EnableCSVOutput();

/**
 * Causes Karel to wait between each action function (Move, TurnLeft,
 * PutBeeper, PickBeeper) until the user enters input into the terminal to
 * proceed.
 */
void EnablePromptBeforeAction();
