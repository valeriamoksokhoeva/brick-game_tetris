#include "../../brick_game/tetris/backend.h"
#include "frontend.h"

int main() {
  srand((unsigned char)time(NULL));
  (void)rand();

  start_ncurses();
  UserAction_t action = Pause;
  bool hold = false;
  GameInfo_t game_info;
  while (action != Terminate) {
    game_info = updateCurrentState();
    drawGame(&game_info);
    if (get_inputs(&action))
      userInput(action, hold);  // если был ввод с клавы, то пускаем userInput
  }
  terminateGame();
  end_ncurses();
  return 0;
}