#include <stdio.h>
#include <stdlib.h>
#include "STM32L432KC.h"
#include "DE-DP14211.h"
#include <math.h>

#define XSTART 1
#define YSTART 5
#define TIMVAL 12000
#define STARTMSPERFRAME 100
#define ROWS 16
#define COLS 32
#define SPD 0.3

// note encodings where lowercase -> flat
#define a3 208
#define A3 220
#define b3 233
#define B3 247
#define C4 262
#define d4 277
#define D4 294
#define e4 311
#define E4 330
#define F4 349
#define g4 370
#define G4 392
#define a4 415
#define A4 440
#define b4 466
#define B4 494
#define C5 523
#define d5 554
#define D5 587
#define e5 622
#define E5 659
#define F5 698
#define g5 740
#define G5 784
#define a5 831
#define A5 880
#define b5 932
#define B5 988
#define C6 1047

char initstate[16][32];
char state[16][32];

// Objects
struct Player {
  float x;
  float y;
};

struct Goal {
  int x;
  int y;
};

// Functions
void delay(int cycles);

void drawPlayer(struct Player p, char board[16][32], char initboard[16][32]);

int movePlayer(struct Player *p, char board[16][32], char initboard[16][32], char x, char y);

void tickTimer(char board[16][32], char initboard[16][32], int tim, int timval);

void drawBoard(char board[16][32], char oldboard[16][32]);

void copyBoard(char board[16][32], char newboard[16][32]);

int checkGoal(struct Player p, struct Goal g[4], int n);

void drawFrame(char (*boards[8])[16][32], char (*initboards[8])[16][32], int frame);

int main(void);

// Matrix Presets

char sans[16][32] = {{0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b01, 0b11, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b11, 0b01, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b01, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b01, 0b00, 0b01, 0b00, 0b01, 0b00, 0b01, 0b00, 0b01, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00}};

char happy[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char sad[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0},
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char testmaze[16][32] = {{0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1}, 
                        {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0}, 
                        {0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1}};


char start_count1[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b11, 0b0, 0b11, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b11, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char start_count2[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b11, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char start_count3[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b11, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};


char empty[16][32] = {{0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00}};

char start_ani1[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b1, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, {0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b11, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char start_ani2[16][32] = {{0b11, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b11, 0b0, 0b0, 0b11, 0b11, 0b0, 0b11, 0b11, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b11, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b1, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, {0b11, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b11, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b11, 0b11, 0b0, 0b11, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b0, 0b11, 0b11, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b11, 0b0, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b11, 0b1, 0b1, 0b0, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}};

char start_ani3[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char record_ani1[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0}, {0b0, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b1, 0b11, 0b11, 0b11, 0b1, 0b11, 0b11, 0b11, 0b1, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b11, 0b0}, {0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b11, 0b0, 0b0, 0b0}, {0b0, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char record_ani2[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b11, 0b11, 0b1, 0b0, 0b1, 0b11, 0b11, 0b1, 0b11, 0b11, 0b1, 0b0, 0b1, 0b11, 0b11, 0b1, 0b11, 0b11, 0b1, 0b0}, {0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0}, {0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b11, 0b1, 0b1, 0b1, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b0, 0b11, 0b11, 0b11, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

// Matrix Animations

char (*start_ani[3])[16][32] = {&start_ani2, &start_ani3, &start_ani1};
char (*start_ani_init[3])[16][32] = {&start_ani1, &start_ani2, &start_ani3};

char (*record_ani[2])[16][32] = {&record_ani2, &record_ani1};
char (*record_ani_init[2])[16][32] = {&record_ani1, &record_ani2};

// Sounds

const int moveloop[][2] = {
{80,	20},
{0,	100},
{60,	20},
{0,	100},
{0, 0}};

const int winsound[][2] = {
{C4, 50},
{E4, 50},
{0, 0}};

const int recordsound[][2] = {
{G4, 50},
{B5, 50},
{C5, 50},
{0, 0}};

const int losesound[][2] = {
{B3, 50},
{b3, 50},
{A3, 50},
{0, 0}};

const int userin[][2] = {
{D4, 50},
{0, 0}};

const int countsound[][2] = {
{A4, 150},
{0, 0}};

const int gosound[][2] = {
{A5, 150},
{0, 0}};