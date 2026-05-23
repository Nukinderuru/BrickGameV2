#ifndef TESTS_SNAKE_TEST_UTILS_H_
#define TESTS_SNAKE_TEST_UTILS_H_

#include "../src/brick_game/snake/snake.h"
#include "../src/brick_game/snake/snake_game.h"

namespace snake_test {

inline void StartSnakeAt(const uint64_t start_ms) {
  s21::SnakeGame& game = s21::GetSnakeGame();
  game.SetCurrentTimeForTests(start_ms);
  game.ResetToMenu();
  s21::userInput(Start, false);
  s21::updateCurrentState();
}

inline void AdvanceTo(const uint64_t time_ms) {
  s21::SnakeGame& game = s21::GetSnakeGame();
  game.SetCurrentTimeForTests(time_ms);
  s21::updateCurrentState();
}

}  // namespace snake_test

#endif  // TESTS_SNAKE_TEST_UTILS_H_
