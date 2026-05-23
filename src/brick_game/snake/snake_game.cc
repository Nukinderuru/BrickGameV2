#include "snake_game.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>

#include "snake_storage.h"

namespace s21 {

namespace {

SnakeGame::Direction DirectionFromAction(const UserAction_t action) {
  switch (action) {
    case Up:
      return SnakeGame::Direction::kUp;
    case Right:
      return SnakeGame::Direction::kRight;
    case Down:
      return SnakeGame::Direction::kDown;
    case Left:
    default:
      return SnakeGame::Direction::kLeft;
  }
}

}  // namespace

SnakeGame::SnakeGame()
    : apple_{0, 0},
      state_(State::kStart),
      direction_(Direction::kRight),
      pending_direction_(Direction::kRight),
      has_pending_direction_(false),
      paused_(false),
      accelerated_(false),
      boost_steps_remaining_(0),
      last_tick_ms_(0),
      score_(0),
      high_score_(0),
      info_{},
      use_time_override_(false),
      current_time_override_ms_(0) {
  for (int row = 0; row < kRows; ++row) {
    field_rows_[row] = field_storage_[row];
  }
  for (int row = 0; row < kPreviewSize; ++row) {
    next_rows_[row] = next_storage_[row];
  }
  info_.field = field_rows_;
  info_.next = next_rows_;
  ResetToMenu();
}

void SnakeGame::ResetBuffers() {
  for (int row = 0; row < kRows; ++row) {
    for (int col = 0; col < kCols; ++col) {
      field_storage_[row][col] = 0;
    }
  }
  for (int row = 0; row < kPreviewSize; ++row) {
    for (int col = 0; col < kPreviewSize; ++col) {
      next_storage_[row][col] = 0;
    }
  }
}

void SnakeGame::ResetToMenu() {
  snake_.clear();
  ResetBuffers();
  apple_ = {0, 0};
  state_ = State::kStart;
  direction_ = Direction::kRight;
  pending_direction_ = Direction::kRight;
  has_pending_direction_ = false;
  paused_ = false;
  accelerated_ = false;
  boost_steps_remaining_ = 0;
  last_tick_ms_ = NowMs();
  score_ = 0;
  high_score_ = LoadSnakeHighScore();
  info_.level = 1;
  info_.speed = 1;
  info_.pause = 0;
  SyncInfo();
}

void SnakeGame::StartGame() {
  ResetBuffers();
  snake_.clear();
  state_ = State::kSpawn;
  direction_ = Direction::kRight;
  pending_direction_ = Direction::kRight;
  has_pending_direction_ = false;
  paused_ = false;
  accelerated_ = false;
  boost_steps_remaining_ = 0;
  last_tick_ms_ = NowMs();
  score_ = 0;
  SyncInfo();
}

void SnakeGame::SpawnInitialSnake() {
  snake_.clear();
  constexpr int row = kRows / 2;
  const int start_col = 2;
  for (int offset = 0; offset < kInitialLength; ++offset) {
    snake_.push_back({row, start_col + offset});
  }
  SpawnApple();
  state_ = State::kMoving;
  last_tick_ms_ = NowMs();
}

void SnakeGame::SpawnApple() {
  std::vector<Position> free_cells;
  free_cells.reserve(kRows * kCols - static_cast<int>(snake_.size()));
  for (int row = 0; row < kRows; ++row) {
    for (int col = 0; col < kCols; ++col) {
      const Position cell{row, col};
      if (!IsOccupiedBySnake(cell, false)) {
        free_cells.push_back(cell);
      }
    }
  }
  if (free_cells.empty()) {
    apple_ = {-1, -1};
    return;
  }

  static std::mt19937 generator(std::random_device{}());
  std::uniform_int_distribution<size_t> distribution(0, free_cells.size() - 1);
  apple_ = free_cells[distribution(generator)];
}

void SnakeGame::TryQueueDirection(const Direction direction) {
  const Direction base_direction = has_pending_direction_ ? pending_direction_ : direction_;
  if (direction == base_direction || IsOpposite(base_direction, direction)) {
    return;
  }
  pending_direction_ = direction;
  has_pending_direction_ = true;
}

bool SnakeGame::IsOpposite(const Direction lhs, const Direction rhs) {
  return (lhs == Direction::kUp && rhs == Direction::kDown) ||
         (lhs == Direction::kDown && rhs == Direction::kUp) ||
         (lhs == Direction::kLeft && rhs == Direction::kRight) ||
         (lhs == Direction::kRight && rhs == Direction::kLeft);
}

SnakeGame::Position SnakeGame::NextHeadPosition() const {
  Position next = snake_.back();
  const Direction direction = has_pending_direction_ ? pending_direction_ : direction_;
  switch (direction) {
    case Direction::kUp:
      --next.row;
      break;
    case Direction::kRight:
      ++next.col;
      break;
    case Direction::kDown:
      ++next.row;
      break;
    case Direction::kLeft:
      --next.col;
      break;
  }
  return next;
}

bool SnakeGame::IsOccupiedBySnake(const Position position,
                                  const bool ignore_tail) const {
  for (size_t index = 0; index < snake_.size(); ++index) {
    if (ignore_tail && index == 0) {
      continue;
    }
    if (snake_[index] == position) {
      return true;
    }
  }
  return false;
}

void SnakeGame::Step() {
  if (has_pending_direction_) {
    direction_ = pending_direction_;
    has_pending_direction_ = false;
  }

  const Position next_head = NextHeadPosition();
  if (next_head.row < 0 || next_head.row >= kRows || next_head.col < 0 ||
      next_head.col >= kCols) {
    state_ = State::kGameOver;
    return;
  }

  const bool grows = next_head == apple_;
  if (IsOccupiedBySnake(next_head, !grows)) {
    state_ = State::kGameOver;
    return;
  }

  snake_.push_back(next_head);
  if (!grows) {
    snake_.pop_front();
  }

  if (static_cast<int>(snake_.size()) >= kWinLength) {
    state_ = State::kWin;
    return;
  }

  if (grows) {
    ++score_;
    if (score_ > high_score_) {
      high_score_ = score_;
      SaveSnakeHighScore(high_score_);
    }
    SpawnApple();
  }

  if (boost_steps_remaining_ > 0) {
    --boost_steps_remaining_;
  }
}

void SnakeGame::SyncInfo() {
  ResetBuffers();
  if (apple_.row >= 0 && apple_.row < kRows && apple_.col >= 0 && apple_.col < kCols &&
      state_ != State::kStart) {
    field_storage_[apple_.row][apple_.col] = 2;
  }
  for (size_t index = 0; index < snake_.size(); ++index) {
    const Position& cell = snake_[index];
    field_storage_[cell.row][cell.col] = (index + 1 == snake_.size()) ? 3 : 1;
  }
  info_.score = score_;
  info_.high_score = high_score_;
  info_.level = 1;
  info_.speed = CurrentDelayMs() == kBoostDelayMs ? 2 : 1;
  info_.pause = paused_ ? 1 : 0;
}

void SnakeGame::HandleInput(const UserAction_t action, const bool hold) {
  if (action == Start) {
    if (state_ == State::kStart || state_ == State::kGameOver || state_ == State::kWin) {
      StartGame();
      SyncInfo();
    }
    return;
  }

  if (action == Terminate) {
    state_ = State::kGameOver;
    paused_ = false;
    SyncInfo();
    return;
  }

  if (state_ != State::kMoving) {
    return;
  }

  if (action == Pause) {
    paused_ = !paused_;
    SyncInfo();
    return;
  }

  if (paused_) {
    return;
  }

  if (action == Action) {
    accelerated_ = hold;
    if (!hold) {
      return;
    }
    boost_steps_remaining_ = std::max(boost_steps_remaining_, 1);
    SyncInfo();
    return;
  }

  if (action == Up || action == Right || action == Down || action == Left) {
    TryQueueDirection(DirectionFromAction(action));
  }
}

GameInfo_t SnakeGame::Update() {
  if (state_ == State::kSpawn) {
    SpawnInitialSnake();
  }

  if (state_ == State::kMoving && !paused_) {
    const uint64_t now = NowMs();
    if (now - last_tick_ms_ >= CurrentDelayMs()) {
      last_tick_ms_ = now;
      Step();
      if (!accelerated_) {
        boost_steps_remaining_ = 0;
      }
    }
  }

  SyncInfo();
  return info_;
}

ViewStatus SnakeGame::GetViewStatus() const {
  if (state_ == State::kStart) {
    return ViewStatus::kStartScreen;
  }
  if (state_ == State::kWin) {
    return ViewStatus::kWin;
  }
  if (state_ == State::kGameOver) {
    return ViewStatus::kGameOver;
  }
  if (paused_) {
    return ViewStatus::kPaused;
  }
  return ViewStatus::kRunning;
}

SnakeGame::State SnakeGame::GetState() const { return state_; }

bool SnakeGame::IsPaused() const { return paused_; }

const std::deque<SnakeGame::Position>& SnakeGame::GetSnake() const { return snake_; }

SnakeGame::Position SnakeGame::GetApple() const { return apple_; }

int SnakeGame::GetScore() const { return score_; }

int SnakeGame::GetHighScore() const { return high_score_; }

void SnakeGame::SetCurrentTimeForTests(const uint64_t now_ms) {
  use_time_override_ = true;
  current_time_override_ms_ = now_ms;
}

void SnakeGame::ClearCurrentTimeOverrideForTests() { use_time_override_ = false; }

void SnakeGame::SetAppleForTests(const Position apple) {
  apple_ = apple;
  SyncInfo();
}

void SnakeGame::SetSnakeForTests(const std::deque<Position>& snake,
                                const Direction direction) {
  snake_ = snake;
  direction_ = direction;
  pending_direction_ = direction;
  has_pending_direction_ = false;
  SyncInfo();
}

void SnakeGame::SetStateForTests(const State state) {
  state_ = state;
  SyncInfo();
}

uint64_t SnakeGame::NowMs() const {
  if (use_time_override_) {
    return current_time_override_ms_;
  }
  const auto now = std::chrono::steady_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

uint64_t SnakeGame::CurrentDelayMs() const {
  return (accelerated_ || boost_steps_remaining_ > 0) ? kBoostDelayMs : kBaseDelayMs;
}

SnakeGame& GetSnakeGame() {
  static SnakeGame game;
  return game;
}

}  // namespace s21
