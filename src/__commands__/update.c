#include "../__input__/statement.h"

StatementStatus constructUpdate(Statement* statement) {
    statement->type = STATEMENT_SELECT;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeUpdate(Statement* statement, Table* table) {
    return EXECUTE_FAILURE;
}