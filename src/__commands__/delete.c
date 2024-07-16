#include "../__input__/statement.h"

StatementStatus constructDelete(InputBuffer* buffer, Statement* statement) {
    Operation delete = {0};
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
            return EXECUTE_FAILURE;
        }

        node = getPage(cursor->table->pager, cursor->pageNum);
        numCells = *leafNodeNumCells(node);

        if (cursor->cellNum < numCells) {
            uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);

            if (keyAtIdx == keyToDelete) {
                treeDeleteKey(table, keyToDelete);
                free(cursor);
                return EXECUTE_SUCCESS;
            } else {
                return EXECUTE_NO_ROW_FOUND;
            }
        } else {
            return EXECUTE_FAILURE;
        }
    }  else {
        uint32_t* startPtr = &(statement->operationBounds.startIdx);
        *startPtr = (*startPtr >= getTableMinID(table) ? *startPtr : getTableMinID(table));
        Cursor* startCursor = tableFind(table, *startPtr);

        uint32_t* endPtr = &(statement->operationBounds.endIdx);
        *endPtr = (*endPtr <= getTableMaxID(table) ? *endPtr : getTableMaxID(table));
        Cursor* endCursor = tableFind(table, *endPtr);

        uint32_t iter = *(startPtr);
        while (cursorValue(startCursor) != cursorValue(endCursor) || iter != *endPtr) {
            node = getPage(startCursor->table->pager, startCursor->pageNum);
            numCells = *leafNodeNumCells(node);

            uint32_t keyAtIdx = *leafNodeKey(node, startCursor->cellNum);
            printf("keyAtIdx: %d\nstartPtr: %d\nendPtr: %d\n", keyAtIdx, iter, *endPtr);

            cursorAdvance(startCursor);
            if (keyAtIdx == iter) {
                treeDeleteKey(table, keyAtIdx);
            }
            ++iter;
        }

        free(startCursor);
        free(endCursor);
        return EXECUTE_SUCCESS;
    }
}
