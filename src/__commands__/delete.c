#include "../__input__/statement.h"

StatementStatus constructDelete(InputBuffer* buffer, Statement* statement) {
    Operation delete = {0};
    statement->operationBounds = delete;
    char* command = strtok(buffer->buffer, " ");
    char* prompt = strtok(NULL, "\n");

    if (prompt == NULL) {
        return CONSTRUCTION_SYNTAX_ERROR;
    }

    printf("constructDelete: command = %s, prompt = %s\n", command, prompt);
    return checkIfFromToCommand(statement, STATEMENT_DELETE, prompt);
}

ExecuteStatus executeDelete(Statement* statement, Table* table) {
    StatementType type = statement->operationBounds.type;
    bool boundedOperation = (type == OPERATION_IN_BOUNDS || type == OPERATION_STARTING_FROM || type == OPERATION_END_TO);
    printf("executeDelete: StatementType = %d\n", type);

    void* node;
    uint32_t numCells;
    
    if (!boundedOperation) {
        Row* rowToDelete = &(statement->rowToChange);
        uint32_t keyToDelete = rowToDelete->id;
        Cursor* cursor = tableFind(table, keyToDelete);
        if (cursor == NULL) {
            printf("executeDelete: Error, cursor for key %u is NULL\n", keyToDelete);
            return EXECUTE_FAILURE;
        }

        node = getPage(cursor->table->pager, cursor->pageNum);
        numCells = *leafNodeNumCells(node);
        printf("executeDelete: KeyToDelete: %u, cursor cell num: %u, cursor page num: %u\n", keyToDelete, cursor->cellNum, cursor->pageNum);
        
        // Print node keys for debugging
        printf("executeDelete: Node keys at page %u:\n", cursor->pageNum);
        for (uint32_t i = 0; i < numCells; ++i) {
            printf("Key at cell %u: %u\n", i, *leafNodeKey(node, i));
        }

        if (cursor->cellNum < numCells) {
            uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);
            printf("executeDelete: KeyAtIdx: %u\n", keyAtIdx);

            if (keyAtIdx == keyToDelete) {
                treeDeleteKey(table, keyToDelete);
                free(cursor);
                return EXECUTE_SUCCESS;
            } else {
                printf("Error: No such row (of id %u) to change found\n", keyToDelete);
                return EXECUTE_NO_ROW_FOUND;
            }
        } else {
            printf("Error: cursor->cellNum (%u) >= numCells (%u)\n", cursor->cellNum, numCells);
            return EXECUTE_FAILURE;
        }
    }  else {
        uint32_t* startPtr = &(statement->operationBounds.startIdx);
        *startPtr = (*startPtr >= getTableMinID(table) ? *startPtr : getTableMinID(table));
        Cursor* startCursor = tableFind(table, *startPtr);
        printf("executeDelete: startCursor cell num: %u, startCursor page num: %u\n", startCursor->cellNum, startCursor->pageNum);

        uint32_t* endPtr = &(statement->operationBounds.endIdx);
        *endPtr = (*endPtr <= getTableMaxID(table) ? *endPtr : getTableMaxID(table));
        Cursor* endCursor = tableFind(table, *endPtr);
        printf("executeDelete: endCursor cell num: %u, endCursor page num: %u\n", endCursor->cellNum, endCursor->pageNum);

        node = getPage(startCursor->table->pager, startCursor->pageNum);
        numCells = *leafNodeNumCells(node);

        if (startCursor->cellNum < numCells && endCursor->cellNum < numCells) {
            for (uint32_t i = *startPtr; i <= *endPtr; ++i) {
                Cursor* cursor = tableFind(table, i);
                if (cursor->cellNum < numCells) {
                    uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);

                    if (keyAtIdx == i) {
                        treeDeleteKey(table, i);
                    } else {
                        continue;
                    }
                } else {
                    return EXECUTE_FAILURE;
                }
            }

            free(startCursor);
            free(endCursor);
            return EXECUTE_SUCCESS;
        } else {
            free(startCursor);
            free(endCursor);
            return EXECUTE_FAILURE;
        }
    }
}
