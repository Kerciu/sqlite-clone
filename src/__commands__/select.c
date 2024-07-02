#include "../__input__/statement.h"

StatementStatus constructSelect(InputBuffer* buffer, Statement* statement) {
    statement->type = STATEMENT_SELECT;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeSelect(Statement* statement, Table* table) {
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
