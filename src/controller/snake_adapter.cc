#include "snake_adapter.h"

#include "../brick_game/snake/snake.h"
#include "../brick_game/snake/snake_game.h"

namespace s21 {

std::string SnakeAdapter::GetTitle() const { return "BrickGame: Snake"; }

std::vector<std::string> SnakeAdapter::GetControls() const {
  return {"Enter: start/restart", "P: pause", "Q/Esc: menu",
          "Arrows: turn", "Space: speed up"};
}

bool SnakeAdapter::HasPreview() const { return false; }

bool SnakeAdapter::UsesActionHold() const { return true; }

void SnakeAdapter::Reset() { resetForMenu(); }

void SnakeAdapter::HandleInput(const UserAction_t action, const bool hold) {
  userInput(action, hold);
}

GameInfo_t SnakeAdapter::Update() { return updateCurrentState(); }

ViewStatus SnakeAdapter::GetViewStatus() const { return GetSnakeGame().GetViewStatus(); }

}  // namespace s21
