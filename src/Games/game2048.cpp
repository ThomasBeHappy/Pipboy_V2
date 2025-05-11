#include "game2048.h"
#include "../ui.h"
#include <TFT_eSPI.h>
#include "DFRobotDFPlayerMini.h"

// Game-specific state variables
static bool isGameOver = false;
int board[4][4];
bool hasMoved = false;

// Constants
const int GRID_SIZE = 60;
const int GRID_PADDING = 5;
const int BOARD_SIZE = 4;

extern TFT_eSPI tft;
extern DFRobotDFPlayerMini myDFPlayer;

// Helper function to get color for a tile
uint16_t getTileColor(int value) {
    switch (value) {
        case 2: return TFT_LIGHTGREY;
        case 4: return TFT_YELLOW;
        case 8: return TFT_ORANGE;
        case 16: return TFT_RED;
        case 32: return TFT_MAGENTA;
        case 64: return TFT_PURPLE;
        case 128: return TFT_BLUE;
        case 256: return TFT_CYAN;
        case 512: return TFT_GREEN;
        case 1024: return TFT_DARKGREEN;
        case 2048: return TFT_GOLD;
        default: return TFT_DARKGREY;
    }
}

void addNewTile() {
    int emptyCells = 0;
    int positions[16][2];
    
    // Find all empty cells
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == 0) {
                positions[emptyCells][0] = i;
                positions[emptyCells][1] = j;
                emptyCells++;
            }
        }
    }
    
    if (emptyCells > 0) {
        int pos = random(emptyCells);
        board[positions[pos][0]][positions[pos][1]] = (random(10) < 9) ? 2 : 4;
    }
}

void Reset2048() {
    // Clear board
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            board[i][j] = 0;
        }
    }
    
    // Add two initial tiles
    addNewTile();
    addNewTile();
    
    isGameOver = false;
    score = 0;
    tft.fillScreen(TFT_BLACK);
}

bool moveLeft() {
    bool moved = false;
    for (int i = 0; i < 4; i++) {
        for (int j = 1; j < 4; j++) {
            if (board[i][j] != 0) {
                int k = j;
                while (k > 0 && (board[i][k-1] == 0 || board[i][k-1] == board[i][k])) {
                    if (board[i][k-1] == 0) {
                        board[i][k-1] = board[i][k];
                        board[i][k] = 0;
                        moved = true;
                    } else if (board[i][k-1] == board[i][k]) {
                        board[i][k-1] *= 2;
                        score += board[i][k-1];
                        board[i][k] = 0;
                        moved = true;
                        break;
                    }
                    k--;
                }
            }
        }
    }
    return moved;
}

bool moveRight() {
    bool moved = false;
    for (int i = 0; i < 4; i++) {
        for (int j = 2; j >= 0; j--) {
            if (board[i][j] != 0) {
                int k = j;
                while (k < 3 && (board[i][k+1] == 0 || board[i][k+1] == board[i][k])) {
                    if (board[i][k+1] == 0) {
                        board[i][k+1] = board[i][k];
                        board[i][k] = 0;
                        moved = true;
                    } else if (board[i][k+1] == board[i][k]) {
                        board[i][k+1] *= 2;
                        score += board[i][k+1];
                        board[i][k] = 0;
                        moved = true;
                        break;
                    }
                    k++;
                }
            }
        }
    }
    return moved;
}

bool moveUp() {
    bool moved = false;
    for (int j = 0; j < 4; j++) {
        for (int i = 1; i < 4; i++) {
            if (board[i][j] != 0) {
                int k = i;
                while (k > 0 && (board[k-1][j] == 0 || board[k-1][j] == board[k][j])) {
                    if (board[k-1][j] == 0) {
                        board[k-1][j] = board[k][j];
                        board[k][j] = 0;
                        moved = true;
                    } else if (board[k-1][j] == board[k][j]) {
                        board[k-1][j] *= 2;
                        score += board[k-1][j];
                        board[k][j] = 0;
                        moved = true;
                        break;
                    }
                    k--;
                }
            }
        }
    }
    return moved;
}

bool moveDown() {
    bool moved = false;
    for (int j = 0; j < 4; j++) {
        for (int i = 2; i >= 0; i--) {
            if (board[i][j] != 0) {
                int k = i;
                while (k < 3 && (board[k+1][j] == 0 || board[k+1][j] == board[k][j])) {
                    if (board[k+1][j] == 0) {
                        board[k+1][j] = board[k][j];
                        board[k][j] = 0;
                        moved = true;
                    } else if (board[k+1][j] == board[k][j]) {
                        board[k+1][j] *= 2;
                        score += board[k+1][j];
                        board[k][j] = 0;
                        moved = true;
                        break;
                    }
                    k++;
                }
            }
        }
    }
    return moved;
}

bool isGameOverCheck() {
    // Check for empty cells
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == 0) return false;
        }
    }
    
    // Check for possible merges
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i < 3 && board[i][j] == board[i+1][j]) return false;
            if (j < 3 && board[i][j] == board[i][j+1]) return false;
        }
    }
    
    return true;
}

void Handle2048Input() {
    uint16_t touchX, touchY;
    if (tft.getTouch(&touchX, &touchY, 600)) {
        int centerX = tft.width() / 2;
        int centerY = tft.height() / 2;
        
        int dx = touchX - centerX;
        int dy = touchY - centerY;
        
        if (abs(dx) > abs(dy)) {
            if (dx > 0) {
                hasMoved = moveRight();
            } else {
                hasMoved = moveLeft();
            }
        } else {
            if (dy > 0) {
                hasMoved = moveDown();
            } else {
                hasMoved = moveUp();
            }
        }
        
        if (hasMoved) {
            addNewTile();
            myDFPlayer.play(5);  // Play move sound
            if (isGameOverCheck()) {
                isGameOver = true;
                myDFPlayer.play(6);  // Play game over sound
            }
        }
    }
}

void Update2048() {
    // Game logic is handled in Handle2048Input
}

void Draw2048() {
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

    // Draw board
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int x = j * (GRID_SIZE + GRID_PADDING) + GRID_PADDING;
            int y = i * (GRID_SIZE + GRID_PADDING) + GRID_PADDING + 30; // Add space for score
            
            tft.fillRect(x, y, GRID_SIZE, GRID_SIZE, getTileColor(board[i][j]));
            
            if (board[i][j] != 0) {
                tft.setTextColor(TFT_BLACK);
                tft.setTextSize(2);
                tft.setCursor(x + 20, y + 20);
                tft.print(board[i][j]);
            }
        }
    }

    // Draw score if changed
    if (lastScore != score) {
        tft.fillRect(0, 0, tft.width(), 30, TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 5);
        tft.print("Score: ");
        tft.print(score);
        lastScore = score;
    }
} 