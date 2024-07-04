#include "statement.h"

BoundStatus validateBound(char* bound) {
    if (bound == NULL || *bound == '\0') return BOUND_CREATION_FAILURE;

    for (int i = 0; bound[i] != '\0'; ++i) {
        if (!isdigit(bound[i])) {
            return BOUND_CREATION_FAILURE;
        }
    }

    long int intBound = strtol(bound, NULL, 10);
    if (intBound < 0 || intBound > INT_MAX) return BOUND_CREATION_FAILURE;

    return BOUND_CREATION_SUCCESS;
}

BoundStatus validateBounds(char* start, char* end) {
    bool boundsValidated = (validateBound(start) == BOUND_CREATION_SUCCESS &&
                            validateBound(end) == BOUND_CREATION_SUCCESS);

    long int startInt = strtol(start, NULL , 10);
    long int endInt = strtol(end, NULL, 10);
    bool boundRanged = (startInt < endInt);

    return (boundsValidated && boundRanged) ? BOUND_CREATION_SUCCESS : BOUND_CREATION_FAILURE;
}

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

    else if (strcmp(buffer->buffer, ".maxid") == 0) {
        printf("Table max ID: %d\n", getTableMaxID(table));
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
        return constructDrop(buffer, statement);
    }

    if (strncmp(buffer->buffer, "OPEN TABLE", 10) == 0) {
        return constructOpenTable(buffer, statement);
    }

    if (strncmp(buffer->buffer, "HELP", 4) == 0) {
        return constructHelp(buffer, statement);
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
        
        case STATEMENT_HELP:
            return executeHelp(statement);

        default:
            return EXECUTE_FAILURE;

    }
}
