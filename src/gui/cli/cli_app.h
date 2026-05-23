#ifndef SRC_GUI_CLI_CLI_APP_H_
#define SRC_GUI_CLI_CLI_APP_H_

#include "../../controller/app_controller.h"

namespace s21 {

class CliApp {
 public:
  CliApp();
  ~CliApp();

  int Run();

 private:
  void HandleInput(int ch);
  void Render();
  void RenderMenu() const;
  void RenderGame() const;
  void RenderOverlay(ViewStatus status) const;
  void DrawCell(int row, int col, int value) const;

  AppController controller_;
  GameInfo_t current_info_;
};

}  // namespace s21

#endif  // SRC_GUI_CLI_CLI_APP_H_
