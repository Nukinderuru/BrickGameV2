#include "tetris_adapter.h"

extern "C" {
#include "../brick_game/tetris/tetris.h"
#include "../brick_game/tetris/tetris_ui_bridge.h"
}

namespace s21 {

std::string TetrisAdapter::GetTitle() const { return "BrickGame: Tetris"; }

std::vector<std::string> TetrisAdapter::GetControls() const {
  return {"Enter: start/restart", "P: pause", "Q/Esc: menu",
          "Left/Right: move", "Down: soft drop", "Space: rotate"};
}

bool TetrisAdapter::HasPreview() const { return true; }

bool TetrisAdapter::UsesActionHold() const { return false; }

void TetrisAdapter::Reset() { tetrisResetForMenu(); }

void TetrisAdapter::HandleInput(const UserAction_t action, const bool hold) {
  userInput(action, hold);
}

GameInfo_t TetrisAdapter::Update() { return updateCurrentState(); }

ViewStatus TetrisAdapter::GetViewStatus() const {
  switch (tetrisGetViewState()) {
    case kTetrisViewStart:
      return ViewStatus::kStartScreen;
    case kTetrisViewPaused:
      return ViewStatus::kPaused;
    case kTetrisViewGameOver:
      return ViewStatus::kGameOver;
    case kTetrisViewRunning:
    default:
      return ViewStatus::kRunning;
  }
}

}  // namespace s21
