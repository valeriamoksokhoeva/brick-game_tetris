/**
 * @file brick_game.h
 * @brief Main header file for Brick Game
 *
 * Contains data structure definitions and functions for game control
 */

#ifndef BRICK_GAME_H_
#define BRICK_GAME_H_

#include <stdbool.h>

/**
 * @brief User action enumeration
 *
 * Defines all possible user input actions
 */
typedef enum UserAction {
  Start,      ///< Start the game
  Pause,      ///< Pause the game
  Terminate,  ///< Terminate the game
  Left,       ///< Move left
  Right,      ///< Move right
  Up,         ///< Move up
  Down,       ///< Move down
  Action      ///< Special action (rotate figure)
} UserAction_t;

/**
 * @brief Game state information structure
 *
 * Contains all necessary data for displaying current game state
 */
typedef struct GameInfo {
  int** field;     ///< 2D array representing game field
  int** next;      ///< 2D array showing next figure
  int score;       ///< Current score
  int high_score;  ///< High score record
  int level;       ///< Current level
  int speed;       ///< Current game speed
  int pause;       ///< Pause flag (1 - game paused, 0 - game active)
} GameInfo_t;

/**
 * @brief User input handler
 *
 * @param action User action from UserAction_t enum
 * @param hold Key hold flag
 *
 * Processes all user actions and modifies game state accordingly.
 */
void userInput(UserAction_t action, bool hold);

/**
 * @brief Get current game state
 *
 * @return GameInfo_t Structure containing current game state
 *
 * Should be called regularly for UI updates.
 * Returns current field state, score, level and other parameters.
 * Automatically moves figure down when game timer expires.
 */
GameInfo_t updateCurrentState();

#endif