#include "../__input__/statement.h"

StatementStatus constructAlign(InputBuffer* buffer, Statement* statement) {
    /*
    *
    *   TODO --> MAKE THIS COMMAND USE PARSING.C LIB 
    * 
    */
    char* command = strtok(buffer->buffer, " ");
    char* argOne = strtok(NULL, " ");
    char* argTwo = strtok(NULL, " ");
    Operation align;

    if (argOne == NULL || argTwo == NULL) {
        align.startIdx = 0;
        align.endIdx = INT_MAX;
        align.type = OPERATION_IN_BOUNDS;
        statement->operationBounds = align;
        statement->type = STATEMENT_ALIGN;
        return CONSTRUCTION_SUCCESS;
    }


    if (strcmp(argOne, "TO") == 0) {
        align.type = OPERATION_END_TO;
        
        if (validateBound(argTwo) == BOUND_CREATION_SUCCESS) {
            uint32_t endBound = atoi(argTwo);
            align.startIdx = 0;
            align.endIdx = atoi(argTwo);
        }
        else {
            return CONSTRUCTION_FAILURE_WRONG_BONDS;
        }

        statement->operationBounds = align;
        statement->type = STATEMENT_ALIGN;
        return CONSTRUCTION_SUCCESS;
    }

    if (strcmp(argOne, "FROM") == 0) {
        align.type = OPERATION_STARTING_FROM;

        if (validateBound(argTwo) == BOUND_CREATION_SUCCESS) {
            uint32_t endBound = atoi(argTwo);
            align.startIdx = atoi(argTwo);
            align.endIdx = INT_MAX;
        }
        else {
            return CONSTRUCTION_FAILURE_WRONG_BONDS;
        }

        statement->operationBounds = align;
        statement->type = STATEMENT_ALIGN;
        return CONSTRUCTION_SUCCESS;
    }

    if (validateBounds(argOne, argTwo) == BOUND_CREATION_SUCCESS) {
        align.type = OPERATION_IN_BOUNDS;

        align.startIdx = atoi(argOne);
        align.endIdx = atoi(argTwo);
        
        statement->operationBounds = align;
        statement->type = STATEMENT_ALIGN;
        return CONSTRUCTION_SUCCESS;
    }

    return CONSTRUCTION_FAILURE_WRONG_BONDS;
}

ExecuteStatus executeAlign(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);
    
    uint32_t start = statement->operationBounds.startIdx;
    uint32_t end = statement->operationBounds.endIdx;

    uint32_t maxID = getTableMaxID(table);
    if (end != INT_MAX && end > maxID) {
        return EXECUTE_FAILURE_OUT_OF_RANGE;
    }
    end = (end == INT_MAX ? maxID : end);

    Row row;
    Cursor* cursor = tableFind(table, start);

    if (cursor->cellNum < numCells) {
        for (uint32_t i = start; i <= end || !cursor->endOfTable; ++i) {
            deserializeRow(cursorValue(cursor), &row);

            if (row.id != i && row.id <= end) {
                row.id = i;
                *(leafNodeKey(node, cursor->cellNum)) = i;
            }

            serializeRow(&row, leafNodeValue(node, cursor->cellNum));
            cursorAdvance(cursor);
        }
    }

    return EXECUTE_SUCCESS;
}