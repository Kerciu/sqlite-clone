#include "../__input__/statement.h"

StatementStatus constructDrop(Statement* statement) {
    statement->type = STATEMENT_DROP;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDrop(Statement* statement, Table* table) {
    // TODO IMPLEMENT THIS !!
    for (uint32_t i = table->pager->numPages - 1; i >= 0; --i) {
        if (table->pager->pages[i] != NULL) {
            free(table->pager->pages[i]);
            table->pager->pages[i] = NULL;
        }
    }

    table->pager->numPages = 0;
    table->rootPageNum = 0;

    return EXECUTE_SUCCESS;
}