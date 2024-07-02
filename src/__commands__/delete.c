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
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    Row* rowToDelete = &(statement->rowToChange);
    uint32_t keyToDelete = rowToDelete->id;
    Cursor* cursor = tableFind(table, keyToDelete);

    if (cursor->cellNum < numCells) {
        uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);

        if (keyAtIdx == keyToDelete) {
            leafNodeDelete(cursor);
            free(cursor);
            return EXECUTE_SUCCESS;
        }
        else return EXECUTE_NO_ROW_FOUND;
    }

    return EXECUTE_FAILURE;
}