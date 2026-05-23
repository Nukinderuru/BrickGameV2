#include "snake.h"

#include "snake_game.h"

namespace s21 {

void userInput(const UserAction_t action, const bool hold) {
  GetSnakeGame().HandleInput(action, hold);
}

GameInfo_t updateCurrentState() { return GetSnakeGame().Update(); }

void resetForMenu() { GetSnakeGame().ResetToMenu(); }

}  // namespace s21
