# Makefile for Clock Sync project
# Author: Kacper Pasinski, kp459461@students.mimuw.edu.pl
# Date: 2025
# Description: Build and clean rules for peer-time-sync program

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c17
LDFLAGS = -lpthread

# Output executable
TARGET = peer-time-sync

# Source and object files
SRCS = main.c err.c hello.c leader.c message.c peer_list.c socket_utils.c state.c sync.c
OBJS = $(SRCS:.c=.o)

# Default rule: build the program
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each .c file into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets (always run even if files exist)
.PHONY: all clean
