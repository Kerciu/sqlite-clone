#include "../__input__/statement.h"

StatementStatus constructDrop(InputBuffer* buffer, Statement* statement) {
    statement->operationBounds.type = OPERATION_IN_BOUNDS;
    statement->operationBounds.startIdx = 0;
    statement->operationBounds.endIdx = INT_MAX;

    statement->type = STATEMENT_DROP;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDrop(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    uint32_t* startPtr = &(statement->operationBounds.startIdx);
    *startPtr = (*startPtr >= getTableMinID(table) ? *startPtr : getTableMinID(table));
    Cursor* startCursor = tableFind(table, *startPtr);

    uint32_t* endPtr = &(statement->operationBounds.endIdx);
    *endPtr = (*endPtr <= getTableMaxID(table) ? *endPtr : getTableMaxID(table)); 
    Cursor* endCursor = tableFind(table, *endPtr);

    if (startCursor->cellNum < numCells && endCursor->cellNum < numCells) {
        for (uint32_t i = *startPtr; i <= *endPtr; ++i) {
            printf("Start and end values in a while loop: %d and %d\n", i, *endPtr);
            Cursor* cursor = tableFind(table, i);
            if (cursor->cellNum < numCells) {
                uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);

                if (keyAtIdx == i) {
                    leafNodeDelete(cursor);
                }
                else continue;
            }
            else {
                return EXECUTE_FAILURE;
            }
        }

        free(startCursor);
        free(endCursor);
        return EXECUTE_SUCCESS;
    }
    else {
        free(startCursor);
        free(endCursor);
        return EXECUTE_FAILURE;
    }
}