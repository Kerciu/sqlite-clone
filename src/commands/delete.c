#include "../input/statement/statement.h"

StatementStatus constructDelete(InputBuffer* buffer, Statement* statement) {
    Operation delete = {0};
    statement->type = STATEMENT_DELETE;
    statement->operationBounds = delete;
    char* command = strtok(buffer->buffer, " ");
    char* prompt = strtok(NULL, "\n");

    return (
    (prompt == NULL || !isNumber(prompt) ||
        validateBound(prompt) != BOUND_CREATION_SUCCESS)
    ?
    CONSTRUCTION_SYNTAX_ERROR : CONSTRUCTION_SUCCESS);
}

ExecuteStatus executeDelete(Statement* statement, Table* table) {
    void* node;
    uint32_t numCells;
    
    Row* rowToDelete = &(statement->rowToChange);
    uint32_t keyToDelete = rowToDelete->id;
    Cursor* cursor = tableFind(table, keyToDelete);
    if (cursor == NULL) {
        fprintf(stderr, "Cursor did not found key %d in table\n", keyToDelete);
        return EXECUTE_FAILURE;
    }

    node = getPage(table->pager, cursor->pageNum);
    numCells = *leafNodeNumCells(node);

    if (cursor->cellNum < numCells) {
        uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);

        if (keyAtIdx == keyToDelete) {
            treeDeleteKey(table, keyToDelete);
            printf("Deleting key %d\n", keyToDelete);
            free(cursor);
            return EXECUTE_SUCCESS;
        } else {
            free(cursor);
            return EXECUTE_NO_ROW_FOUND;
        }
    } else {
        free(cursor);
        return EXECUTE_FAILURE;
    }
}
