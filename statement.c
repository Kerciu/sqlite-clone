#include "statement.h"

SpecialCommandStatus executeSpecialCommand(InputBuf* buffer) {

    if (buffer->buffer == NULL)
        return SPECIAL_EXEC_FAILURE;

    if (strcmp(buffer->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    }

    else return SPECIAL_EXEC_FAILURE;
}

StatementStatus constructStatement(InputBuf* buffer, Statement* statement) {

    if (strncmp(buffer->buffer, "insert", 6) == 0) {

        int args = sscanf(buffer->buffer, "insert %d %31s %255s", &(statement->rowToInsert.id),
                statement->rowToInsert.username, statement->rowToInsert.email);
        if (args < 3) return CONSTRUCTION_SYNTAX_ERROR;

        statement->type = STATEMENT_INSERT;
        return CONSTRUCTION_SUCCESS;
    }

    if (strncmp(buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return CONSTRUCTION_SUCCESS;
    }

    return CONSTRUCTION_FAILURE_UNRECOGNIZED;
}

ExecuteStatus executeStatement(Statement* statement, Table* table) {

    switch(statement->type) {
        case STATEMENT_INSERT:
            return executeInsert(statement, table);

        case STATEMENT_SELECT:
            return executeSelect(statement, table);
    }
}

ExecuteStatus executeInsert(Statement* statement, Table* table) {
    if (table->rowNum >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FAILURE;
    }

    Row* rowToInsert = &(statement->rowToInsert);
    serializeRow(rowToInsert, reserveRowSlot(table, table->rowNum));
    ++(table->rowNum);

    return EXECUTE_SUCCESS;

}

ExecuteStatus executeSelect(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->rowNum; ++i) {
        deserializeRow(reserveRowSlot(table, i), &row);
        displayRow(&row);
    }

    return EXECUTE_SUCCESS;
}
