#include "../__input__/statement.h"

void printCommandMan(char* commandName) {
    if (strcmp(commandName, "INSERT") == 0) {
        printf("INSERT <id> <username> <email>\n");
        printf("Inserts a new row into the table.\n");
    } else if (strcmp(commandName, "SELECT") == 0) {
        printf("SELECT\n");
        printf("Selects all rows from the table.\n");
    } else if (strcmp(commandName, "DELETE") == 0) {
        printf("DELETE <id>\n");
        printf("Deletes the row with the specified id.\n");
    } else if (strcmp(commandName, "UPDATE") == 0) {
        printf("UPDATE <id> <username> <email>\n");
        printf("Updates the row with the specified id.\n");
    } else if (strcmp(commandName, "OPEN TABLE") == 0) {
        printf("OPEN TABLE <file-name>\n");
        printf("Opens an existing table or creates a new one with the specified file name.\n");
    } else if (strcmp(commandName, "DROP") == 0) {
        printf("DROP *<file-name>\t* - optional\n");
        printf("Deletes the table with specified file name or clears current working one\n");
    } else if (strcmp(commandName, "ALIGN") == 0) {
        printf("ALIGN *FROM <id> / \n\t*TO <id> / \n\t*<start-id> *<end-id>\n\t* - optional\n");
        printf("Align ids in whole table if no arguments given or align in specified bounds.\n");
    } else {
        printf("Unrecognized command: %s\n", commandName);
    }
}

void printCommands() {
    printf("Available commands:\n");
    printf(" - INSERT\n");
    printf(" - SELECT\n");
    printf(" - DELETE\n");
    printf(" - UPDATE\n");
    printf(" - OPEN TABLE\n");
    printf(" - DROP\n");
    printf(" - ALIGN\n");
    printf("Use HELP <command> for more details on a specific command.\n");
}

StatementStatus constructHelp(InputBuffer* buffer, Statement* statement) {
    char* command = strtok(buffer->buffer, " ");
    char* manual = strtok(NULL, "\n");
    statement->commandManual = (manual == NULL ? NULL : manual);
    statement->type = STATEMENT_HELP;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeHelp(Statement* statement) {
    statement->commandManual == NULL ? printCommands() : printCommandMan(statement->commandManual);
    statement->commandManual = NULL;
    return EXECUTE_SUCCESS;
}
