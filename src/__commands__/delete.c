#include "../__input__/statement.h"

StatementStatus constructDelete(InputBuffer* buffer, Statement* statement) {
    char* command = strtok(buffer->buffer, " ");
    char* idPrompt = strtok(NULL, " ");

    if (idPrompt == NULL) {
        return CONSTRUCTION_SYNTAX_ERROR;
    }

    uint32_t id = atoi(idPrompt);
    if (id < 0 || id > INT_MAX) {
        return CONSTRUCTION_FAILURE_NEGATIVE_ID;
    }

    statement->rowToChange.id = id;
    statement->type = STATEMENT_DELETE;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDelete(Statement* statement, Table* table) {
    return EXECUTE_FAILURE;
}