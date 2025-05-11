#include "games.h"
#include "../ui.h"
#include <TFT_eSPI.h>
#include "DFRobotDFPlayerMini.h"

// Game state variables
bool isPlaying = false;
int score = 0;
GameType currentGame = GAME_FLAPPY;

// Common game constants
const int RETURN_BUTTON_X = 10;
const int RETURN_BUTTON_Y = 10;
const int RETURN_BUTTON_WIDTH = 60;
const int RETURN_BUTTON_HEIGHT = 30;

extern TFT_eSPI tft;
extern DFRobotDFPlayerMini myDFPlayer;

void StartGame(GameType game) {
    currentGame = game;
    isPlaying = true;
    score = 0;
    
    switch (game) {
        case GAME_FLAPPY:
            ResetFlappy();
            break;
        case GAME_SNAKE:
            ResetSnake();
            break;
        case GAME_PONG:
            ResetPong();
            break;
        case GAME_2048:
            Reset2048();
            break;
        default:
            break;
    }
}

void ReturnToWatch() {
    isPlaying = false;
    currentGame = GAME_NONE;
    lv_scr_load(ui_Main);
}

void HandleGameInput() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY, 600)) {
        // Check if return button was pressed
        if (touchX >= RETURN_BUTTON_X && touchX <= RETURN_BUTTON_X + RETURN_BUTTON_WIDTH &&
            touchY >= RETURN_BUTTON_Y && touchY <= RETURN_BUTTON_Y + RETURN_BUTTON_HEIGHT) {
            ReturnToWatch();
            return;
        }
        
        // Handle game-specific input
        switch (currentGame) {
            case GAME_FLAPPY:
                HandleFlappyInput();
                break;
            case GAME_SNAKE:
                HandleSnakeInput();
                break;
            case GAME_PONG:
                HandlePongInput();
                break;
            case GAME_2048:
                Handle2048Input();
                break;
            default:
                break;
        }
    }
}

void UpdateGame() {
    if (!isPlaying) return;
    
    switch (currentGame) {
        case GAME_FLAPPY:
            UpdateFlappy();
            break;
        case GAME_SNAKE:
            UpdateSnake();
            break;
        case GAME_PONG:
            UpdatePong();
            break;
        case GAME_2048:
            Update2048();
            break;
        default:
            break;
    }
}

void DrawGame() {
    if (!isPlaying) return;
    
    // Draw common elements
    tft.fillRect(RETURN_BUTTON_X, RETURN_BUTTON_Y, RETURN_BUTTON_WIDTH, RETURN_BUTTON_HEIGHT, TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(RETURN_BUTTON_X + 5, RETURN_BUTTON_Y + 10);
    tft.print("Return");
    
    // Draw game-specific elements
    switch (currentGame) {
        case GAME_FLAPPY:
            DrawFlappy();
            break;
        case GAME_SNAKE:
            DrawSnake();
            break;
        case GAME_PONG:
            DrawPong();
            break;
        case GAME_2048:
            Draw2048();
            break;
        default:
            break;
    }
}

void ResetGame() {
    switch (currentGame) {
        case GAME_FLAPPY:
            ResetFlappy();
            break;
        case GAME_SNAKE:
            ResetSnake();
            break;
        case GAME_PONG:
            ResetPong();
            break;
        case GAME_2048:
            Reset2048();
            break;
        default:
            break;
    }
}
