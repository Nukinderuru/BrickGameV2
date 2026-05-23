#include "cli_app.h"

#include <clocale>
#include <ncurses.h>
#include <ctime>

#include <cstring>
#include <vector>

namespace s21 {

namespace {

constexpr int kFieldRows = 20;
constexpr int kFieldCols = 10;
constexpr int kPreviewSize = 4;

const char* StatusTitle(const ViewStatus status) {
  switch (status) {
    case ViewStatus::kStartScreen:
      return "PRESS ENTER";
    case ViewStatus::kPaused:
      return "PAUSED";
    case ViewStatus::kGameOver:
      return "GAME OVER";
    case ViewStatus::kWin:
      return "YOU WIN";
    case ViewStatus::kRunning:
    default:
      return nullptr;
  }
}

const char* StatusHint(const ViewStatus status) {
  switch (status) {
    case ViewStatus::kStartScreen:
      return "TO START";
    case ViewStatus::kPaused:
      return "PRESS P";
    case ViewStatus::kGameOver:
      return "ENTER TO RETRY";
    case ViewStatus::kWin:
      return "ENTER TO REPLAY";
    case ViewStatus::kRunning:
    default:
      return nullptr;
  }
}

}  // namespace

CliApp::CliApp() : current_info_{} {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);
  if (has_colors()) {
    start_color();
    use_default_colors();
    init_pair(1, COLOR_GREEN, -1);
    init_pair(2, COLOR_RED, -1);
    init_pair(3, COLOR_YELLOW, -1);
    init_pair(4, COLOR_CYAN, -1);
    init_pair(5, COLOR_MAGENTA, -1);
    init_pair(6, COLOR_BLUE, -1);
    init_pair(7, COLOR_WHITE, -1);
  }
}

CliApp::~CliApp() { endwin(); }

int CliApp::Run() {
  while (controller_.GetAppState() != AppState::kExiting) {
    if (const int ch = getch(); ch != ERR) {
      HandleInput(ch);
    }
    if (controller_.GetAppState() == AppState::kPlaying) {
      current_info_ = controller_.GetActiveGame().Update();
    }
    Render();
    constexpr timespec sleep_time = {.tv_sec = 0, .tv_nsec = 16000000L};
    nanosleep(&sleep_time, nullptr);
  }
  return 0;
}

void CliApp::HandleInput(const int ch) {
  if (controller_.GetAppState() == AppState::kMenu) {
    switch (ch) {
      case KEY_UP:
        controller_.MoveMenuSelection(-1);
        break;
      case KEY_DOWN:
        controller_.MoveMenuSelection(1);
        break;
      case '\n':
      case KEY_ENTER:
        controller_.StartSelectedGame();
        break;
      case 'q':
      case 'Q':
      case 27:
        controller_.Exit();
        break;
      default:
        break;
    }
    return;
  }

  if (ch == 'q' || ch == 'Q' || ch == 27) {
    controller_.ReturnToMenu();
    return;
  }

  GameAdapter& game = controller_.GetActiveGame();
  switch (ch) {
    case '\n':
    case KEY_ENTER:
      game.HandleInput(Start, false);
      break;
    case 'p':
    case 'P':
      game.HandleInput(Pause, false);
      break;
    case KEY_LEFT:
      game.HandleInput(Left, false);
      break;
    case KEY_RIGHT:
      game.HandleInput(Right, false);
      break;
    case KEY_UP:
      game.HandleInput(Up, false);
      break;
    case KEY_DOWN:
      game.HandleInput(Down, false);
      break;
    case ' ':
      game.HandleInput(Action, true);
      break;
    default:
      break;
  }
}

void CliApp::Render() const {
  erase();
  if (controller_.GetAppState() == AppState::kMenu) {
    RenderMenu();
  } else {
    RenderGame();
  }
  refresh();
}

