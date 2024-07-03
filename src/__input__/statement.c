#include "statement.h"

SpecialCommandStatus executeSpecialCommand(InputBuffer* buffer, Table* table) {

    if (buffer->buffer == NULL)
        return SPECIAL_EXEC_FAILURE;

    if (strcmp(buffer->buffer, ".exit") == 0) {
        closeDataBase(table);
        exit(EXIT_SUCCESS);
    }

    else if (strcmp(buffer->buffer, ".constants") == 0) {
        printf("Constants:\n");
        printConstants();
        return SPECIAL_EXEC_SUCCESS;
    }

    else if (strcmp(buffer->buffer, ".btree") == 0) {
        printf("B-Tree:\n");
        printTree(table->pager, 0, 0);
        return SPECIAL_EXEC_SUCCESS;
    }

    else return SPECIAL_EXEC_FAILURE;
}

StatementStatus constructStatement(InputBuffer* buffer, Statement* statement) {

    if (strncmp(buffer->buffer, "INSERT", 6) == 0) {

        return constructInsert(buffer, statement);
    }

    if (strncmp(buffer->buffer, "SELECT", 6) == 0) {

        return constructSelect(buffer, statement);
    }

    if (strncmp(buffer->buffer, "UPDATE", 5) == 0) {

        return constructUpdate(buffer, statement);
    }

    if (strncmp(buffer->buffer, "DELETE", 6) == 0) {

        return constructDelete(buffer, statement);
    }

    if (strncmp(buffer->buffer, "ALIGN", 4) == 0) {
        
        return constructAlign(buffer, statement);
    }

    if (strncmp(buffer->buffer, "DROP", 4) == 0) {
        return constructDrop(statement);
    }

    if (strncmp(buffer->buffer, "OPEN TABLE", 10) == 0) {
        return constructOpenTable(buffer, statement);
    }

    return CONSTRUCTION_FAILURE_UNRECOGNIZED;
}

ExecuteStatus executeStatement(Statement* statement, Table* table) {

    switch(statement->type) {
        case STATEMENT_INSERT:
            return executeInsert(statement, table);

        case STATEMENT_SELECT:
            return executeSelect(statement, table);

        case STATEMENT_UPDATE:
            return executeUpdate(statement, table);
        
        case STATEMENT_DELETE:
            return executeDelete(statement, table);
        
        case STATEMENT_ALIGN:
            return executeAlign(statement, table);
        
        case STATEMENT_DROP:
            return executeDrop(statement, table);

        case STATEMENT_OPEN_TABLE:
            return executeOpenTable(statement, &table);

        default:
            return EXECUTE_FAILURE;

    }
}
