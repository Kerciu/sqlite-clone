#include "../__input__/statement.h"

StatementStatus constructDelete(InputBuffer* buffer, Statement* statement) {
    Operation delete;
    statement->operationBounds = delete;
    char* command = strtok(buffer->buffer, " ");
    char* prompt = strtok(NULL, "\n");

    if (prompt == NULL) {
        return CONSTRUCTION_SYNTAX_ERROR;
    }

    return checkIfFromToCommand(statement, STATEMENT_DELETE, prompt);
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