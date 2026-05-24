#include "board_widget.h"

#include <QColor>
#include <QPainter>

namespace s21 {

namespace {

constexpr int kRows = 20;
constexpr int kCols = 10;
constexpr int kPreviewSize = 4;
constexpr int kCellSize = 24;

QColor CellColor(const int value) {
  switch (value) {
    case 1:
      return QColor(90, 200, 120);
    case 2:
      return QColor(230, 80, 80);
    case 3:
      return QColor(255, 205, 70);
    case 4:
      return QColor(80, 200, 220);
    case 5:
      return QColor(200, 120, 240);
    case 6:
      return QColor(90, 120, 240);
    case 7:
      return QColor(240, 240, 240);
    default:
      return QColor(0, 0, 0, 0);
  }
}

}  // namespace

BoardWidget::BoardWidget(QWidget* parent) : QWidget(parent), info_{}, has_preview_(false) {}

void BoardWidget::SetGameInfo(const GameInfo_t& info, const bool has_preview) {
  info_ = info;
  const bool preview_changed = has_preview_ != has_preview;
  has_preview_ = has_preview;
  if (preview_changed) {
    updateGeometry();
  }
  update();
}

void BoardWidget::paintEvent(QPaintEvent* event) {
  QWidget::paintEvent(event);

  QPainter painter(this);
  painter.fillRect(rect(), QColor(24, 28, 34));

  const QRect field_rect(16, 16, kCols * kCellSize, kRows * kCellSize);
  painter.fillRect(field_rect, QColor(32, 36, 44));
  painter.setPen(QColor(80, 84, 92));
  painter.drawRect(field_rect);

  if (info_.field != nullptr) {
    for (int row = 0; row < kRows; ++row) {
      for (int col = 0; col < kCols; ++col) {
        const int value = info_.field[row][col];
        QRect cell_rect(field_rect.left() + col * kCellSize,
                        field_rect.top() + row * kCellSize, kCellSize, kCellSize);
        painter.setPen(QColor(55, 60, 70));
        painter.drawRect(cell_rect);
        if (value > 0) {
          painter.fillRect(cell_rect.adjusted(2, 2, -2, -2), CellColor(value));
        }
      }
    }
  }

  if (has_preview_ && info_.next != nullptr) {
    const QRect preview_rect(field_rect.right() + 32, field_rect.top() + 24,
                             kPreviewSize * kCellSize, kPreviewSize * kCellSize);
    painter.setPen(QColor(80, 84, 92));
    painter.drawText(preview_rect.left(), preview_rect.top() - 10, "Next");
    painter.drawRect(preview_rect);
    for (int row = 0; row < kPreviewSize; ++row) {
      for (int col = 0; col < kPreviewSize; ++col) {
        const int value = info_.next[row][col];
        QRect cell_rect(preview_rect.left() + col * kCellSize,
                        preview_rect.top() + row * kCellSize, kCellSize, kCellSize);
        painter.setPen(QColor(55, 60, 70));
        painter.drawRect(cell_rect);
        if (value > 0) {
          painter.fillRect(cell_rect.adjusted(2, 2, -2, -2), CellColor(value));
        }
      }
    }
  }
}

QSize BoardWidget::sizeHint() const {
  const int preview_width = has_preview_ ? (32 + kPreviewSize * kCellSize + 16) : 16;
  return QSize(16 + kCols * kCellSize + preview_width,
               16 + kRows * kCellSize + 16);
}

QSize BoardWidget::minimumSizeHint() const {
  return sizeHint();
}

}  // namespace s21
