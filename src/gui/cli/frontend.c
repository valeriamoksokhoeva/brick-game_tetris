#include "frontend.h"

int get_inputs(UserAction_t* action) {
  int ch = getch();
  if (ch == '\n') {
    *action = Start;
  } else if (ch == KEY_LEFT) {
    *action = Left;
  } else if (ch == KEY_RIGHT) {
    *action = Right;
  } else if (ch == KEY_UP) {
    *action = Up;
  } else if (ch == KEY_DOWN) {
    *action = Down;
  } else if (ch == ' ') {
    *action = Action;
  } else if (ch == 'p' || ch == 'P') {
    *action = Pause;
  } else if (ch == 'q' || ch == 'Q') {
    *action = Terminate;
  }

  return (ch == ERR ? 0 : 1);
}

void start_ncurses() {
  initscr();
  curs_set(0);
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
}

WINDOW* winMainField() {
  static WINDOW* win = NULL;
  if (!win) {
    win = newwin(HEIGHT + 2, WIDTH + 2, 0, 0);
  }
  return win;
}

WINDOW* winNextFigure() {
  static WINDOW* win = NULL;
  if (!win) {
    win = newwin(7, 14, 12, 15);
  }
  return win;
}

WINDOW* winButtons() {
  static WINDOW* win = NULL;
  if (!win) {
    win = newwin(10, 21, 0, 15);
  }
  return win;
}

WINDOW* winScoreSpeed() {
  static WINDOW* win = NULL;
  if (!win) {
    win = newwin(10, 21, 12, 40);
  }
  return win;
}

void drawScoreSpeed(WINDOW* win, int score, int speed, int high_score,
                    int level) {
  mvwprintw(win, 1, 1, "SCORE:");
  mvwprintw(win, 2, 1, "%d", score);
  mvwprintw(win, 3, 1, "SPEED:");
  mvwprintw(win, 4, 1, "%d", speed);
  mvwprintw(win, 5, 1, "HIGH SCORE:");
  mvwprintw(win, 6, 1, "%d", high_score);
  mvwprintw(win, 7, 1, "LEVEL:");
  mvwprintw(win, 8, 1, "%d", level);
}

void drawButtons(WINDOW* buttons) {
  mvwprintw(buttons, 1, 1, "Start = ENTER");
  mvwprintw(buttons, 2, 1, "Pause = 'p'");
  mvwprintw(buttons, 3, 1, "Terminate = 'q'");
  mvwprintw(buttons, 4, 1, "Left = left arrow");
  mvwprintw(buttons, 5, 1, "Right = right arrow");
  mvwprintw(buttons, 6, 1, "Down = down arrow");
  mvwprintw(buttons, 7, 1, "Up = up arrow");
  mvwprintw(buttons, 8, 1, "Action = SPACE");
}

void drawField(WINDOW* win, int** field) {
  for (int i = 1; i < WIDTH + 1; i++) {
    for (int j = 1; j < HEIGHT + 1; j++) {
      wmove(win, j, i);
      if (field[j][i]) {
        wprintw(win, "*");
      } else {
        wprintw(win, ".");
      }
    }
  }
}

void drawNextFig(WINDOW* win, int** next) {
  mvwprintw(win, 1, 1, "Next figure:");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      wmove(win, i + 2, j + 2);
      if (next[i][j]) {
        wprintw(win, "*");
      } else {
        wprintw(win, " ");
      }
    }
  }
}

void clearWin(WINDOW* win, int h, int w) {
  for (int i = 1; i < h; i++) {
    for (int j = 1; j < w; j++) {
      wmove(win, i, j);
      wprintw(win, " ");
    }
  }
}

void drawPause(WINDOW* winField, WINDOW* winNext, int pause_status) {
  clearWin(winField, HEIGHT + 1, WIDTH + 1);
  clearWin(winNext, 6, 13);
  if (pause_status == 1) {
    mvwprintw(winField, 1, 1, "Pause");
    mvwprintw(winField, 2, 1, "Press 'p'");
    mvwprintw(winField, 3, 1, "to");
    mvwprintw(winField, 4, 1, "continue");
  } else if (pause_status == 2) {
    mvwprintw(winField, 1, 1, "START");
    mvwprintw(winField, 2, 1, "Press");
    mvwprintw(winField, 3, 1, "'Enter'");
    mvwprintw(winField, 4, 1, "To start");
  } else if (pause_status == 3) {
    mvwprintw(winField, 1, 1, "Game Over");
    mvwprintw(winField, 2, 1, "Press");
    mvwprintw(winField, 3, 1, "'Enter");
    mvwprintw(winField, 4, 1, "To play");
    mvwprintw(winField, 5, 1, "Again");
  }
}

void drawGame(GameInfo_t* game_info) {
  WINDOW* fieldWin = winMainField();
  WINDOW* nextFigWin = winNextFigure();
  WINDOW* buttonsWin = winButtons();
  WINDOW* scorespeedWin = winScoreSpeed();
  refresh();
  box(fieldWin, 0, 0);
  box(nextFigWin, 0, 0);
  box(buttonsWin, 0, 0);
  box(scorespeedWin, 0, 0);
  drawButtons(buttonsWin);
  if (!game_info->pause) {
    drawField(fieldWin, game_info->field);
    drawNextFig(nextFigWin, game_info->next);
    drawScoreSpeed(scorespeedWin, game_info->score, game_info->speed,
                   game_info->high_score, game_info->level);
  } else {
    drawPause(fieldWin, nextFigWin, game_info->pause);
  }
  wrefresh(scorespeedWin);
  wrefresh(fieldWin);
  wrefresh(nextFigWin);
  wrefresh(buttonsWin);
}

void end_ncurses() {
  WINDOW* fieldWin = winMainField();
  WINDOW* nextFigWin = winNextFigure();
  WINDOW* buttonsWin = winButtons();
  WINDOW* scorespeedWin = winScoreSpeed();
  if (fieldWin) {
    delwin(fieldWin);
    fieldWin = NULL;
  }
  if (nextFigWin) {
    delwin(nextFigWin);
    nextFigWin = NULL;
  }
  if (buttonsWin) {
    delwin(buttonsWin);
    buttonsWin = NULL;
  }
  if (scorespeedWin) {
    delwin(scorespeedWin);
    scorespeedWin = NULL;
  }
  curs_set(1);
  echo();
  endwin();
}