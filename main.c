#include <stdio.h>
#include "inputbuf.h"
#include "statement.h"
#include "row.h"

int main(int argc, char* argv[]) {
    Table* table = createTable();
    InputBuf* input = createInputBuffer();
    if (input == NULL || table == NULL) {
        if (table) freeTable(table);
        if (input) free(input);
        fprintf(stderr, "Failed to allocate memory..\n");
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

        Statement statement;
        switch (constructStatement(input, &statement)) {
            case CONSTRUCTION_SUCCESS:
                break;
            case CONSTRUCTION_FAILURE_UNRECOGNIZED:
                fprintf(stderr, "Unrecognized keyword at the end: %s\n", input->buffer);
                continue;;
            case CONSTRUCTION_FAILURE:
                fprintf(stderr, "Fatal error while constructing statement\n");
                break;
            case CONSTRUCTION_SYNTAX_ERROR:
                fprintf(stderr, "Syntax error. Could not parse statement\n");
                continue;
        }

        switch(executeStatement(&statement, table)) {
            case EXECUTE_SUCCESS:
                printf("Executed properly :)\n");
                break;
            case EXECUTE_TABLE_FAILURE:
                fprintf(stderr, "Error. Full table, failure occured\n");
                break;
        }
        destroyInputBuffer(input);
    }

    freeTable(table);
    return 0;
}