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

    if (strncmp(buffer->buffer, "insert", 6) == 0) {

        return constructInsert(buffer, statement);
    }

    if (strncmp(buffer->buffer, "select", 6) == 0) {

        return constructSelect(statement);
    }

    if (strncmp(buffer->buffer, "update", 5) == 0) {

        return constructUpdate(buffer, statement);
    }

    if (strncmp(buffer->buffer, "delete", 6) == 0) {

        return constructDelete(statement);
    }

    if (strncmp(buffer->buffer, "align", 4) == 0) {
        
        return constructAlign(statement);
    }

    if (strncmp(buffer->buffer, "drop", 4) == 0) {
        return constructDrop(statement);
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
            return executeSort(statement, table);

        default:
            return EXECUTE_FAILURE;

    }
}
