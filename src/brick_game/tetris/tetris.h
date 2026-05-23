#ifndef SRC_BRICK_GAME_TETRIS_TETRIS_H_
#define SRC_BRICK_GAME_TETRIS_TETRIS_H_

#include "../common/brick_game_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState(void);

#ifdef __cplusplus
}
#endif

#endif  // SRC_BRICK_GAME_TETRIS_TETRIS_H_
