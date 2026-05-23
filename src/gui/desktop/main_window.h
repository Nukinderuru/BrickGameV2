#ifndef SRC_GUI_DESKTOP_MAIN_WINDOW_H_
#define SRC_GUI_DESKTOP_MAIN_WINDOW_H_

#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "../../controller/app_controller.h"
#include "board_widget.h"

namespace s21 {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;

 private slots:
  void Tick();
  void StartTetris();
  void StartSnake();
  void ReturnToMenu();

 private:
  void BuildUi();
  void RefreshMenuSelection();
  void RefreshGameView();
  void UpdateOverlay(ViewStatus status);
  void SendGameAction(int key, bool hold);

  AppController controller_;
  QTimer timer_;
  GameInfo_t current_info_;

  QStackedWidget* pages_;
  QWidget* menu_page_;
  QWidget* game_page_;
  QLabel* menu_hint_;
  QPushButton* tetris_button_;
  QPushButton* snake_button_;
  QPushButton* quit_button_;
  QLabel* title_label_;
  BoardWidget* board_widget_;
  QLabel* score_label_;
  QLabel* high_score_label_;
  QLabel* level_label_;
  QLabel* speed_label_;
  QLabel* pause_label_;
  QLabel* controls_label_;
  QLabel* overlay_label_;
};

}  // namespace s21

#endif  // SRC_GUI_DESKTOP_MAIN_WINDOW_H_
