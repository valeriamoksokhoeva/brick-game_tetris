#include <check.h>

#include "../brick_game/tetris/backend.h"

void setup_test_environment() {
  initGame();
  startGame();
}

void teardown_test_environment() { terminateGame(); }

START_TEST(test_getGameState_singleton) {
  GameState* state1 = getGameState();
  GameState* state2 = getGameState();
  ck_assert_ptr_eq(state1, state2);
}
END_TEST

START_TEST(test_initField_allocations) {
  GameState* state = getGameState();
  initField();

  ck_assert_ptr_nonnull(state->info.field);
  for (int i = 0; i < HEIGHT + 2; i++) {
    ck_assert_ptr_nonnull(state->info.field[i]);
  }

  for (int i = 0; i < HEIGHT + 2; i++) {
    for (int j = 0; j < WIDTH + 2; j++) {
      if (i == 0 || i == HEIGHT + 1 || j == 0 || j == WIDTH + 1) {
        ck_assert_int_eq(state->info.field[i][j], 1);
      } else {
        ck_assert_int_eq(state->info.field[i][j], 0);
      }
    }
  }
}
END_TEST

START_TEST(test_initNextFigure_allocations) {
  GameState* state = getGameState();
  initNextFigure();

  ck_assert_ptr_nonnull(state->info.next);
  for (int i = 0; i < 4; i++) {
    ck_assert_ptr_nonnull(state->info.next[i]);
  }
}
END_TEST

START_TEST(test_calculatePoints_all_cases) {
  ck_assert_int_eq(calculatePoints(0), 0);
  ck_assert_int_eq(calculatePoints(1), 100);
  ck_assert_int_eq(calculatePoints(2), 300);
  ck_assert_int_eq(calculatePoints(3), 700);
  ck_assert_int_eq(calculatePoints(4), 1500);
  ck_assert_int_eq(calculatePoints(0), 0);
}
END_TEST

START_TEST(test_getFigure_all_shapes) {
  for (int fig = 0; fig < 7; fig++) {
    int has_block = 0;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (getFigure(fig, 0, i, j) == 1) {
          has_block = 1;
        }
      }
    }
    ck_assert_int_eq(has_block, 1);
  }

  // Конкретные тесты для фигуры O
  ck_assert_int_eq(getFigure(0, 0, 1, 1), 1);
  ck_assert_int_eq(getFigure(0, 0, 1, 2), 1);
  ck_assert_int_eq(getFigure(0, 0, 2, 1), 1);
  ck_assert_int_eq(getFigure(0, 0, 2, 2), 1);
}
END_TEST

START_TEST(test_checkCollisions_no_collision) {
  setup_test_environment();

  int collision = checkCollisions(0, 0, WIDTH / 2, 1);
  ck_assert_int_eq(collision, 0);

  teardown_test_environment();
}
END_TEST

START_TEST(test_checkCollisions_wall_collision) {
  setup_test_environment();

  int collision = checkCollisions(0, 0, -1, 1);
  ck_assert_int_eq(collision, 1);

  collision = checkCollisions(0, 0, WIDTH, 1);
  ck_assert_int_eq(collision, 1);

  teardown_test_environment();
}
END_TEST

START_TEST(test_move_functions_basic) {
  setup_test_environment();
  GameState* state = getGameState();

  state->current_fig = 0;
  state->rotation = 0;
  state->x = WIDTH / 2;
  state->y = 5;

  int initial_x = state->x;
  int initial_y = state->y;

  moveRight();
  ck_assert_int_eq(state->x, initial_x + 1);

  moveLeft();
  ck_assert_int_eq(state->x, initial_x);

  int can_move = moveDown();
  ck_assert_int_eq(can_move, 1);
  ck_assert_int_eq(state->y, initial_y + 1);

  teardown_test_environment();
}
END_TEST

START_TEST(test_rotateFigure_safe_position) {
  setup_test_environment();
  GameState* state = getGameState();

  state->current_fig = 0;
  state->rotation = 0;
  state->x = WIDTH / 2;
  state->y = 5;

  int initial_rotation = state->rotation;

  rotateFigure();
  ck_assert_int_eq(state->rotation, (initial_rotation + 1) % 4);

  teardown_test_environment();
}
END_TEST

START_TEST(test_pauseGame_toggle) {
  setup_test_environment();
  GameState* state = getGameState();

  state->info.pause = 0;
  pauseGame();
  ck_assert_int_eq(state->info.pause, 1);

  pauseGame();
  ck_assert_int_eq(state->info.pause, 0);

  teardown_test_environment();
}
END_TEST

START_TEST(test_updateScoreandLevel_basic) {
  setup_test_environment();
  GameState* state = getGameState();

  int initial_score = state->info.score;
  int initial_level = state->info.level;

  updateScoreandLevel(2);

  ck_assert_int_eq(state->info.score, initial_score + 300);
  ck_assert_int_eq(state->info.level, initial_level);

  teardown_test_environment();
}
END_TEST

START_TEST(test_updateScoreandLevel_level_up) {
  setup_test_environment();
  GameState* state = getGameState();

  state->score = 600;
  state->info.level = 1;

  updateScoreandLevel(1);

  ck_assert_int_eq(state->info.level, 2);
  ck_assert_int_eq(state->score, 100);

  teardown_test_environment();
}
END_TEST

