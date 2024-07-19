#include "../input/statement/statement.h"

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
    Cursor* cursor = tableStart(table);

    Row row;
    uint32_t start = statement->operationBounds.startIdx;
    uint32_t end = statement->operationBounds.endIdx;
    while (!cursor->endOfTable) {
        deserializeRow(cursorValue(cursor), &row);
        if (row.id >= start && row.id <= end) {
            displayRow(&row);
            printf("Cellnum: %d\n", cursor->cellNum);
        }
        cursorAdvance(cursor);
    }

    free(cursor);
    return EXECUTE_SUCCESS;
}
