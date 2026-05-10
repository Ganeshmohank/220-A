# Simple build for recursion_demo (gcc, no external deps).
# Executable is written to bin/recursion_demo

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -pedantic -Iinclude
LDFLAGS :=
TARGET  := bin/recursion_demo
SRCS    := src/main.c src/factorial.c
OBJS    := $(SRCS:src/%.c=build/%.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS) | bin build
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	mkdir -p bin

build:
	mkdir -p build

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf bin build
