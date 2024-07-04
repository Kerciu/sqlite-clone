#include "../__input__/statement.h"

StatementStatus constructDrop(InputBuffer* buffer, Statement* statement) {
    statement->type = STATEMENT_DROP;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDrop(Statement* statement, Table* table) {
    clearDataBase(table);
    return EXECUTE_SUCCESS;
}