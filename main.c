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

InputBuf* createInputBuffer(void) {
    InputBuf* buf = (InputBuf*)malloc(sizeof(InputBuf));
    buf->buffer = NULL;
    buf->bufferSize = buf->inputBuffer = 0;

    return buf;
}

void destroyInputBuffer(InputBuf* buf) {
    if (buf) {
        free(buf->buffer);
        free(buf);
    }
}

void printPrompt(void) {
    printf(" >> ");
}

void fetchCommand(InputBuf* buf) {
    if (buf == NULL) return;

    ssize_t readBytes = getline(&(buf->buffer), &(buf->bufferSize), stdin);
    if (readBytes <= 0) {
        fprintf(stderr, "Failed to read input..\n");
        exit(EXIT_FAILURE);
    }

    buf->inputBuffer = readBytes - 1;
    buf->buffer[readBytes - 1] = '\0'; // remove trailing '\n'

}

int main(int argc, char* argv[]) {
    InputBuf* input = createInputBuffer();
    if (input == NULL) {
        fprintf(stderr, "Failed to create input buffer..\n");
        return -1;
    }

    while (true) {
        printPrompt();
        fetchCommand(input);

        if (strcmp(input->buffer, ".exit") == 0) {
            destroyInputBuffer(input);
            exit(EXIT_SUCCESS);
        }
        else if(strcmp(input->buffer, ".hello") == 0) {
            printf("Hello !\n");
        }
        else {
            printf("Unrecognized command\n");
        }
    }
}