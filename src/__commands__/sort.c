#include "../__input__/statement.h"

StatementStatus constructSort(Statement* statement) {
    statement->type = STATEMENT_SELECT;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeSort(Statement* statement, Table* table) {
    return EXECUTE_FAILURE;
}