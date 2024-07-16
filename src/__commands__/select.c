#include "../__input__/statement.h"

StatementStatus constructSelect(InputBuffer* buffer, Statement* statement) {
    char* command = strtok(buffer->buffer, " ");
    char* prompt = strtok(NULL, "\n");

    if (prompt == NULL) {
        statement->operationBounds.startIdx = 0;
        statement->operationBounds.endIdx = INT_MAX;
        statement->operationBounds.type = OPERATION_EVERY_ELEMENT;
        statement->type = STATEMENT_SELECT;
        return CONSTRUCTION_SUCCESS;
    }

    return checkIfFromToCommand(statement, STATEMENT_SELECT, prompt);
}

ExecuteStatus executeSelect(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    if (numCells == 0) {
        return EXECUTE_SUCCESS_NO_RECORDS;
    }

    Cursor* cursor = tableStart(table);

    Row row;
    uint32_t start = statement->operationBounds.startIdx;
    uint32_t end = statement->operationBounds.endIdx;
    while (!cursor->endOfTable) {
        deserializeRow(cursorValue(cursor), &row);
        if (row.id >= start && row.id <= end) {
            displayRow(&row);
        }
        cursorAdvance(cursor);
    }

    free(cursor);

    return EXECUTE_SUCCESS;
}
