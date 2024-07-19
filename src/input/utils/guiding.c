#include "../buffer/input_buffer.h"
#include "../statement/statement.h"

void printUsage(const char* programName) {
    fprintf(stderr, "Usage: %s <file-name>\n", programName);
    fprintf(stderr, " - No arguments: Need to use \"OPEN TABLE <file-name>\"\n");
    fprintf(stderr, " - <file-name>: Use the specified file as the database\n");
}

void printGuidingMsg(void) {
    printf("Enter in \"OPEN TABLE <file-name>\" to create or open table\n");
    printf("Enter in \"HELP\" for command list\n");
    printf("Enter in \".exit\" to exit the program\n");
}

void chooseWorkingDB(InputBuffer* buffer, Statement* statement, Table** table) {
    while (true) {
        printPrompt();
        fetchCommand(buffer);
        if (strncmp(buffer->buffer, "OPEN TABLE", 10) == 0) { 
            if (constructStatement(buffer, statement) != CONSTRUCTION_SUCCESS) {
                fprintf(stderr, "Failure occured while opening table: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            if (executeOpenTable(statement, table) == EXECUTE_SUCCESS) {
                return;
            } else {
                fprintf(stderr, "Failed to open table.\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (strncmp(buffer->buffer, ".exit", 5) == 0) {
            exit(EXIT_SUCCESS);
        }
        else if (strncmp(buffer->buffer, "HELP", 4) == 0) {
            if (constructHelp(buffer, statement) != CONSTRUCTION_SUCCESS) {
                fprintf(stderr, "Failure while opening help manual: %d\n", errno);
                exit(EXIT_FAILURE);
            }
            if (executeHelp(statement) == EXECUTE_SUCCESS) {
                continue;
            } else {
                fprintf(stderr, "Failed to execute help manual.\n");
                exit(EXIT_FAILURE);
            }
        }
        else {
            printGuidingMsg();
            continue;
        }
    }
}
