#include "tetris_ui_bridge.h"

#include "tetris_internal.h"

TetrisViewState_t tetrisGetViewState(void) {
  const TetrisGame_t *game = tetrisGetGame();
  tetrisEnsureInitialized();

  if (game->state == kStateGameOver) {
    return kTetrisViewGameOver;
  }
  if (game->paused) {
    return kTetrisViewPaused;
  }
  if (game->state == kStateStart) {
    return kTetrisViewStart;
  }
  return kTetrisViewRunning;
}

void tetrisResetForMenu(void) {
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
}
