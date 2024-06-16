#ifndef INPUT_BUFFER
#define INPUT_BUFFER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stddef.h>

typedef struct {
    char* buffer;
    size_t bufferSize;
    ssize_t inputBuffer;
} InputBuf;

InputBuf* createInputBuffer(void);
void destroyInputBuffer(InputBuf* buf);
void printPrompt(void);
void fetchCommand(InputBuf* buf);

#endif // INPUT_BUFFER
