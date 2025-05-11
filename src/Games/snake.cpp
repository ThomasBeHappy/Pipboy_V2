#include "snake.h"
#include "../ui.h"
#include <TFT_eSPI.h>
#include "DFRobotDFPlayerMini.h"

// Game-specific state variables
static bool isGameOver = false;
int snakeX[100];
int snakeY[100];
int snakeLength = 3;
int foodX = 0;
int foodY = 0;
int direction = 0; // 0=right, 1=down, 2=left, 3=up

// Constants
const int GRID_SIZE = 10;
const int GRID_WIDTH = 32;
const int GRID_HEIGHT = 24;

extern TFT_eSPI tft;
extern DFRobotDFPlayerMini myDFPlayer;

void GenerateFood() {
    bool validPosition;
    do {
        validPosition = true;
        foodX = random(GRID_WIDTH) * GRID_SIZE;
        foodY = random(GRID_HEIGHT) * GRID_SIZE;
        
        // Check if food spawns on snake
        for (int i = 0; i < snakeLength; i++) {
            if (snakeX[i] == foodX && snakeY[i] == foodY) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
}

void ResetSnake() {
    snakeLength = 3;
    direction = 0;
    isGameOver = false;
    
    // Initialize snake in middle of screen
    int startX = (GRID_WIDTH / 2) * GRID_SIZE;
    int startY = (GRID_HEIGHT / 2) * GRID_SIZE;
    
    for (int i = 0; i < snakeLength; i++) {
        snakeX[i] = startX - (i * GRID_SIZE);
        snakeY[i] = startY;
    }
    
    GenerateFood();
    tft.fillScreen(TFT_BLACK);
}

void HandleSnakeInput() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY, 600)) {
        // Calculate center of screen
        int centerX = tft.width() / 2;
        int centerY = tft.height() / 2;
        
        // Determine direction based on touch position relative to center
        int dx = touchX - centerX;
        int dy = touchY - centerY;
        
        // Prevent 180-degree turns
        if (abs(dx) > abs(dy)) {
            if (dx > 0 && direction != 2) direction = 0;  // Right
            else if (dx < 0 && direction != 0) direction = 2;  // Left
        } else {
            if (dy > 0 && direction != 3) direction = 1;  // Down
            else if (dy < 0 && direction != 1) direction = 3;  // Up
        }
    }
}

void UpdateSnake() {
    if (isGameOver) return;

    // Move snake body
    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i-1];
        snakeY[i] = snakeY[i-1];
    }

    // Move snake head
    switch (direction) {
        case 0: snakeX[0] += GRID_SIZE; break;  // Right
        case 1: snakeY[0] += GRID_SIZE; break;  // Down
        case 2: snakeX[0] -= GRID_SIZE; break;  // Left
        case 3: snakeY[0] -= GRID_SIZE; break;  // Up
    }

    // Check wall collision
    if (snakeX[0] < 0 || snakeX[0] >= tft.width() ||
        snakeY[0] < 0 || snakeY[0] >= tft.height()) {
        isGameOver = true;
        return;
    }

    // Check self collision
    for (int i = 1; i < snakeLength; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            isGameOver = true;
            return;
        }
    }

    // Check food collision
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        snakeLength++;
        score++;
        GenerateFood();
        myDFPlayer.play(6);  // Play point sound
    }
}

void DrawSnake() {
    static int lastSnakeLength = -1;
    static int lastFoodX = -1;
    static int lastFoodY = -1;
    static int lastScore = -1;
    
    if (isGameOver) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.setCursor(100, 100);
        tft.print("Game Over!");
        tft.setCursor(80, 130);
        tft.print("Score: ");
        tft.print(score);
        tft.setCursor(70, 160);
        tft.print("Tap to restart");
        return;
    }

    // Draw food if it changed position
    if (lastFoodX != foodX || lastFoodY != foodY) {
        if (lastFoodX != -1) {
            tft.fillRect(lastFoodX, lastFoodY, GRID_SIZE, GRID_SIZE, TFT_BLACK);
        }
        tft.fillRect(foodX, foodY, GRID_SIZE, GRID_SIZE, TFT_RED);
        lastFoodX = foodX;
        lastFoodY = foodY;
    }

    // Draw snake if length changed
    if (lastSnakeLength != snakeLength) {
        tft.fillRect(0, 0, tft.width(), tft.height(), TFT_BLACK);
        lastSnakeLength = snakeLength;
    }

    // Draw snake
    for (int i = 0; i < snakeLength; i++) {
        tft.fillRect(snakeX[i], snakeY[i], GRID_SIZE, GRID_SIZE, TFT_GREEN);
    }

    // Draw score if changed
    if (lastScore != score) {
        tft.fillRect(10, 10, 60, 20, TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 10);
        tft.print(score);
        lastScore = score;
    }
} 