#include "../__input__/statement.h"

StatementStatus constructOpen(InputBuffer* buffer, Statement* statement) {
    statement->type = STATEMENT_OPEN;
    return CONSTRUCTION_FAILURE_UNRECOGNIZED;
}

ExecuteStatus executeOpen(Statement* statement, Table* table) {
    
    return EXECUTE_FAILURE;
}
