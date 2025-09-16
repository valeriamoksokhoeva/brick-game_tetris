CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LDFLAGS = -lncurses
TEST_LDFLAGS = 

SRC_DIR = .
BUILD_DIR = build
INSTALL_DIR = ../install
DOC_DIR = ../doc
DIST_DIR = ../dist
DIST_NAME = brick_game-1.0

BIN_SRCS = $(wildcard gui/cli/*.c)
GAME_SRCS = $(wildcard brick_game/tetris/*.c)
HEADERS = $(wildcard gui/cli/*.h brick_game/tetris/*.h)
TEST_SRCS = tests/test.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

BINARY = tetris
LIBRARY = tetris.a
TEST_BINARY = unit_tests

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	TEST_LDFLAGS += -lcheck -lm -lpthread -lrt -lsubunit
	LEAK_CHECK = valgrind --leak-check=full --show-leak-kinds=all -s ./$(TEST_BINARY)
	OPEN_CMD = xdg-open
else
	TEST_LDFLAGS += -lcheck -lm -lpthread
	LEAK_CHECK = leaks --atExit -- ./$(TEST_BINARY)
	OPEN_CMD = open
endif


.PHONY: all install uninstall clean dvi dist test gcov_report

all: $(BUILD_DIR)/$(BINARY)

run: $(BUILD_DIR)/$(BINARY)
	./$(BUILD_DIR)/$(BINARY)

install: $(BUILD_DIR)/$(BINARY)
	@mkdir -p $(INSTALL_DIR)
	cp $< $(INSTALL_DIR)

uninstall: 
	rm -f $(INSTALL_DIR)/$(BINARY)

clean:
	rm -rf $(BUILD_DIR) $(INSTALL_DIR) $(DIST_DIR) $(DOC_DIR) $(TEST_OBJS) $(TEST_BINARY) *.gcda *.gcno coverage.info coverage_report/

dvi:
	doxygen

dist: clean
	@mkdir ../dist/
	tar czvf ../dist/tetris.tar.gz ./brick_game ./gui ./tests ./Doxyfile ./Makefile 

$(BUILD_DIR)/$(BINARY): $(MAIN_SRC) $(BIN_SRCS) $(GAME_SRCS) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(MAIN_SRC) $(BIN_SRCS) $(GAME_SRCS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/$(LIBRARY): $(GAME_SRCS) $(wildcard brick_game/tetris/*.h)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(GAME_SRCS)
	ar rcs $@ *.o
	rm -f *.o

library: $(BUILD_DIR)/$(LIBRARY)

test: $(TEST_BINARY)
	@echo "Running tests..."
	./$(TEST_BINARY)
	@echo "Tests completed"
# 	rm -f $(TEST_BINARY)

$(TEST_BINARY): $(TEST_SRCS) $(GAME_SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(TEST_SRCS) $(GAME_SRCS) -o $@ $(TEST_LDFLAGS)

test_with_coverage: CFLAGS += --coverage
test_with_coverage: test

gcov_report: clean test_with_coverage
	lcov -t "Report" -o coverage.info -c -d .
	genhtml coverage.info --output-directory coverage_report/
	$(OPEN_CMD) coverage_report/index.html


leaks: test
	$(LEAK_CHECK)

valgrind: test
	$(LEAK_CHECK)

style:
	cp ../materials/linters/.clang-format .
	clang-format -n $(BIN_SRCS) $(GAME_SRCS) $(HEADERS) brick_game/*.h tests/test.c
	clang-format -i $(BIN_SRCS) $(GAME_SRCS) $(HEADERS) brick_game/*.h tests/test.c
	rm -rf .clang-format
