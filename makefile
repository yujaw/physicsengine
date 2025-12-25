CC ?= clang
INCLUDES ?= include
BUILD_DIR ?= build

CFLAGS ?= -Wall -Wextra -O2 \
    -I$(INCLUDES) \
    -I/opt/homebrew/include \
    -I/opt/homebrew/include/SDL2 \
    -I/opt/homebrew/Cellar/glfw/3.4/include/GLFW

LDFLAGS ?= \
    -L/opt/homebrew/lib \
    -L/opt/homebrew/Cellar/glfw/3.4/lib

LDLIBS ?= -lglfw -lSDL2 \
    -framework Cocoa \
    -framework OpenGL \
    -framework IOKit \
    -framework CoreVideo

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET ?= $(BUILD_DIR)/main

.PHONY: all clean

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -rf $(BUILD_DIR)