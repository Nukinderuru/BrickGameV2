#include "snake_storage.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace s21 {

namespace {

constexpr char kRelativeDirectory[] = ".local/share/brick_game_snake";
constexpr char kHighScoreFile[] = "high_score";

std::filesystem::path BuildHighScorePath() {
  const char* home = std::getenv("HOME");
  if (home == nullptr || home[0] == '\0') {
    return {};
  }
  return std::filesystem::path(home) / kRelativeDirectory / kHighScoreFile;
}

}  // namespace

int LoadSnakeHighScore() {
  const std::filesystem::path path = BuildHighScorePath();
  if (path.empty()) {
    return 0;
  }

  std::ifstream file(path);
  int high_score = 0;
  if (file >> high_score && high_score >= 0) {
    return high_score;
  }
  return 0;
}

void SaveSnakeHighScore(const int high_score) {
  const std::filesystem::path path = BuildHighScorePath();
  if (path.empty()) {
    return;
  }

  std::error_code error;
  std::filesystem::create_directories(path.parent_path(), error);
  if (error) {
    return;
  }

  std::ofstream file(path, std::ios::trunc);
  if (!file.is_open()) {
    return;
  }
  file << high_score << '\n';
}

}  // namespace s21
