#include <stdio.h>
#include "inputbuf.h"
#include "statement.h"
#include "row.h"

int main(int argc, char* argv[]) {
    InputBuf* input = createInputBuffer();
    if (input == NULL) {
        fprintf(stderr, "Failed to create input buffer..\n");
        return -1;
    }

    while (true) {
        printPrompt();
        fetchCommand(input);

        if (input->buffer[0] == '.')         // Special command recognized
        {
            switch (executeSpecialCommand(input)) {
                case SPECIAL_EXEC_SUCCESS:
                    continue;
                case SPECIAL_EXEC_FAILURE:
                    printf("Failed to recognize special command: %s\n", input->buffer);
                    continue;
            }
        }
        else
        {
        Statement statement;
        switch (constructStatement(input, &statement)) {
            case CONSTRUCTION_SUCCESS:
                break;
            case CONSTRUCTION_FAILURE_UNRECOGNIZED:
                fprintf(stderr, "Unrecognized keyword at the end: %s\n", input->buffer);
                continue;;
            case CONSTRUCTION_FAILURE:
                fprintf(stderr, "Fatal error while constructing statement\n");
                exit(1);
        }

        executeStatement(&statement);
        destroyInputBuffer(input);
        }
    }

    return 0;
}