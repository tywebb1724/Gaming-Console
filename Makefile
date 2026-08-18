# ============================================================
#  SP1DER GAMES Console Emulator
#    make             build (optimised)
#    make run         build, then launch from the repo root
#    make DEBUG=1     build with sanitizers into build-debug/
#    make clean       remove build output
#    make -j4         parallel build
# ============================================================

CC ?= gcc

STD  := -std=gnu11
WARN := -Wall -Wextra -Wno-unused-parameter
INC  := -Isrc -Isrc/ui -Isrc/play -Isrc/libretro

RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS := -L/usr/local/lib -lraylib -lGL -lX11

ifdef DEBUG
  BUILD  := build-debug
  TARGET := SP1DER-GAMES-debug
  OPT    := -O0 -g3
  SANI   := -fsanitize=address,undefined -fno-omit-frame-pointer
else
  BUILD  := build
  TARGET := SP1DER-GAMES
  OPT    := -O2 -g
  SANI   :=
endif

CFLAGS  := $(STD) $(WARN) $(OPT) $(SANI) $(INC) $(RAYLIB_CFLAGS)
LDFLAGS := $(SANI)
LDLIBS  := $(RAYLIB_LIBS) -lm -lpthread -ldl -lrt

SRCS := \
  src/main.c \
  src/var.c \
  src/games.c \
  src/categories.c \
  src/states.c \
  src/controller.c \
  src/ui/ui.c \
  src/ui/uipause.c \
  src/ui/theme.c \
  src/ui/brightness.c \
  src/ui/diagnostics.c \
  src/ui/erase.c \
  src/play/play.c \
  src/play/playpause.c \
  src/libretro/retro_bridge.c

OBJS := $(SRCS:%.c=$(BUILD)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build build-debug SP1DER-GAMES SP1DER-GAMES-debug

-include $(DEPS)

.PHONY: all run clean
