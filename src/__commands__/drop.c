#include "../__input__/statement.h"

StatementStatus constructDrop(InputBuffer* buffer, Statement* statement) {
    statement->type = STATEMENT_DROP;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDrop(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);
    if (numCells < 1) return EXECUTE_SUCCESS_NO_RECORDS;

    uint32_t start = getTableMinID(table);
    uint32_t end = getTableMaxID(table);
    for (uint32_t i = start; i <= end; ++i) {
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

    return EXECUTE_SUCCESS;
}