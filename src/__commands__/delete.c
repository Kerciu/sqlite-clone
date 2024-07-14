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
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    StatementType type = statement->operationBounds.type;

    if (type == OPERATION_SINGLE_ELEMENT) {
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
        else {
            return EXECUTE_FAILURE;
        }
    }
    else if (type == OPERATION_IN_BOUNDS || type == OPERATION_STARTING_FROM || type == OPERATION_END_TO) {
        uint32_t* startPtr = &(statement->operationBounds.startIdx);
        *startPtr = (*startPtr >= getTableMinID(table) ? *startPtr : getTableMinID(table));
        Cursor* startCursor = tableFind(table, *startPtr);

        uint32_t* endPtr = &(statement->operationBounds.endIdx);
        *endPtr = (*endPtr <= getTableMaxID(table) ? *endPtr : getTableMaxID(table)); 
        Cursor* endCursor = tableFind(table, *endPtr);

        if (startCursor->cellNum < numCells && endCursor->cellNum < numCells) {
            for (uint32_t i = *startPtr; i <= *endPtr; ++i) {
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

    else return EXECUTE_FAILURE;
}