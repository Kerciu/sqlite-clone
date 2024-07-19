#include "../input/statement/statement.h"

StatementStatus constructAlign(InputBuffer* buffer, Statement* statement) {
    char* command = strtok(buffer->buffer, " ");
    char* prompt = strtok(NULL, "\n");

    statement->type = STATEMENT_ALIGN;
    if (prompt == NULL) {
        statement->operationBounds.startIdx = 0;
        statement->operationBounds.endIdx = INT_MAX;
        statement->operationBounds.type = OPERATION_EVERY_ELEMENT;
        return CONSTRUCTION_SUCCESS;
    }

    return checkIfFromToCommand(statement, STATEMENT_ALIGN, prompt);
}

ExecuteStatus executeAlign(Statement* statement, Table* table) {
    uint32_t start = statement->operationBounds.startIdx;
    uint32_t end = statement->operationBounds.endIdx;

    uint32_t maxID = getTableMaxID(table);
    if (end != INT_MAX && end > maxID) {
        return EXECUTE_FAILURE_OUT_OF_RANGE;
    }
    end = (end == INT_MAX ? maxID : end);

    Row row;
    Cursor* cursor = tableFind(table, start);
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t numCells = *leafNodeNumCells(node);

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