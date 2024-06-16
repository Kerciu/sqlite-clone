#include <stdio.h>
#include "inputbuf.h"

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