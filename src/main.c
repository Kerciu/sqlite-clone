#include <stdio.h>
#include "__input__/input.h"
#include "__input__/statement.h"
#include "db/database.h"

void printUsage(const char* programName) {
    fprintf(stderr, "Usage: %s [<file-name>]\n", programName);
    fprintf(stderr, " - No arguments: Need to use \"OPEN TABLE <file-name>\"\n");
    fprintf(stderr, " - <file-name>: Use the specified file as the database\n");
}

void printEncourageMsg(void) {
    printf("Enter in \"OPEN TABLE <file-name>\" to create or open table\n");
    printf("Enter in \"HELP\" for command list\n");
    printf("Enter in \".exit\" to exit the program\n");
}

void chooseWorkingDB(InputBuffer* buffer, Statement* statement, Table** table) {
    while (true) {
        printEncourageMsg();
        printPrompt();
        fetchCommand(buffer);
        if (strncmp(buffer->buffer, "OPEN TABLE", 10) == 0) {
            if (constructStatement(buffer, statement) != CONSTRUCTION_SUCCESS) {
                fprintf(stderr, "Failure occured while opening database: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            if (executeOpenTable(statement, table) == EXECUTE_SUCCESS) {
                printf("Table opened successfully.\n");
                return;
            } else {
                fprintf(stderr, "Failed to open table.\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strncmp(buffer->buffer, ".exit", 5) == 0) {
            exit(EXIT_SUCCESS);
        }
        else continue;
    }
}

int main(int argc, char* argv[]) {
    Statement statement;
    Table* table = NULL;
    InputBuffer* input = createInputBuffer();
    if (input == NULL) {
        fprintf(stderr, "Failed to allocate memory..\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 1) {
        chooseWorkingDB(input, &statement, &table);
    }
    else if (argc == 2) {
        char* fileHandle = argv[1];
        Table* table = openDataBase(fileHandle);
    }
    else {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
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
            case EXECUTE_NO_ROW_FOUND:
                fprintf(stderr, "Error: No such row (of id %d) to change found\n", statement.rowToChange.id);
                break;
            case EXECUTE_DROP_FAILURE:
                fprintf(stderr, "Failed to drop the data base \"%s\"\n", table->fileHandle);
        }
    }

    destroyInputBuffer(input);
    closeDataBase(table);
    return 0;
}