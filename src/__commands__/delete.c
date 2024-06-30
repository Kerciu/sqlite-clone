#include "../__input__/statement.h"

StatementStatus constructDelete(Statement* statement) {
    statement->type = STATEMENT_SELECT;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDelete(Statement* statement, Table* table) {
    return EXECUTE_FAILURE;
}