#ifndef SRC_CONTROLLER_SNAKE_ADAPTER_H_
#define SRC_CONTROLLER_SNAKE_ADAPTER_H_

#include "game_adapter.h"

namespace s21 {

class SnakeAdapter : public GameAdapter {
 public:
  std::string GetTitle() const override;
  std::vector<std::string> GetControls() const override;
  bool HasPreview() const override;
  bool UsesActionHold() const override;

  void Reset() override;
  void HandleInput(UserAction_t action, bool hold) override;
  GameInfo_t Update() override;
  ViewStatus GetViewStatus() const override;
};

}  // namespace s21

#endif  // SRC_CONTROLLER_SNAKE_ADAPTER_H_
