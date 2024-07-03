#include "../__input__/statement.h"

StatementStatus constructDrop(InputBuffer* buffer, Statement* statement) {
    statement->type = STATEMENT_DROP;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDrop(Statement* statement, Table* table) {
    closeDataBase(table);
    return (remove(table->fileHandle) == 0 ? EXECUTE_SUCCESS : EXECUTE_DROP_FAILURE);
}