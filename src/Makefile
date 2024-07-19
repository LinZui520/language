CC = gcc

CFLAGS = -Wall -Werror -std=gnu17 -nostdlib -fno-stack-protector

SRC_DIR = .
OBJ_DIR = ../obj
BIN_DIR = ../bin

SRCS_C = $(wildcard $(SRC_DIR)/*.c)
SRCS_S = $(wildcard $(SRC_DIR)/*.s)

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS_C))
OBJS += $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(SRCS_S))

TARGET = $(BIN_DIR)/compiler

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.[cs] | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
