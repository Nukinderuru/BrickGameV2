#include <gtest/gtest.h>

#include <deque>

#include "../src/brick_game/snake/snake_game.h"
#include "snake_test_utils.h"

namespace {

TEST(SnakeGameplayTest, EatingAppleGrowsSnake) {
  snake_test::StartSnakeAt(1000);
  s21::GetSnakeGame().SetAppleForTests({10, 6});

  snake_test::AdvanceTo(1300);

  EXPECT_EQ(s21::GetSnakeGame().GetSnake().size(), 5U);
}

TEST(SnakeGameplayTest, ReverseDirectionIsIgnored) {
  snake_test::StartSnakeAt(1000);
  const auto original_head = s21::GetSnakeGame().GetSnake().back();
  s21::userInput(Left, false);

  snake_test::AdvanceTo(1300);

  EXPECT_EQ(s21::GetSnakeGame().GetSnake().back().row, original_head.row);
  EXPECT_EQ(s21::GetSnakeGame().GetSnake().back().col, original_head.col + 1);
}

TEST(SnakeGameplayTest, WallCollisionEndsGame) {
  snake_test::StartSnakeAt(1000);
  std::deque<s21::SnakeGame::Position> snake = {
      {10, 6}, {10, 7}, {10, 8}, {10, 9},
  };
  s21::GetSnakeGame().SetSnakeForTests(snake, s21::SnakeGame::Direction::kRight);
  s21::GetSnakeGame().SetStateForTests(s21::SnakeGame::State::kMoving);

  snake_test::AdvanceTo(1300);

  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kGameOver);
}

TEST(SnakeGameplayTest, SelfCollisionEndsGame) {
  snake_test::StartSnakeAt(1000);
  std::deque<s21::SnakeGame::Position> snake = {
      {11, 4}, {11, 5}, {11, 6}, {10, 6}, {9, 6},
      {9, 5},  {9, 4},  {10, 4}, {10, 5},
  };
  s21::GetSnakeGame().SetSnakeForTests(snake, s21::SnakeGame::Direction::kLeft);
  s21::GetSnakeGame().SetStateForTests(s21::SnakeGame::State::kMoving);

  snake_test::AdvanceTo(1300);

  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kGameOver);
}

TEST(SnakeGameplayTest, ActionBoostDoesNotChangeDisplayedLevelSpeed) {
  snake_test::StartSnakeAt(1000);

  s21::userInput(Action, true);
  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(info.speed, 1);
}

TEST(SnakeGameplayTest, UpdateBeforeDelayDoesNotMoveSnake) {
  snake_test::StartSnakeAt(1000);
  const auto original_head = s21::GetSnakeGame().GetSnake().back();

  snake_test::AdvanceTo(1200);

  EXPECT_EQ(s21::GetSnakeGame().GetSnake().back(), original_head);
}

TEST(SnakeGameplayTest, ValidTurnIsAppliedOnNextStep) {
  snake_test::StartSnakeAt(1000);

  s21::userInput(Up, false);
  snake_test::AdvanceTo(1300);

  EXPECT_EQ(s21::GetSnakeGame().GetSnake().back(),
            (s21::SnakeGame::Position{9, 5}));
}

TEST(SnakeGameplayTest, PauseCanResumeMovement) {
  snake_test::StartSnakeAt(1000);
  s21::userInput(Pause, false);
  s21::userInput(Pause, false);

  snake_test::AdvanceTo(1300);

  EXPECT_FALSE(s21::GetSnakeGame().IsPaused());
  EXPECT_EQ(s21::GetSnakeGame().GetSnake().back(),
            (s21::SnakeGame::Position{10, 6}));
}

TEST(SnakeGameplayTest, TerminateClearsPauseAndStopsGame) {
  snake_test::StartSnakeAt(1000);
  s21::userInput(Pause, false);

  s21::userInput(Terminate, false);
  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kGameOver);
  EXPECT_EQ(info.pause, 0);
}

TEST(SnakeGameplayTest, ActionReleaseRestoresNormalSpeedIndicator) {
  snake_test::StartSnakeAt(1000);
  s21::userInput(Action, true);
  s21::updateCurrentState();

  s21::userInput(Action, false);
  snake_test::AdvanceTo(1100);
  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(info.speed, 1);
}

TEST(SnakeGameplayTest, ActionIgnoredOutsideMovingState) {
  s21::resetForMenu();

  s21::userInput(Action, true);
  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(info.speed, 1);
  EXPECT_EQ(s21::GetSnakeGame().GetState(), s21::SnakeGame::State::kStart);
}

TEST(SnakeGameplayTest, FieldContainsAppleAndSnakeHeadMarkers) {
  snake_test::StartSnakeAt(1000);
  s21::GetSnakeGame().SetAppleForTests({0, 0});

  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(info.field[0][0], 2);
  EXPECT_EQ(info.field[10][5], 3);
  EXPECT_EQ(info.field[10][2], 1);
  EXPECT_EQ(info.next[0][0], 0);
}

TEST(SnakeGameplayTest, FivePointsIncreaseLevelAndSpeed) {
  snake_test::StartSnakeAt(1000);
  for (int index = 0; index < 5; ++index) {
    s21::GetSnakeGame().SetSnakeForTests({{10, 0}, {10, 1}, {10, 2}, {10, 3}},
                                         s21::SnakeGame::Direction::kRight);
    s21::GetSnakeGame().SetStateForTests(s21::SnakeGame::State::kMoving);
    s21::GetSnakeGame().SetAppleForTests({10, 4});
    snake_test::AdvanceTo(1300 + index * 300);
  }

  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(info.score, 5);
  EXPECT_EQ(info.level, 2);
  EXPECT_EQ(info.speed, 2);
  EXPECT_EQ(s21::GetSnakeGame().GetLevel(), 2);
}

TEST(SnakeGameplayTest, LevelIsCappedAtTen) {
  snake_test::StartSnakeAt(1000);
  for (int index = 0; index < 50; ++index) {
    s21::GetSnakeGame().SetSnakeForTests({{10, 0}, {10, 1}, {10, 2}, {10, 3}},
                                         s21::SnakeGame::Direction::kRight);
    s21::GetSnakeGame().SetStateForTests(s21::SnakeGame::State::kMoving);
    s21::GetSnakeGame().SetAppleForTests({10, 4});
    snake_test::AdvanceTo(1300 + index * 300);
  }

  const GameInfo_t info = s21::updateCurrentState();
  EXPECT_EQ(info.level, 10);
  EXPECT_EQ(info.speed, 10);
  EXPECT_EQ(s21::GetSnakeGame().GetLevel(), 10);
}

}  // namespace
