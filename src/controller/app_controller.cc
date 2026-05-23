#include "app_controller.h"

namespace s21 {

AppController::AppController()
    : state_(AppState::kMenu),
      selected_menu_index_(0),
      active_game_type_(GameType::kTetris),
      menu_items_({"Tetris", "Snake", "Quit"}) {}

AppState AppController::GetAppState() const { return state_; }

const std::vector<std::string>& AppController::GetMenuItems() const {
  return menu_items_;
}

int AppController::GetSelectedMenuIndex() const { return selected_menu_index_; }

GameType AppController::GetActiveGameType() const { return active_game_type_; }

void AppController::MoveMenuSelection(const int delta) {
  const int item_count = static_cast<int>(menu_items_.size());
  selected_menu_index_ = (selected_menu_index_ + delta + item_count) % item_count;
}

void AppController::StartSelectedGame() {
  if (selected_menu_index_ == 2) {
    Exit();
    return;
  }
  StartGame(selected_menu_index_ == 0 ? GameType::kTetris : GameType::kSnake);
}

void AppController::StartGame(const GameType game) {
  active_game_type_ = game;
  GetActiveGame().Reset();
  state_ = AppState::kPlaying;
}

void AppController::ReturnToMenu() {
  GetActiveGame().Reset();
  state_ = AppState::kMenu;
}

void AppController::Exit() { state_ = AppState::kExiting; }

GameAdapter& AppController::GetActiveGame() {
  return active_game_type_ == GameType::kTetris
             ? static_cast<GameAdapter&>(tetris_)
             : static_cast<GameAdapter&>(snake_);
}

const GameAdapter& AppController::GetActiveGame() const {
  return active_game_type_ == GameType::kTetris
             ? static_cast<const GameAdapter&>(tetris_)
             : static_cast<const GameAdapter&>(snake_);
}

}  // namespace s21
