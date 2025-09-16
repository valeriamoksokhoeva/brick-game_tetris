#include "backend.h"

GameState* getGameState(void) {
  static GameState game_state = {0};
  return &game_state;
}

void userInput(UserAction_t action, bool hold) {
  (void)hold;
  switch (action) {
    case Start:
      startGame();
      break;
    case Terminate:
      terminateGame();
      break;
    case Left:
      moveLeft();
      break;
    case Right:
      moveRight();
      break;
    case Up:
      break;
    case Down:
      moveDown();
      break;
    case Action:
      rotateFigure();
      break;
    case Pause:
      pauseGame();
      break;
  }
}

GameInfo_t updateCurrentState() {
  GameState* game_state = getGameState();
  updateGameState(game_state);
  return game_state->info;
}

void pauseGame() {
  GameState* game_state = getGameState();
  if (game_state->info.pause == 1 || game_state->info.pause == 0)
    game_state->info.pause = !game_state->info.pause;
}

void initField() {
  GameState* game_state = getGameState();
  if (game_state->info.field) {
    for (int i = 0; i < HEIGHT + 2; i++) {
      free(game_state->info.field[i]);
    }
    free(game_state->info.field);
  }

  game_state->info.field = (int**)malloc((HEIGHT + 2) * sizeof(int*));
  for (int i = 0; i < HEIGHT + 2; i++) {
    game_state->info.field[i] = (int*)malloc((WIDTH + 2) * sizeof(int));
    for (int j = 0; j < WIDTH + 2; j++) {
      if (i == 0 || i == HEIGHT + 1 || j == 0 || j == WIDTH + 1) {
        game_state->info.field[i][j] = 1;
        game_state->game_field[i][j] = 1;
      } else {
        game_state->info.field[i][j] = 0;
        game_state->game_field[i][j] = 0;
      }
    }
  }
}

void initNextFigure() {
  GameState* game_state = getGameState();
  if (game_state->info.next) {
    for (int i = 0; i < 4; i++) {
      free(game_state->info.next[i]);
    }
    free(game_state->info.next);
  }

  game_state->info.next = (int**)malloc(4 * sizeof(int*));
  for (int i = 0; i < 4; i++) {
    game_state->info.next[i] = (int*)malloc(4 * sizeof(int));
  }
}

void initGame() {
  GameState* game_state = getGameState();
  initField();
  initNextFigure();
  game_state->info.pause = 2;
  game_state->status = START;
}

void startGame() {
  GameState* game_state = getGameState();
  if (game_state->status == GAME_OVER) {
    terminateGame();
    initGame();
  }
  if (game_state->status == START) {
    FILE* file = fopen("score.txt", "r");
    if (file) {
      if (fscanf(file, "%d", &game_state->info.high_score) < 0) {
        game_state->info.high_score = 0;
      }
      fclose(file);
    }
    game_state->info.score = 0;
    game_state->info.level = 1;
    game_state->info.speed = 1;
    game_state->info.pause = 0;
    game_state->current_fig = 8;
    game_state->next_fig = 8;
    game_state->rotation = 0;
    game_state->score = 0;
    game_state->status = SPAWN;
  }
}

void checkScore() {
  GameState* game_state = getGameState();
  if (game_state->info.score > game_state->info.high_score) {
    game_state->info.high_score = game_state->info.score;
    FILE* file = fopen("score.txt", "w");
    if (file) {
      fprintf(file, "%d", game_state->info.score);
      fclose(file);
    }
  }
}

void updateScoreandLevel(int lines) {
  GameState* game_state = getGameState();
  int points = calculatePoints(lines);
  game_state->score += points;
  game_state->info.score += points;
  if (game_state->score >= 600 && game_state->info.level < 10) {
    game_state->score -= 600;
    game_state->info.level += 1;
    game_state->info.speed += 1;
  }
  checkScore();
}

int calculatePoints(int n) {
  int points = 0;
  if (n == 1) {
    points = 100;
  } else if (n == 2) {
    points = 300;
  } else if (n == 3) {
    points = 700;
  } else if (n == 4) {
    points = 1500;
  }
  return points;
}

void generateNextFig() {
  GameState* game_state = getGameState();
  if (game_state->next_fig == 8) {
    game_state->current_fig = rand() % 7;
  } else {
    game_state->current_fig = game_state->next_fig;
  }
  game_state->rotation = 0;
  game_state->x = WIDTH / 2;
  game_state->y = 0;
  game_state->next_fig = rand() % 7;
  if (checkCollisions(game_state->current_fig, game_state->rotation,
                      game_state->x, game_state->y)) {
    game_state->info.pause = 3;
    game_state->status = GAME_OVER;
  } else {
    game_state->status = MOVING;
  }
}

