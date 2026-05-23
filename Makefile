CXX = g++
CC = gcc
CXXFLAGS = -Wall -Wextra -Werror -std=c++17
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L
CURSES_LIB = -lncurses
BUILD_DIR = build
DIST_DIR = dist
PREFIX ?= $(HOME)/.local
BINDIR = $(PREFIX)/bin
OBJ_DIR = $(BUILD_DIR)/obj

TETRIS_SRC = \
	src/brick_game/tetris/tetris.c \
	src/brick_game/tetris/tetris_board.c \
	src/brick_game/tetris/tetris_fsm.c \
	src/brick_game/tetris/tetris_logic.c \
	src/brick_game/tetris/tetris_random.c \
	src/brick_game/tetris/tetris_storage.c \
	src/brick_game/tetris/tetris_ui_bridge.c

CLI_CPP_SRC = \
	src/controller/app_controller.cc \
	src/controller/snake_adapter.cc \
	src/controller/tetris_adapter.cc \
	src/brick_game/snake/snake_api.cc \
	src/brick_game/snake/snake_game.cc \
	src/gui/cli/cli_app.cc \
	src/gui/cli/main.cc

TEST_SRC = \
	tests/test_main.cc \
	tests/test_snake_fsm.cc \
	tests/test_snake_gameplay.cc

TETRIS_OBJ = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(TETRIS_SRC))
CLI_CPP_OBJ = $(patsubst src/%.cc,$(OBJ_DIR)/%.o,$(CLI_CPP_SRC))

all: $(BUILD_DIR)/brick_game_cli desktop

$(BUILD_DIR)/brick_game_cli: $(TETRIS_OBJ) $(CLI_CPP_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(CURSES_LIB)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I./src -c $< -o $@

$(OBJ_DIR)/%.o: src/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I./src -c $< -o $@

desktop:
	@mkdir -p $(BUILD_DIR)/desktop
	@test -d "$(shell qmake6 -query QT_INSTALL_HEADERS)" || \
		(printf 'Qt6 development headers are not installed at %s\n' "$(shell qmake6 -query QT_INSTALL_HEADERS)" && exit 1)
	qmake6 src/gui/desktop/desktop.pro -o $(BUILD_DIR)/desktop/Makefile
	$(MAKE) -C $(BUILD_DIR)/desktop

install: all
	@mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BUILD_DIR)/brick_game_cli $(DESTDIR)$(BINDIR)/brick_game_cli
	cp $(BUILD_DIR)/desktop/brick_game_desktop $(DESTDIR)$(BINDIR)/brick_game_desktop

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/brick_game_cli
	rm -f $(DESTDIR)$(BINDIR)/brick_game_desktop

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

dvi:
	@printf 'README.md\n\ndocs/tetris_fsm.md\ndocs/snake_fsm.md\n'

dist:
	@mkdir -p $(DIST_DIR)
	tar -czf $(DIST_DIR)/brick_game_v2.tar.gz README.md Makefile docs src tests

tests:
	@mkdir -p $(BUILD_DIR)
	@test -f /usr/include/gtest/gtest.h || \
		(printf 'GTest headers are not installed under /usr/include/gtest\n' && exit 1)
	$(CXX) $(CXXFLAGS) -I./src $(TEST_SRC) src/brick_game/snake/snake_api.cc \
		src/brick_game/snake/snake_game.cc -lgtest -lgtest_main -lpthread -o $(BUILD_DIR)/tests
	$(BUILD_DIR)/tests

.PHONY: all desktop install uninstall clean dvi dist tests
