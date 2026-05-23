#ifndef SRC_BRICK_GAME_SNAKE_SNAKE_H_
#define SRC_BRICK_GAME_SNAKE_SNAKE_H_

#include "../common/brick_game_types.h"

namespace s21 {

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();
void resetForMenu();

}  // namespace s21

#endif  // SRC_BRICK_GAME_SNAKE_SNAKE_H_
