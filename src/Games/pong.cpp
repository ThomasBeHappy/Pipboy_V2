#include "pong.h"
#include "../UI/ui.h"
#include <TFT_eSPI.h>
#include "DFRobotDFPlayerMini.h"

// Game-specific state variables
static bool isGameOver = false;
int paddle1Y = 100;
int paddle2Y = 100;
float ballX = 160;
float ballY = 120;
float ballSpeedX = 2.0f;
float ballSpeedY = 2.0f;
int player1Score = 0;
int player2Score = 0;

// Constants
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 40;
const int BALL_SIZE = 8;
const int PADDLE_SPEED = 5;

extern TFT_eSPI tft;
extern DFRobotDFPlayerMini myDFPlayer;

void ResetPong() {
    paddle1Y = 100;
    paddle2Y = 100;
    ballX = 160;
    ballY = 120;
    ballSpeedX = 2.0f;
    ballSpeedY = 2.0f;
    player1Score = 0;
    player2Score = 0;
    isGameOver = false;
    tft.fillScreen(TFT_BLACK);
}

void HandlePongInput() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY, 600)) {
        // Left side controls paddle 1
        if (touchX < tft.width() / 2) {
            paddle1Y = touchY - PADDLE_HEIGHT/2;
            if (paddle1Y < 0) paddle1Y = 0;
            if (paddle1Y > tft.height() - PADDLE_HEIGHT) paddle1Y = tft.height() - PADDLE_HEIGHT;
        }
        // Right side controls paddle 2
        else {
            paddle2Y = touchY - PADDLE_HEIGHT/2;
            if (paddle2Y < 0) paddle2Y = 0;
            if (paddle2Y > tft.height() - PADDLE_HEIGHT) paddle2Y = tft.height() - PADDLE_HEIGHT;
        }
    }
}

void UpdatePong() {
    if (isGameOver) return;

    // Update ball position
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // Ball collision with top and bottom
    if (ballY <= 0 || ballY >= tft.height() - BALL_SIZE) {
        ballSpeedY = -ballSpeedY;
        myDFPlayer.play(5);  // Play bounce sound
    }

    // Ball collision with paddles
    if (ballX <= PADDLE_WIDTH && ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
        ballSpeedX = -ballSpeedX;
        myDFPlayer.play(5);  // Play bounce sound
    }
    if (ballX >= tft.width() - PADDLE_WIDTH - BALL_SIZE && 
        ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
        ballSpeedX = -ballSpeedX;
        myDFPlayer.play(5);  // Play bounce sound
    }

    // Score points
    if (ballX < 0) {
        player2Score++;
        ballX = 160;
        ballY = 120;
        ballSpeedX = 2.0f;
        ballSpeedY = 2.0f;
        myDFPlayer.play(6);  // Play score sound
    }
    if (ballX > tft.width()) {
        player1Score++;
        ballX = 160;
        ballY = 120;
        ballSpeedX = -2.0f;
        ballSpeedY = 2.0f;
        myDFPlayer.play(6);  // Play score sound
    }

    // Check for game over
    if (player1Score >= 5 || player2Score >= 5) {
        isGameOver = true;
    }
}

void DrawPong() {
    static int lastPaddle1Y = -1;
    static int lastPaddle2Y = -1;
    static int lastBallX = -1;
    static int lastBallY = -1;
    static int lastPlayer1Score = -1;
    static int lastPlayer2Score = -1;
    
    if (isGameOver) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.setCursor(100, 100);
        tft.print("Game Over!");
        tft.setCursor(80, 130);
        tft.print(player1Score);
        tft.print(" - ");
        tft.print(player2Score);
        tft.setCursor(70, 160);
        tft.print("Tap to restart");
        return;
    }

    // Draw paddles if they moved
    if (lastPaddle1Y != paddle1Y) {
        tft.fillRect(0, lastPaddle1Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_BLACK);
        tft.fillRect(0, paddle1Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_WHITE);
        lastPaddle1Y = paddle1Y;
    }
    
    if (lastPaddle2Y != paddle2Y) {
        tft.fillRect(tft.width() - PADDLE_WIDTH, lastPaddle2Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_BLACK);
        tft.fillRect(tft.width() - PADDLE_WIDTH, paddle2Y, PADDLE_WIDTH, PADDLE_HEIGHT, TFT_WHITE);
        lastPaddle2Y = paddle2Y;
    }

    // Draw ball if it moved
    if (lastBallX != (int)ballX || lastBallY != (int)ballY) {
        tft.fillRect(lastBallX, lastBallY, BALL_SIZE, BALL_SIZE, TFT_BLACK);
        tft.fillRect((int)ballX, (int)ballY, BALL_SIZE, BALL_SIZE, TFT_WHITE);
        lastBallX = (int)ballX;
        lastBallY = (int)ballY;
    }

    // Draw scores if they changed
    if (lastPlayer1Score != player1Score || lastPlayer2Score != player2Score) {
        tft.fillRect(70, 10, 180, 20, TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.setCursor(70, 10);
        tft.print(player1Score);
        tft.print(" - ");
        tft.print(player2Score);
        lastPlayer1Score = player1Score;
        lastPlayer2Score = player2Score;
    }
}