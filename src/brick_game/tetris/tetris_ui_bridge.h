#ifndef SRC_BRICK_GAME_TETRIS_TETRIS_UI_BRIDGE_H_
#define SRC_BRICK_GAME_TETRIS_TETRIS_UI_BRIDGE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  kTetrisViewStart = 0,
  kTetrisViewRunning,
  kTetrisViewPaused,
  kTetrisViewGameOver
} TetrisViewState_t;

TetrisViewState_t tetrisGetViewState(void);
void tetrisResetForMenu(void);

#ifdef __cplusplus
}
#endif

#endif  // SRC_BRICK_GAME_TETRIS_TETRIS_UI_BRIDGE_H_
