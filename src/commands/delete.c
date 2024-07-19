#include "../input/statement/statement.h"

StatementStatus constructDelete(InputBuffer* buffer, Statement* statement) {
    Operation delete = {0};
    statement->type = STATEMENT_DELETE;
    statement->operationBounds = delete;
    char* command = strtok(buffer->buffer, " ");
    char* prompt = strtok(NULL, "\n");

    if (prompt == NULL) {
        return CONSTRUCTION_SYNTAX_ERROR;
    }

    return checkIfFromToCommand(statement, STATEMENT_DELETE, prompt);
}

ExecuteStatus executeDelete(Statement* statement, Table* table) {
    StatementType type = statement->operationBounds.type;
    bool boundedOperation = (type == OPERATION_IN_BOUNDS || type == OPERATION_EVERY_ELEMENT);

    void* node;
    uint32_t numCells;
    
    if (!boundedOperation) {
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
                return EXECUTE_NO_ROW_FOUND;
            }
        } else {
            return EXECUTE_FAILURE;
        }
    }  else {
        uint32_t start = statement->operationBounds.startIdx;
        start = (start >= getTableMinID(table) ? start : getTableMinID(table));
        Cursor* cursor = tableFind(table, start);

        uint32_t end = statement->operationBounds.endIdx;
        end = (end <= getTableMaxID(table) ? end : getTableMaxID(table));

        if (cursor == NULL) return EXECUTE_FAILURE;

        Row row;
        for (uint32_t i = start; i <= end; ++i) {
            cursor = tableFind(table, i);
            deserializeRow(cursorValue(cursor), &row);
            if (cursor == NULL || row.id != i) continue;

            node = getPage(table->pager, cursor->pageNum);
            numCells = *leafNodeNumCells(node);

            if (cursor->cellNum < numCells) {
                treeDeleteKey(table, i);
            }
        }

        free(cursor);
        return EXECUTE_SUCCESS;
    }
}
