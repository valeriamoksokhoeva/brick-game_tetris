/**
 * @file backend.h
 * @brief Header file for backend
 *
 * Contains all backend
 */
#ifndef BACKEND_H_
#define BACKEND_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../brick_game.h"

#define WIDTH 10  /**< Game field width */
#define HEIGHT 20 /**< Game field height */

/**
 * @brief Game status
 */
typedef enum { START, SPAWN, MOVING, ATTACHING, GAME_OVER } GameStatus;

/**
 * @brief Main game state structure
 */
typedef struct {
  int score;
  int current_fig;
  int next_fig;
  int y;
  int x;
  int rotation;
  int game_field[HEIGHT + 2][WIDTH + 2];
  GameInfo_t info;
  GameStatus status;
} GameState;

/** @brief Gets current game state instance */
GameState* getGameState();
/** @brief Initializes game field */
void initField();
/** @brief Initializes next figure display */
void initNextFigure();
/** @brief Starts or restarts game */
void startGame();
/** @brief Initializes new game */
void initGame();
/** @brief Generates next figure */
void generateNextFig();
/** @brief Gets figure block data */
int getFigure(int figure, int rotation, int x, int y);
/** @brief Copies figures to display buffers */
void copyFigToField(GameState* game_state);
/** @brief Checks for collisions */
int checkCollisions(int figure, int rotation, int x, int y);
/** @brief Rotates current figure */
void rotateFigure();
/** @brief Moves figure down */
int moveDown();
/** @brief Moves figure right */
void moveRight();
/** @brief Moves figure left */
void moveLeft();
/** @brief Cleans up game resources */
void terminateGame();
/** @brief Locks figure in place */
void stayDown();
/** @brief Checks and clears lines */
void checkLines();
/** @brief Clears line and shifts above */
void lineCleanShift(int line);
/** @brief Simple timer delay */
void waitTimer(int n);
/** @brief Main game loop logic */
void run_game(GameState* game_state);
/** @brief Updates game state */
void updateGameState(GameState* game_state);
/** @brief Toggles pause state */
void pauseGame();
/** @brief Updates high score */
void checkScore();
/** @brief Updates score and level */
void updateScoreandLevel(int lines);
/** @brief Calculates points for lines */
int calculatePoints(int n);

#endif