CC = gcc
CFLAGS = -g -Wall
TARGET = kacpersql

SRC_DIRS = . src src/__commands__ src/db src/__input__
OBJ_DIR = obj

SRCS = $(wildcard $(foreach dir, $(SRC_DIRS), $(dir)/*.c))
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

HEADS = $(wildcard $(foreach dir, $(SRC_DIRS), $(dir)/*.h))

DIRS = $(sort $(dir $(OBJS)))

.PHONY: all clean

all: $(DIRS) $(TARGET)

$(DIRS):
	mkdir -p $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: %.c $(HEADS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/%.c $(HEADS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
