#include "../__input__/statement.h"

StatementStatus constructCreateTable(InputBuffer* buffer, Statement* statement) {
    statement->type = STATEMENT_CREATE_TABLE;
    return CONSTRUCTION_FAILURE_UNRECOGNIZED;
}

ExecuteStatus executeCreateTable(Statement* statement, Table* table) {
    
    return EXECUTE_FAILURE;
}
