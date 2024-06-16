#include "statement.h"

SpecialCommandStatus executeSpecialCommand(InputBuf* buffer) {
    if (buffer->buffer == NULL) return SPECIAL_EXEC_FAILURE;
    if (strcmp(buffer->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    else return SPECIAL_EXEC_FAILURE;
}

StatementStatus constructStatement(InputBuf* buffer, Statement* statement) {
    if (strncmp(buffer->buffer, "insert", 6) == 0) {
        // TODO IMPLEMENT
        statement->type = STATEMENT_INSERT;
        return CONSTRUCTION_SUCCESS;
    }
    if (strncmp(buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return CONSTRUCTION_SUCCESS;
    }
    return CONSTRUCTION_FAILURE_UNRECOGNIZED;
}

void executeStatement(Statement* statement) {
    switch(statement->type) {
        case STATEMENT_INSERT:
            printf("Inserting.. it works!\n");
            break;
        case STATEMENT_SELECT:
            printf("Selection... it works!\n");
            break;
    }
}