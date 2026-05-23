#include <gtest/gtest.h>

#include <stdlib.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "../src/brick_game/snake/snake.h"
#include "../src/brick_game/snake/snake_game.h"
#include "snake_test_utils.h"

namespace {

class TemporaryHome {
 public:
  TemporaryHome() : original_home_(GetEnv("HOME")) {
    std::string pattern = "/tmp/brick_game_snake_test_XXXXXX";
    std::vector<char> buffer(pattern.begin(), pattern.end());
    buffer.push_back('\0');
    char* created = mkdtemp(buffer.data());
    EXPECT_NE(created, nullptr);
    if (created != nullptr) {
      path_ = created;
      setenv("HOME", path_.c_str(), 1);
    }
  }

  ~TemporaryHome() {
    if (original_home_.empty()) {
      unsetenv("HOME");
    } else {
      setenv("HOME", original_home_.c_str(), 1);
    }
    if (!path_.empty()) {
      std::error_code error;
      std::filesystem::remove_all(path_, error);
    }
  }

  std::filesystem::path HighScorePath() const {
    return std::filesystem::path(path_) / ".local/share/brick_game_snake/high_score";
  }

 private:
  static std::string GetEnv(const char* name) {
    const char* value = getenv(name);
    return value == nullptr ? std::string() : std::string(value);
  }

  std::string original_home_;
  std::string path_;
};

TEST(SnakeScoreTest, EatingAppleIncreasesScore) {
  TemporaryHome temporary_home;
  s21::resetForMenu();
  snake_test::StartSnakeAt(1000);
  s21::GetSnakeGame().SetAppleForTests({10, 6});

  const GameInfo_t info = [] {
    snake_test::AdvanceTo(1300);
    return s21::updateCurrentState();
  }();

  EXPECT_EQ(info.score, 1);
  EXPECT_EQ(s21::GetSnakeGame().GetScore(), 1);
}

TEST(SnakeScoreTest, HighScoreUpdatesImmediatelyWhenExceeded) {
  TemporaryHome temporary_home;
  s21::resetForMenu();
  snake_test::StartSnakeAt(1000);
  s21::GetSnakeGame().SetAppleForTests({10, 6});

  const GameInfo_t info = [] {
    snake_test::AdvanceTo(1300);
    return s21::updateCurrentState();
  }();

  EXPECT_EQ(info.high_score, 1);
  EXPECT_EQ(s21::GetSnakeGame().GetHighScore(), 1);
}

TEST(SnakeScoreTest, HighScoreIsSavedToFile) {
  TemporaryHome temporary_home;
  s21::resetForMenu();
  snake_test::StartSnakeAt(1000);
  s21::GetSnakeGame().SetAppleForTests({10, 6});

  snake_test::AdvanceTo(1300);

  std::ifstream file(temporary_home.HighScorePath());
  int high_score = 0;
  ASSERT_TRUE(file.is_open());
  ASSERT_TRUE(file >> high_score);
  EXPECT_EQ(high_score, 1);
}

TEST(SnakeScoreTest, ResetLoadsSavedHighScore) {
  TemporaryHome temporary_home;
  std::filesystem::create_directories(temporary_home.HighScorePath().parent_path());
  std::ofstream file(temporary_home.HighScorePath(), std::ios::trunc);
  ASSERT_TRUE(file.is_open());
  file << 42 << '\n';
  file.close();

  s21::resetForMenu();
  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(info.high_score, 42);
  EXPECT_EQ(s21::GetSnakeGame().GetHighScore(), 42);
}

TEST(SnakeScoreTest, RestartKeepsLoadedHighScoreAndResetsScore) {
  TemporaryHome temporary_home;
  std::filesystem::create_directories(temporary_home.HighScorePath().parent_path());
  std::ofstream file(temporary_home.HighScorePath(), std::ios::trunc);
  ASSERT_TRUE(file.is_open());
  file << 5 << '\n';
  file.close();

  s21::resetForMenu();
  snake_test::StartSnakeAt(1000);
  s21::GetSnakeGame().SetAppleForTests({10, 6});
  snake_test::AdvanceTo(1300);
  s21::GetSnakeGame().SetStateForTests(s21::SnakeGame::State::kGameOver);

  s21::userInput(Start, false);
  const GameInfo_t info = s21::updateCurrentState();

  EXPECT_EQ(info.score, 0);
  EXPECT_EQ(info.high_score, 5);
}

}  // namespace
