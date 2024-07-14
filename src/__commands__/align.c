#include "../__input__/statement.h"

StatementStatus constructAlign(InputBuffer* buffer, Statement* statement) {
    char* command = strtok(buffer->buffer, " ");
    char* prompt = strtok(NULL, "\n");

    if (prompt == NULL) {
        statement->operationBounds.startIdx = 0;
        statement->operationBounds.endIdx = INT_MAX;
        statement->operationBounds.type = OPERATION_IN_BOUNDS;
        statement->type = STATEMENT_ALIGN;
        return CONSTRUCTION_SUCCESS;
    }

    return checkIfFromToCommand(statement, STATEMENT_ALIGN, prompt);
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