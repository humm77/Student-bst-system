CC = gcc
TARGET = app

SRC = src/main.c src/ui.c src/bst.c
INC = include

UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),arm64)
  RAYLIB_PREFIX ?= /opt/homebrew
else
  RAYLIB_PREFIX ?= /usr/local
endif

CFLAGS = -std=c11 -Wall -Wextra -O2 -I$(INC) -I$(RAYLIB_PREFIX)/include
LDFLAGS = -L$(RAYLIB_PREFIX)/lib
LIBS = -lraylib \
       -framework Cocoa \
       -framework IOKit \
       -framework CoreFoundation \
       -framework CoreAudio \
       -framework OpenGL

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
