#ifndef FRONTEND_H_
#define FRONTEND_H_

#include <ncurses.h>
#include <stdio.h>

#include "../../brick_game/brick_game.h"

#define WIDTH 10
#define HEIGHT 20

int get_inputs(UserAction_t* action);
void start_ncurses();
WINDOW* winMainField();
WINDOW* winNextFigure();
WINDOW* winButtons();
WINDOW* winScoreSpeed();
void drawScoreSpeed(WINDOW* win, int score, int speed, int high_score,
                    int level);
void drawPause(WINDOW* winField, WINDOW* winNext, int pause_status);
void drawButtons(WINDOW* buttons);
void drawField(WINDOW* win, int** field);
void drawNextFig(WINDOW* win, int** next);
void drawGame(GameInfo_t* game_info);
void end_ncurses();

#endif