int getFigure(int figure, int rotation, int x, int y) {
  int blocks[7][4][4][4] = {
      // O
      {{{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
       {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
       {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
       {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}},
      // I
      {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}},
       {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
       {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}},
      // L
      {{{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}},
       {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
       {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}},
      // J
      {{{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}},
       {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
       {{0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 1, 1}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}},
      // Z
      {{{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},
       {{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}},
       {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
       {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}},
      // S
      {{{0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}},
       {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}},
       {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
       {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}},
      // T
      {{{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},
       {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
       {{0, 0, 1, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}}}};
  return blocks[figure][rotation][x][y];
}

/* копирование фигур в некст и филд перед передачей во фронт */
void copyFigToField(GameState* game_state) {
  for (int i = 0; i < HEIGHT + 2; i++) {
    for (int j = 0; j < WIDTH + 2; j++) {
      game_state->info.field[i][j] = game_state->game_field[i][j];
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getFigure(game_state->current_fig, game_state->rotation, i, j))
        game_state->info.field[i + game_state->y][j + game_state->x] = 1;
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      game_state->info.next[i][j] = getFigure(game_state->next_fig, 0, i, j);
    }
  }
}

int checkCollisions(int figure, int rotation, int x, int y) {
  const GameState* game_state = getGameState();
  int collision = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getFigure(figure, rotation, i, j) &&
          game_state->game_field[i + y][j + x]) {
        collision = 1;
      }
    }
  }
  return collision;  // 1 = есть переесечение, 0 = нет
}

void rotateFigure() {
  GameState* game_state = getGameState();
  if (!checkCollisions(game_state->current_fig, (game_state->rotation + 1) % 4,
                       game_state->x, game_state->y)) {
    game_state->rotation = (game_state->rotation + 1) % 4;
  }
}

int moveDown() {
  GameState* game_state = getGameState();
  int can_move = 1;
  if (!checkCollisions(game_state->current_fig, game_state->rotation,
                       game_state->x, game_state->y + 1)) {
    game_state->y++;
  } else {
    can_move = 0;
  }
  return can_move;  // 1 can move, 0 can't
}

void moveRight() {
  GameState* game_state = getGameState();
  if (!checkCollisions(game_state->current_fig, game_state->rotation,
                       game_state->x + 1, game_state->y)) {
    game_state->x++;
  }
}

void moveLeft() {
  GameState* game_state = getGameState();
  if (!checkCollisions(game_state->current_fig, game_state->rotation,
                       game_state->x - 1, game_state->y)) {
    game_state->x--;
  }
}

void terminateGame() {
  GameState* game_state = getGameState();
  if (game_state->info.field != NULL) {
    for (int i = 0; i < HEIGHT + 2; i++) {
      free(game_state->info.field[i]);
    }
    free(game_state->info.field);
    game_state->info.field = NULL;
  }

  if (game_state->info.next != NULL) {
    for (int i = 0; i < 4; i++) {
      free(game_state->info.next[i]);
    }
    free(game_state->info.next);
    game_state->info.next = NULL;
  }

  checkScore();
}

/* сохраняем упавшую фигурку в game_field */
void stayDown() {
  GameState* game_state = getGameState();
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (getFigure(game_state->current_fig, game_state->rotation, i, j))
        game_state->game_field[i + game_state->y][j + game_state->x] = 1;
    }
  }
  checkLines();
  game_state->status = SPAWN;
}

void checkLines() {
  const GameState* game_state = getGameState();
  int sum_lines = 0;
  for (int i = 1; i < HEIGHT + 1; i++) {
    int full = 1;
    for (int j = 1; j < WIDTH + 1; j++) {
      if (!game_state->game_field[i][j])
        full = 0;  // если есть хотя бы одна 0, то строка не заполнена
    }
    if (full) {
      lineCleanShift(i);
      sum_lines++;
    }
  }
  if (sum_lines > 0) {
    updateScoreandLevel(sum_lines);
  }
}

void lineCleanShift(int line) {
  GameState* game_state = getGameState();
  for (int i = line; i > 1; i--) {
    for (int j = 0; j < WIDTH + 2; j++) {
      game_state->game_field[i][j] = game_state->game_field[i - 1][j];
    }
  }
  for (int j = 1; j < WIDTH + 1; j++) {
    game_state->game_field[1][j] = 0;
  }
}

void waitTimer(int n) {
  clock_t start = clock();
  double fulltime = 0.0025 * n;
  while ((double)(clock() - start) / CLOCKS_PER_SEC < fulltime);
}

void run_game(GameState* game_state) {
  static int game_timer = 0;
  waitTimer(27 - 2 * game_state->info.speed);
  game_timer++;

  if (game_state->status == MOVING) {
    if (game_timer >= 27 - 2 * game_state->info.speed) {
      game_timer = 0;
      int can_move = moveDown();
      if (!can_move) {
        game_state->status = ATTACHING;
      }
    }
  } else if (game_state->status == ATTACHING) {
    stayDown();
  } else if (game_state->status == SPAWN) {
    generateNextFig();
  }
}

void updateGameState(GameState* game_state) {
  if (game_state->info.field == NULL) {
    initGame();
  } else {
    run_game(game_state);
    copyFigToField(game_state);
  }
}