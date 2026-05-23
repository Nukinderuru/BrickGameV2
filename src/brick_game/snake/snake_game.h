#ifndef SRC_BRICK_GAME_SNAKE_SNAKE_GAME_H_
#define SRC_BRICK_GAME_SNAKE_SNAKE_GAME_H_

#include <cstdint>
#include <deque>

#include "../common/brick_game_types.h"
#include "../common/view_status.h"

namespace s21 {

class SnakeGame {
 public:
  struct Position {
    int row;
    int col;

    bool operator==(const Position& other) const {
      return row == other.row && col == other.col;
    }
  };

  enum class State {
    kStart = 0,
    kSpawn,
    kMoving,
    kGameOver,
    kWin,
  };

  enum class Direction {
    kUp = 0,
    kRight,
    kDown,
    kLeft,
  };

  SnakeGame();

  void ResetToMenu();
  void HandleInput(UserAction_t action, bool hold);
  GameInfo_t Update();
  ViewStatus GetViewStatus() const;

  State GetState() const;
  bool IsPaused() const;
  const std::deque<Position>& GetSnake() const;
  Position GetApple() const;
  int GetScore() const;
  int GetHighScore() const;

  void SetCurrentTimeForTests(uint64_t now_ms);
  void ClearCurrentTimeOverrideForTests();
  void SetAppleForTests(Position apple);
  void SetSnakeForTests(const std::deque<Position>& snake, Direction direction);
  void SetStateForTests(State state);

 private:
  static constexpr int kRows = 20;
  static constexpr int kCols = 10;
  static constexpr int kPreviewSize = 4;
  static constexpr int kInitialLength = 4;
  static constexpr int kWinLength = 200;
  static constexpr uint64_t kBaseDelayMs = 250;
  static constexpr uint64_t kBoostDelayMs = 90;

  void ResetBuffers();
  void StartGame();
  void SpawnInitialSnake();
  void SpawnApple();
  void TryQueueDirection(Direction direction);
  static bool IsOpposite(Direction lhs, Direction rhs) ;
  Position NextHeadPosition() const;
  bool IsOccupiedBySnake(Position position, bool ignore_tail) const;
  void Step();
  void SyncInfo();
  uint64_t NowMs() const;
  uint64_t CurrentDelayMs() const;

  int field_storage_[kRows][kCols];
  int next_storage_[kPreviewSize][kPreviewSize];
  int* field_rows_[kRows];
  int* next_rows_[kPreviewSize];

  std::deque<Position> snake_;
  Position apple_;
  State state_;
  Direction direction_;
  Direction pending_direction_;
  bool has_pending_direction_;
  bool paused_;
  bool accelerated_;
  int boost_steps_remaining_;
  uint64_t last_tick_ms_;
  int score_;
  int high_score_;
  GameInfo_t info_;

  bool use_time_override_;
  uint64_t current_time_override_ms_;
};

SnakeGame& GetSnakeGame();

}  // namespace s21

#endif  // SRC_BRICK_GAME_SNAKE_SNAKE_GAME_H_
