#include "../__input__/statement.h"

StatementStatus constructSelect(InputBuffer* buffer, Statement* statement) {
    statement->type = STATEMENT_SELECT;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeSelect(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    if (numCells == 0) {
        return EXECUTE_SUCCESS_NO_RECORDS;
    }

    Cursor* cursor = tableStart(table);

    Row row;
    while (!cursor->endOfTable) {
        deserializeRow(cursorValue(cursor), &row);
        displayRow(&row);
        cursorAdvance(cursor);
    }

    free(cursor);

    return EXECUTE_SUCCESS;
}
