#include "../__input__/statement.h"

StatementStatus constructDrop(Statement* statement) {
    statement->type = STATEMENT_DROP;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDrop(Statement* statement, Table* table) {
    return EXECUTE_FAILURE;
}