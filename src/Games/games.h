#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "DFRobotDFPlayerMini.h"

// Game types
enum GameType {
    GAME_FLAPPY,
    GAME_SNAKE,
    GAME_PONG,
    GAME_2048,
    GAME_NONE
};

// Game state
extern bool isPlaying;
extern int score;
extern GameType currentGame;

// Game functions
void StartGame(GameType game);
void UpdateGame();
void DrawGame();
void HandleGameInput();
void ResetGame();
void ReturnToWatch();
void PlayGameSound(uint8_t _sound);

// Game-specific functions
void UpdateFlappy();
void DrawFlappy();
void ResetFlappy();
void HandleFlappyInput();

void UpdateSnake();
void DrawSnake();
void ResetSnake();
void HandleSnakeInput();

void UpdatePong();
void DrawPong();
void ResetPong();
void HandlePongInput();

void Update2048();
void Draw2048();
void Reset2048();
void Handle2048Input();
