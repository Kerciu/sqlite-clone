CC = gcc
CFLAGS = -Wall
TARGET = kacpersql

# Katalogi źródeł i nagłówków
SRC_DIRS = . src src/__commands__ src/db src/__input__
OBJ_DIR = obj

# Znajdowanie wszystkich plików .c w katalogach źródeł oraz main.c
SRCS = $(wildcard $(foreach dir, $(SRC_DIRS), $(dir)/*.c))
# Tworzenie odpowiadających plików .o w katalogu obj z zachowaniem struktury katalogów
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

# Znajdowanie wszystkich plików .h w katalogach źródeł
HEADS = $(wildcard $(foreach dir, $(SRC_DIRS), $(dir)/*.h))

# Tworzenie katalogów obj, jeśli nie istnieją
DIRS = $(sort $(dir $(OBJS)))

.PHONY: all clean

all: $(DIRS) $(TARGET)

# Tworzenie katalogów obj, jeśli nie istnieją
$(DIRS):
	mkdir -p $@

# Kompilacja targetu
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Kompilacja plików .o z plików .c w katalogu głównym
$(OBJ_DIR)/%.o: %.c $(HEADS)
	$(CC) $(CFLAGS) -c $< -o $@

# Kompilacja plików .o z plików .c w podkatalogach
$(OBJ_DIR)/%.o: src/%.c $(HEADS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
