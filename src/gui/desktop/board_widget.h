#ifndef SRC_GUI_DESKTOP_BOARD_WIDGET_H_
#define SRC_GUI_DESKTOP_BOARD_WIDGET_H_

#include <QWidget>

extern "C" {
#include "../../brick_game/common/brick_game_types.h"
}

namespace s21 {

class BoardWidget : public QWidget {
  Q_OBJECT

 public:
  explicit BoardWidget(QWidget* parent = nullptr);

  void SetGameInfo(const GameInfo_t& info, bool has_preview);

 protected:
  void paintEvent(QPaintEvent* event) override;
  QSize minimumSizeHint() const override;

 private:
  GameInfo_t info_;
  bool has_preview_;
};

}  // namespace s21

#endif  // SRC_GUI_DESKTOP_BOARD_WIDGET_H_
