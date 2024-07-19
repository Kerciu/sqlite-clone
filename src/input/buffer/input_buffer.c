#include "input_buffer.h"

InputBuffer* createInputBuffer(void) {
    InputBuffer* buf = (InputBuffer*)malloc(sizeof(InputBuffer));
    buf->buffer = NULL;
    buf->bufferSize = buf->inputBuffer = 0;

    return buf;
}

void destroyInputBuffer(InputBuffer* buf) {
    if (buf) {
        free(buf->buffer);
        free(buf);
    }
}

void printPrompt(void) {
    printf(" >> ");
}

void fetchCommand(InputBuffer* buf) {
    if (buf == NULL) return;

    ssize_t readBytes = getline(&(buf->buffer), &(buf->bufferSize), stdin);
    if (readBytes <= 0) {
        fprintf(stderr, "Failed to read input..\n");
        exit(EXIT_FAILURE);
    }

    buf->inputBuffer = readBytes - 1;
    buf->buffer[readBytes - 1] = '\0'; // remove trailing '\n'

}