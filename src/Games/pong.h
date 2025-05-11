#pragma once
#include "../ui.h"
#include <TFT_eSPI.h>
#include "games.h"

// Game functions
void UpdatePong();
void DrawPong();
void ResetPong();
void HandlePongInput(); 