START_TEST(test_lineCleanShift_basic) {
  setup_test_environment();
  GameState* state = getGameState();

  int test_line = 10;
  for (int j = 1; j <= WIDTH; j++) {
    state->game_field[test_line][j] = 1;
  }

  for (int j = 1; j <= WIDTH; j++) {
    state->game_field[test_line - 1][j] = j % 2;
  }

  lineCleanShift(test_line);

  for (int j = 1; j <= WIDTH; j++) {
    ck_assert_int_eq(state->game_field[test_line][j], j % 2);
  }

  for (int j = 1; j <= WIDTH; j++) {
    ck_assert_int_eq(state->game_field[1][j], 0);
  }

  teardown_test_environment();
}
END_TEST

START_TEST(test_checkLines_no_lines) {
  setup_test_environment();
  GameState* state = getGameState();

  int initial_score = state->info.score;
  checkLines();

  ck_assert_int_eq(state->info.score, initial_score);

  teardown_test_environment();
}
END_TEST

START_TEST(test_checkLines_with_lines) {
  setup_test_environment();
  GameState* state = getGameState();

  int test_line = 15;
  for (int j = 1; j <= WIDTH; j++) {
    state->game_field[test_line][j] = 1;
  }

  int initial_score = state->info.score;
  checkLines();

  ck_assert_int_eq(state->info.score, initial_score + 100);

  teardown_test_environment();
}
END_TEST

START_TEST(test_generateNextFig_first_generation) {
  setup_test_environment();
  GameState* state = getGameState();

  state->next_fig = 8;
  generateNextFig();

  ck_assert_int_ge(state->current_fig, 0);
  ck_assert_int_le(state->current_fig, 6);
  ck_assert_int_ge(state->next_fig, 0);
  ck_assert_int_le(state->next_fig, 6);
  ck_assert_int_eq(state->status, MOVING);

  teardown_test_environment();
}
END_TEST

START_TEST(test_generateNextFig_game_over) {
  setup_test_environment();
  GameState* state = getGameState();

  for (int j = 3; j <= 6; j++) {
    state->game_field[1][j] = 1;
  }

  state->next_fig = 0;
  generateNextFig();

  ck_assert_int_eq(state->info.pause, 3);
  ck_assert_int_eq(state->status, GAME_OVER);

  teardown_test_environment();
}
END_TEST

START_TEST(test_userInput_all_actions) {
  setup_test_environment();

  userInput(Start, false);
  userInput(Left, false);
  userInput(Right, false);
  userInput(Down, false);
  userInput(Action, false);
  userInput(Pause, false);
  userInput(Terminate, false);

  ck_assert(1);

  teardown_test_environment();
}
END_TEST

START_TEST(test_stayDown_functionality) {
  setup_test_environment();
  GameState* state = getGameState();

  state->current_fig = 0;
  state->rotation = 0;
  state->x = WIDTH / 2;
  state->y = 5;

  stayDown();

  ck_assert_int_eq(state->status, SPAWN);

  teardown_test_environment();
}
END_TEST

START_TEST(test_updateCurrentState_basic) {
  setup_test_environment();

  GameInfo_t info = updateCurrentState();

  ck_assert_ptr_nonnull(info.field);
  ck_assert_ptr_nonnull(info.next);

  teardown_test_environment();
}
END_TEST

START_TEST(test_terminateGame_cleanup) {
  setup_test_environment();
  GameState* state = getGameState();

  terminateGame();

  ck_assert_ptr_null(state->info.field);
  ck_assert_ptr_null(state->info.next);

  teardown_test_environment();
}
END_TEST

Suite* backend_suite(void) {
  Suite* s = suite_create("Backend");

  TCase* tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_getGameState_singleton);
  tcase_add_test(tc_core, test_initField_allocations);
  tcase_add_test(tc_core, test_initNextFigure_allocations);
  tcase_add_test(tc_core, test_calculatePoints_all_cases);
  tcase_add_test(tc_core, test_getFigure_all_shapes);

  TCase* tc_collision = tcase_create("Collision");
  tcase_add_test(tc_collision, test_checkCollisions_no_collision);
  tcase_add_test(tc_collision, test_checkCollisions_wall_collision);

  TCase* tc_movement = tcase_create("Movement");
  tcase_add_test(tc_movement, test_move_functions_basic);
  tcase_add_test(tc_movement, test_rotateFigure_safe_position);

  TCase* tc_game_logic = tcase_create("Game Logic");
  tcase_add_test(tc_game_logic, test_pauseGame_toggle);
  tcase_add_test(tc_game_logic, test_updateScoreandLevel_basic);
  tcase_add_test(tc_game_logic, test_updateScoreandLevel_level_up);
  tcase_add_test(tc_game_logic, test_lineCleanShift_basic);
  tcase_add_test(tc_game_logic, test_checkLines_no_lines);
  tcase_add_test(tc_game_logic, test_checkLines_with_lines);
  tcase_add_test(tc_game_logic, test_generateNextFig_first_generation);
  tcase_add_test(tc_game_logic, test_generateNextFig_game_over);

  TCase* tc_integration = tcase_create("Integration");
  tcase_add_test(tc_integration, test_userInput_all_actions);
  tcase_add_test(tc_integration, test_stayDown_functionality);
  tcase_add_test(tc_integration, test_updateCurrentState_basic);
  tcase_add_test(tc_integration, test_terminateGame_cleanup);

  suite_add_tcase(s, tc_core);
  suite_add_tcase(s, tc_collision);
  suite_add_tcase(s, tc_movement);
  suite_add_tcase(s, tc_game_logic);
  suite_add_tcase(s, tc_integration);

  return s;
}

int main(void) {
  int number_failed;
  Suite* s = backend_suite();
  SRunner* sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}