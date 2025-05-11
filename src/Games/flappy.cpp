#include "flappy.h"
#include "../ui.h"
#include <TFT_eSPI.h>
#include "DFRobotDFPlayerMini.h"

// Game-specific state variables
static bool isGameOver = false;
float birdY = 120;  // Start in middle of screen
float birdVelocity = 0;
int pipeX = 320;    // Start off screen
int pipeGap = 80;   // Gap between pipes
int pipeHeight = 120; // Height of top pipe

// Touch state tracking
static unsigned long lastTouchTime = 0;
static const unsigned long TOUCH_DEBOUNCE = 100; // 100ms debounce time

// Constants
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -6.0f;
const int PIPE_WIDTH = 40;
const int BIRD_SIZE = 20;
const int GROUND_HEIGHT = 20;
const int MIN_PIPE_HEIGHT = 50;
const int MAX_PIPE_HEIGHT = 150;

extern TFT_eSPI tft;
extern DFRobotDFPlayerMini myDFPlayer;

void GenerateNewPipeHeight() {
    pipeHeight = random(MIN_PIPE_HEIGHT, MAX_PIPE_HEIGHT);
}

void ResetFlappy() {
    birdY = 120;
    birdVelocity = 0;
    pipeX = 320;
    isGameOver = false;
    GenerateNewPipeHeight();
    tft.fillScreen(TFT_BLACK);  // Clear everything on reset
}

bool AudioIsPlaying() {
    return digitalRead(32); // High when playing
}

void playFlapSound() {
    if(AudioIsPlaying()) {
        myDFPlayer.play(5);  // Play flap sound
    }
}

void playPointSound() {
    myDFPlayer.play(6);  // Play point sound
}

void HandleFlappyInput() {
    uint16_t touchX, touchY;
    bool isTouched = tft.getTouch(&touchX, &touchY, 600);
    unsigned long currentTime = millis();
    
    // Check for touch press with debounce
    if (isTouched && (currentTime - lastTouchTime > TOUCH_DEBOUNCE)) {
        if (isGameOver) {
            ResetFlappy();
        } else {
            birdVelocity = JUMP_FORCE;
            playFlapSound();
        }
        lastTouchTime = currentTime;
    }
    
}

void UpdateFlappy() {
    if (isGameOver) return;

    // Update bird position
    birdVelocity += GRAVITY;
    birdY += birdVelocity;

    // Update pipe position
    pipeX -= 2;
    if (pipeX < -PIPE_WIDTH) {
        pipeX = 320;
        score++;
        GenerateNewPipeHeight();
        playPointSound();
    }

    // Check collisions
    if (birdY < 0 || birdY > 240 - GROUND_HEIGHT - BIRD_SIZE) {
        isGameOver = true;
        return;
    }

    // Improved pipe collision detection
    if (pipeX < 40 + BIRD_SIZE/2 && pipeX > -PIPE_WIDTH) {
        if (birdY < pipeHeight || birdY > pipeHeight + pipeGap) {
            isGameOver = true;
            return;
        }
    }
}

void DrawFlappy() {
    static int lastBirdY = -1;
    static int lastPipeX = -1;
    static int lastScore = -1;
    
    if (isGameOver) {
        // Draw game over screen
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

    // Only redraw what changed
    if (lastPipeX != pipeX) {
        // Erase old pipe
        if (lastPipeX != -1) {
            tft.fillRect(lastPipeX, 0, PIPE_WIDTH, 240, TFT_BLACK);
        }
        // Draw new pipe
        tft.fillRect(pipeX, 0, PIPE_WIDTH, pipeHeight, TFT_GREEN);
        tft.fillRect(pipeX, pipeHeight + pipeGap, PIPE_WIDTH, 240 - (pipeHeight + pipeGap), TFT_GREEN);
        lastPipeX = pipeX;
    }

    if (lastScore != score) {
        tft.fillRect(10, 10, 60, 20, TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 10);
        tft.print(score);
        lastScore = score;
    }

    if (lastBirdY != (int)birdY) {
        // Erase old bird position
        if (lastBirdY != -1) {
            tft.fillCircle(40, lastBirdY, BIRD_SIZE/2, TFT_BLACK);
        }
        // Draw new bird position
        tft.fillCircle(40, (int)birdY, BIRD_SIZE/2, TFT_YELLOW);
        lastBirdY = (int)birdY;
    }

    // Draw static elements only once
    tft.fillRect(0, 240 - GROUND_HEIGHT, 320, GROUND_HEIGHT, TFT_GREEN);
}
