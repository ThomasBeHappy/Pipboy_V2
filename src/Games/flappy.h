#pragma once
#include "../ui.h"
#include <TFT_eSPI.h>
#include "games.h"  // Include games.h to access score


// Game functions
void UpdateFlappy();
void DrawFlappy();
void ResetFlappy();
void HandleFlappyInput();