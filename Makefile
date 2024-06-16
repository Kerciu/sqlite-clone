CC = gcc
CFLAGS = -Wall
TARGET = kacpersql
SRCS = main.c
OBJS = main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)

clean:
	rm -f $(OBJS) $(TARGET)
