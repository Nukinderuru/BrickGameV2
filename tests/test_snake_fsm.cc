#include <gtest/gtest.h>

#include "../src/brick_game/snake/snake_game.h"
#include "snake_test_utils.h"

namespace {

TEST(SnakeFsmTest, StartTransitionsToMoving) {
  snake_test::StartSnakeAt(1000);
  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kMoving);
  EXPECT_EQ(s21::GetSnakeGame().GetSnake().size(), 4U);
}

TEST(SnakeFsmTest, PauseBlocksMovement) {
  snake_test::StartSnakeAt(1000);
  const auto original_head = s21::GetSnakeGame().GetSnake().back();

  s21::userInput(Pause, false);
  snake_test::AdvanceTo(1400);

  EXPECT_TRUE(s21::GetSnakeGame().IsPaused());
  EXPECT_EQ(s21::GetSnakeGame().GetSnake().back(), original_head);
}

TEST(SnakeFsmTest, RestartFromGameOverReturnsToMoving) {
  snake_test::StartSnakeAt(1000);
  s21::GetSnakeGame().SetStateForTests(s21::SnakeGame::State::kGameOver);

  s21::userInput(Start, false);
  s21::updateCurrentState();

  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kMoving);
}

TEST(SnakeFsmTest, ReachingLengthTwoHundredWins) {
  snake_test::StartSnakeAt(1000);
  std::deque<s21::SnakeGame::Position> snake;
  for (int row = 0; row < 19; ++row) {
    for (int col = 0; col < 10; ++col) {
      snake.push_back({row, col});
    }
  }
  for (int col = 0; col < 9; ++col) {
    snake.push_back({19, col});
  }
  s21::GetSnakeGame().SetSnakeForTests(snake, s21::SnakeGame::Direction::kRight);
  s21::GetSnakeGame().SetAppleForTests({19, 9});
  s21::GetSnakeGame().SetStateForTests(s21::SnakeGame::State::kMoving);
  snake_test::AdvanceTo(1300);

  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kWin);
}

TEST(SnakeFsmTest, ResetForMenuReturnsToStartScreen) {
  snake_test::StartSnakeAt(1000);

  s21::resetForMenu();
  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kStart);
  EXPECT_EQ(s21::GetSnakeGame().GetViewStatus(), s21::ViewStatus::kStartScreen);
  EXPECT_EQ(info.pause, 0);
  EXPECT_EQ(info.speed, 1);
  for (int row = 0; row < 20; ++row) {
    for (int col = 0; col < 10; ++col) {
      EXPECT_EQ(info.field[row][col], 0);
    }
  }
}

TEST(SnakeFsmTest, TerminateFromStartEntersGameOverStatus) {
  s21::resetForMenu();

  s21::userInput(Terminate, false);

  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kGameOver);
  EXPECT_EQ(s21::GetSnakeGame().GetViewStatus(), s21::ViewStatus::kGameOver);
}

TEST(SnakeFsmTest, WinStateReportsWinStatus) {
  snake_test::StartSnakeAt(1000);
  s21::GetSnakeGame().SetStateForTests(s21::SnakeGame::State::kWin);

  EXPECT_EQ(s21::GetSnakeGame().GetViewStatus(), s21::ViewStatus::kWin);
}

}  // namespace
