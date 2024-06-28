#include <stdio.h>
#include "inputbuf.h"
#include "statement.h"
#include "database.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "You must enter a file name!\n");
        exit(EXIT_FAILURE);
    }

    char* fileHandle = argv[1];
    Table* table = openDataBase(fileHandle);

    InputBuf* input = createInputBuffer();
    if (input == NULL || table == NULL) {
        if (table) closeDataBase(table);
        if (input) free(input);
        fprintf(stderr, "Failed to allocate memory..\n");
        return -1;
    }

    while (true) {
        printPrompt();
        fetchCommand(input);

        if (input->buffer[0] == '.')         // Special command recognized
        {
            switch (executeSpecialCommand(input, table)) {
                case SPECIAL_EXEC_SUCCESS:
                    continue;
                case SPECIAL_EXEC_FAILURE:
                    printf("Failed to recognize special command: %s\n", input->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (constructStatement(input, &statement)) {
            case CONSTRUCTION_SUCCESS:
                break;
            case CONSTRUCTION_FAILURE_UNRECOGNIZED:
                fprintf(stderr, "Unrecognized keyword at the end: %s\n", input->buffer);
                continue;;
            case CONSTRUCTION_FAILURE_NEGATIVE_ID:
                fprintf(stderr, "Error: id cannot be negative\n");
                continue;
            case CONSTRUCTION_FAILURE_TOO_LONG:
                fprintf(stderr, "Error: input string is too long");
                continue;
            case CONSTRUCTION_SYNTAX_ERROR:
                fprintf(stderr, "Syntax error. Could not parse statement\n");
                continue;
        }

        switch(executeStatement(&statement, table)) {
            case EXECUTE_SUCCESS:
                printf("Executed properly :)\n");
                break;
            case EXECUTE_TABLE_FULL:
                fprintf(stderr, "Error. Full table\n");
                break;
            case EXECUTE_DUPLICATE_KEY_FOUND:
                fprintf(stderr, "Error: Duplicate key\n");
                break;
            case EXECUTE_FAILURE:
                fprintf(stderr, "Fatal Error: Execution failure\n");
                break;
        }
    }

    destroyInputBuffer(input);
    closeDataBase(table);
    return 0;
}