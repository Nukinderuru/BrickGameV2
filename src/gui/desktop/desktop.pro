QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET = brick_game_desktop

INCLUDEPATH += ../../src

SOURCES += \
    ../../src/controller/app_controller.cc \
    ../../src/controller/snake_adapter.cc \
    ../../src/controller/tetris_adapter.cc \
    ../../src/brick_game/snake/snake_api.cc \
    ../../src/brick_game/snake/snake_game.cc \
    ../../src/brick_game/tetris/tetris.c \
    ../../src/brick_game/tetris/tetris_board.c \
    ../../src/brick_game/tetris/tetris_fsm.c \
    ../../src/brick_game/tetris/tetris_logic.c \
    ../../src/brick_game/tetris/tetris_random.c \
    ../../src/brick_game/tetris/tetris_storage.c \
    ../../src/brick_game/tetris/tetris_ui_bridge.c \
    ../../src/gui/desktop/board_widget.cc \
    ../../src/gui/desktop/main.cc \
    ../../src/gui/desktop/main_window.cc

HEADERS += \
    ../../src/controller/app_controller.h \
    ../../src/controller/game_adapter.h \
    ../../src/controller/snake_adapter.h \
    ../../src/controller/tetris_adapter.h \
    ../../src/brick_game/common/brick_game_types.h \
    ../../src/brick_game/snake/snake.h \
    ../../src/brick_game/snake/snake_game.h \
    ../../src/brick_game/tetris/tetris.h \
    ../../src/brick_game/tetris/tetris_ui_bridge.h \
    ../../src/gui/desktop/board_widget.h \
    ../../src/gui/desktop/main_window.h
