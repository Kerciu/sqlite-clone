CC = gcc
CFLAGS = -Wall
TARGET = kacpersql
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
HEADS = $(wildcard *.h)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)