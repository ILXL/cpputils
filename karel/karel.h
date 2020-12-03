#include "../graphics/image.h"
#include "../graphics/image_event.h"

void Move();

void TurnLeft();

void PutBeeper();

void PickBeeper();

bool FrontIsClear();

bool HasBeepersInBag();

// Set-up for professors?
// https://csis.pace.edu/~bergin/KarelJava2ed/KJRdocs/kareltherobot/World.html
void SetDimensions(int width, int height);

void PlaceBeeperAt(int x, int y);

void Finish();

void LoadWorld(std::string filename);
