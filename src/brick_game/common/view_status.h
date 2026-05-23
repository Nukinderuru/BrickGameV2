#ifndef SRC_BRICK_GAME_COMMON_VIEW_STATUS_H_
#define SRC_BRICK_GAME_COMMON_VIEW_STATUS_H_

namespace s21 {

enum class ViewStatus {
  kStartScreen = 0,
  kRunning,
  kPaused,
  kGameOver,
  kWin
};

}  // namespace s21

#endif  // SRC_BRICK_GAME_COMMON_VIEW_STATUS_H_
