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
} InputBuffer;

InputBuffer* createInputBuffer(void);
void destroyInputBuffer(InputBuffer* buf);
void printPrompt(void);
void fetchCommand(InputBuffer* buf);

void printUsage(const char* programName);
void printGuidingMsg(void);

#endif // INPUT_BUFFER
