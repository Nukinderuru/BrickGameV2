#ifndef SRC_CONTROLLER_GAME_ADAPTER_H_
#define SRC_CONTROLLER_GAME_ADAPTER_H_

#include <string>
#include <vector>

extern "C" {
#include "../brick_game/common/brick_game_types.h"
}

#include "../brick_game/common/view_status.h"

namespace s21 {

class GameAdapter {
 public:
  virtual ~GameAdapter() = default;

  virtual std::string GetTitle() const = 0;
  virtual std::vector<std::string> GetControls() const = 0;
  virtual bool HasPreview() const = 0;
  virtual bool UsesActionHold() const = 0;

  virtual void Reset() = 0;
  virtual void HandleInput(UserAction_t action, bool hold) = 0;
  virtual GameInfo_t Update() = 0;
  virtual ViewStatus GetViewStatus() const = 0;
};

}  // namespace s21

#endif  // SRC_CONTROLLER_GAME_ADAPTER_H_
