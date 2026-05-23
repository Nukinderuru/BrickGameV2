#include "main_window.h"

#include <QHBoxLayout>
#include <QPalette>

namespace s21 {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      current_info_{},
      pages_(nullptr),
      menu_page_(nullptr),
      game_page_(nullptr),
      menu_hint_(nullptr),
      tetris_button_(nullptr),
      snake_button_(nullptr),
      quit_button_(nullptr),
      title_label_(nullptr),
      board_widget_(nullptr),
      score_label_(nullptr),
      high_score_label_(nullptr),
      level_label_(nullptr),
      speed_label_(nullptr),
      pause_label_(nullptr),
      controls_label_(nullptr),
      overlay_label_(nullptr) {
  BuildUi();
  connect(&timer_, &QTimer::timeout, this, &MainWindow::Tick);
  timer_.start(16);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (controller_.GetAppState() == AppState::kMenu) {
    if (event->key() == Qt::Key_Up) {
      controller_.MoveMenuSelection(-1);
      RefreshMenuSelection();
    } else if (event->key() == Qt::Key_Down) {
      controller_.MoveMenuSelection(1);
      RefreshMenuSelection();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
      controller_.StartSelectedGame();
      if (controller_.GetAppState() == AppState::kExiting) {
        close();
      } else {
        current_info_ = controller_.GetActiveGame().Update();
        pages_->setCurrentWidget(game_page_);
        RefreshGameView();
      }
    } else if (event->key() == Qt::Key_Q || event->key() == Qt::Key_Escape) {
      close();
    }
    return;
  }

  if (event->key() == Qt::Key_Q || event->key() == Qt::Key_Escape) {
    ReturnToMenu();
    return;
  }

  SendGameAction(event->key(), true);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
  if (controller_.GetAppState() == AppState::kPlaying && event->key() == Qt::Key_Space &&
      controller_.GetActiveGame().UsesActionHold()) {
    SendGameAction(event->key(), false);
  }
}

void MainWindow::Tick() {
  if (controller_.GetAppState() == AppState::kPlaying) {
    current_info_ = controller_.GetActiveGame().Update();
    RefreshGameView();
  }
}

void MainWindow::StartTetris() {
  controller_.StartGame(GameType::kTetris);
  current_info_ = controller_.GetActiveGame().Update();
  pages_->setCurrentWidget(game_page_);
  game_page_->setFocus();
  RefreshGameView();
}

void MainWindow::StartSnake() {
  controller_.StartGame(GameType::kSnake);
  current_info_ = controller_.GetActiveGame().Update();
  pages_->setCurrentWidget(game_page_);
  game_page_->setFocus();
  RefreshGameView();
}

void MainWindow::ReturnToMenu() {
  controller_.ReturnToMenu();
  pages_->setCurrentWidget(menu_page_);
  menu_page_->setFocus();
  RefreshMenuSelection();
}

void MainWindow::BuildUi() {
  pages_ = new QStackedWidget(this);

  menu_page_ = new QWidget(this);
  menu_page_->setFocusPolicy(Qt::StrongFocus);
  QVBoxLayout* menu_layout = new QVBoxLayout(menu_page_);
  QLabel* menu_title = new QLabel("BrickGame v2.0", menu_page_);
  menu_title->setStyleSheet("font-size: 28px; font-weight: bold;");
  menu_title->setAlignment(Qt::AlignHCenter);
  menu_hint_ = new QLabel("Choose a game", menu_page_);
  menu_hint_->setAlignment(Qt::AlignHCenter);
  tetris_button_ = new QPushButton("Tetris", menu_page_);
  snake_button_ = new QPushButton("Snake", menu_page_);
  quit_button_ = new QPushButton("Quit", menu_page_);
  for (QPushButton* button : {tetris_button_, snake_button_, quit_button_}) {
    button->setFixedWidth(220);
    button->setAutoDefault(false);
    button->setDefault(false);
    button->setFocusPolicy(Qt::NoFocus);
  }
  menu_layout->addWidget(menu_title);
  menu_layout->addWidget(menu_hint_);
  menu_layout->addSpacing(16);
  QHBoxLayout* tetris_layout = new QHBoxLayout();
  tetris_layout->addStretch();
  tetris_layout->addWidget(tetris_button_);
  tetris_layout->addStretch();
  menu_layout->addLayout(tetris_layout);
  QHBoxLayout* snake_layout = new QHBoxLayout();
  snake_layout->addStretch();
  snake_layout->addWidget(snake_button_);
  snake_layout->addStretch();
  menu_layout->addLayout(snake_layout);
  QHBoxLayout* quit_layout = new QHBoxLayout();
  quit_layout->addStretch();
  quit_layout->addWidget(quit_button_);
  quit_layout->addStretch();
  menu_layout->addLayout(quit_layout);
  menu_layout->addStretch();
  connect(tetris_button_, &QPushButton::clicked, this, &MainWindow::StartTetris);
  connect(snake_button_, &QPushButton::clicked, this, &MainWindow::StartSnake);
  connect(quit_button_, &QPushButton::clicked, this, &QWidget::close);

  game_page_ = new QWidget(this);
  game_page_->setFocusPolicy(Qt::StrongFocus);
  QHBoxLayout* game_layout = new QHBoxLayout(game_page_);
  QVBoxLayout* left_layout = new QVBoxLayout();
  title_label_ = new QLabel(game_page_);
  title_label_->setStyleSheet("font-size: 24px; font-weight: bold;");
  board_widget_ = new BoardWidget(game_page_);
  overlay_label_ = new QLabel(board_widget_);
  overlay_label_->setAlignment(Qt::AlignCenter);
  overlay_label_->setStyleSheet(
      "background-color: rgba(20, 20, 20, 180); color: white; font-size: 24px;"
      "font-weight: bold; padding: 12px; border-radius: 8px;");
  overlay_label_->hide();
  left_layout->addWidget(title_label_);
  left_layout->addWidget(board_widget_);

  QWidget* sidebar = new QWidget(game_page_);
  QVBoxLayout* sidebar_layout = new QVBoxLayout(sidebar);
  score_label_ = new QLabel(sidebar);
  high_score_label_ = new QLabel(sidebar);
  level_label_ = new QLabel(sidebar);
  speed_label_ = new QLabel(sidebar);
  pause_label_ = new QLabel(sidebar);
  controls_label_ = new QLabel(sidebar);
  controls_label_->setWordWrap(true);
  QPushButton* menu_button = new QPushButton("Back to Menu", sidebar);
  menu_button->setAutoDefault(false);
  menu_button->setDefault(false);
  menu_button->setFocusPolicy(Qt::NoFocus);
  connect(menu_button, &QPushButton::clicked, this, &MainWindow::ReturnToMenu);
  sidebar_layout->addWidget(score_label_);
  sidebar_layout->addWidget(high_score_label_);
  sidebar_layout->addWidget(level_label_);
  sidebar_layout->addWidget(speed_label_);
  sidebar_layout->addWidget(pause_label_);
  sidebar_layout->addSpacing(12);
  sidebar_layout->addWidget(controls_label_);
  sidebar_layout->addStretch();
  sidebar_layout->addWidget(menu_button);

  game_layout->addLayout(left_layout, 1);
  game_layout->addWidget(sidebar);

  pages_->addWidget(menu_page_);
  pages_->addWidget(game_page_);
  setCentralWidget(pages_);
  setWindowTitle("BrickGame v2.0");
  resize(900, 620);
  setFocusPolicy(Qt::StrongFocus);
  menu_page_->setFocus();
  RefreshMenuSelection();
}

void MainWindow::RefreshMenuSelection() {
  const int selected = controller_.GetSelectedMenuIndex();
  const char* active_style = "font-weight: bold; border: 2px solid #5bc0de;";
  tetris_button_->setStyleSheet(selected == 0 ? active_style : "");
  snake_button_->setStyleSheet(selected == 1 ? active_style : "");
  quit_button_->setStyleSheet(selected == 2 ? active_style : "");
}

void MainWindow::RefreshGameView() {
  const GameAdapter& game = controller_.GetActiveGame();
  title_label_->setText(QString::fromStdString(game.GetTitle()));
  board_widget_->SetGameInfo(current_info_, game.HasPreview());
  score_label_->setText(QString("Score: %1").arg(current_info_.score));
  high_score_label_->setText(QString("High score: %1").arg(current_info_.high_score));
  level_label_->setText(QString("Level: %1").arg(current_info_.level));
  speed_label_->setText(QString("Speed: %1").arg(current_info_.speed));
  pause_label_->setText(QString("Paused: %1").arg(current_info_.pause ? "yes" : "no"));

  QString controls;
  for (const std::string& line : game.GetControls()) {
    controls += QString::fromStdString(line) + "\n";
  }
  controls += "Esc/Q: back to menu";
  controls_label_->setText(controls.trimmed());
  UpdateOverlay(game.GetViewStatus());
}

void MainWindow::UpdateOverlay(const ViewStatus status) {
  QString text;
  switch (status) {
    case ViewStatus::kStartScreen:
      text = "Press Enter to start";
      break;
    case ViewStatus::kPaused:
      text = "Paused";
      break;
    case ViewStatus::kGameOver:
      text = "Game Over\nPress Enter to restart";
      break;
    case ViewStatus::kWin:
      text = "You Win\nPress Enter to replay";
      break;
    case ViewStatus::kRunning:
    default:
      break;
  }
  if (text.isEmpty()) {
    overlay_label_->hide();
    return;
  }
  overlay_label_->setGeometry(board_widget_->width() / 2 - 140, board_widget_->height() / 2 - 50,
                             280, 100);
  overlay_label_->setText(text);
  overlay_label_->show();
}

void MainWindow::SendGameAction(const int key, const bool hold) {
  GameAdapter& game = controller_.GetActiveGame();
  switch (key) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
      game.HandleInput(Start, false);
      break;
    case Qt::Key_P:
      game.HandleInput(Pause, false);
      break;
    case Qt::Key_Left:
      game.HandleInput(Left, false);
      break;
    case Qt::Key_Right:
      game.HandleInput(Right, false);
      break;
    case Qt::Key_Up:
      game.HandleInput(Up, false);
      break;
    case Qt::Key_Down:
      game.HandleInput(Down, false);
      break;
    case Qt::Key_Space:
      game.HandleInput(Action, hold);
      break;
    default:
      break;
  }
}

}  // namespace s21