void CliApp::RenderMenu() const {
  const std::vector<std::string>& items = controller_.GetMenuItems();
  mvprintw(2, 4, "BrickGame v2.0");
  mvprintw(4, 4, "Select a game:");
  for (size_t index = 0; index < items.size(); ++index) {
    const bool selected = static_cast<int>(index) == controller_.GetSelectedMenuIndex();
    mvprintw(6 + static_cast<int>(index) * 2, 6, "%s %s", selected ? ">" : " ",
             items[index].c_str());
  }
  mvprintw(14, 4, "Use Up/Down and Enter");
  mvprintw(15, 4, "Q or Esc exits");
}

void CliApp::RenderGame() const {
  const GameAdapter& game = controller_.GetActiveGame();
  mvprintw(1, 2, "%s", game.GetTitle().c_str());
  for (int row = 0; row < kFieldRows; ++row) {
    mvprintw(row + 3, 2, "|");
    for (int col = 0; col < kFieldCols; ++col) {
      DrawCell(row + 3, col * 2 + 3, current_info_.field[row][col]);
    }
    mvprintw(row + 3, 23, "|");
  }
  mvprintw(23, 2, "+--------------------+");

  if (game.HasPreview()) {
    mvprintw(3, 28, "Next:");
    for (int row = 0; row < kPreviewSize; ++row) {
      for (int col = 0; col < kPreviewSize; ++col) {
        DrawCell(row + 4, col * 2 + 28, current_info_.next[row][col]);
      }
    }
  }

  mvprintw(10, 28, "Score: %d", current_info_.score);
  mvprintw(11, 28, "High:  %d", current_info_.high_score);
  mvprintw(12, 28, "Level: %d", current_info_.level);
  mvprintw(13, 28, "Speed: %d", current_info_.speed);
  mvprintw(14, 28, "Pause: %s", current_info_.pause ? "yes" : "no");

  const std::vector<std::string> controls = game.GetControls();
  for (size_t index = 0; index < controls.size(); ++index) {
    mvprintw(16 + static_cast<int>(index), 28, "%s", controls[index].c_str());
  }

  RenderOverlay(game.GetViewStatus());
}

void CliApp::RenderOverlay(const ViewStatus status) {
  const char* line1 = StatusTitle(status);
  const char* line2 = StatusHint(status);
  if (line1 == nullptr || line2 == nullptr) {
    return;
  }

  constexpr int field_left = 2;
  constexpr int field_width = 22;
  constexpr int box_top = 9;
  constexpr int box_width = 18;
  constexpr int box_left = field_left + (field_width - box_width) / 2;
  constexpr int box_height = 6;
  constexpr int inner_width = box_width - 2;
  char border[19] = {};
  char blank[17] = {};
  const int line1_col = box_left + 1 + (inner_width - static_cast<int>(strlen(line1))) / 2;
  const int line2_col = box_left + 1 + (inner_width - static_cast<int>(strlen(line2))) / 2;

  for (int index = 0; index < box_width; ++index) {
    border[index] = (index == 0 || index == box_width - 1) ? '+' : '-';
  }
  for (int index = 0; index < inner_width; ++index) {
    blank[index] = ' ';
  }

  mvprintw(box_top, box_left, "%s", border);
  for (int row = 1; row < box_height - 1; ++row) {
    mvprintw(box_top + row, box_left, "|");
    mvprintw(box_top + row, box_left + 1, "%s", blank);
    mvprintw(box_top + row, box_left + box_width - 1, "|");
  }
  mvprintw(box_top + box_height - 1, box_left, "%s", border);
  mvprintw(box_top + 2, line1_col, "%s", line1);
  mvprintw(box_top + 3, line2_col, "%s", line2);
}

void CliApp::DrawCell(const int row, const int col, const int value) {
  if (value > 0 && value <= 7 && has_colors()) {
    attron(COLOR_PAIR(value));
    mvprintw(row, col, "[]");
    attroff(COLOR_PAIR(value));
  } else {
    mvprintw(row, col, value ? "[]" : "  ");
  }
}

}  // namespace s21
