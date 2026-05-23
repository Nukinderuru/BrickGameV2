#ifndef SRC_CONTROLLER_APP_CONTROLLER_H_
#define SRC_CONTROLLER_APP_CONTROLLER_H_

#include <memory>
#include <string>
#include <vector>

#include "game_adapter.h"
#include "snake_adapter.h"
#include "tetris_adapter.h"

namespace s21 {

enum class AppState {
  kMenu = 0,
  kPlaying,
  kExiting,
};

enum class GameType {
  kTetris = 0,
  kSnake,
};

class AppController {
 public:
  AppController();

  AppState GetAppState() const;
  const std::vector<std::string>& GetMenuItems() const;
  int GetSelectedMenuIndex() const;
  GameType GetActiveGameType() const;

  void MoveMenuSelection(int delta);
  void StartSelectedGame();
  void StartGame(GameType game);
  void ReturnToMenu();
  void Exit();

  GameAdapter& GetActiveGame();
  const GameAdapter& GetActiveGame() const;

 private:
  AppState state_;
  int selected_menu_index_;
  GameType active_game_type_;
  std::vector<std::string> menu_items_;
  TetrisAdapter tetris_;
  SnakeAdapter snake_;
};

}  // namespace s21

#endif  // SRC_CONTROLLER_APP_CONTROLLER_H_
