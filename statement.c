#include "statement.h"

SpecialCommandStatus executeSpecialCommand(InputBuf* buffer, Table* table) {

    if (buffer->buffer == NULL)
        return SPECIAL_EXEC_FAILURE;

    if (strcmp(buffer->buffer, ".exit") == 0) {
        closeDataBase(table);
        exit(EXIT_SUCCESS);
    }

    else return SPECIAL_EXEC_FAILURE;
}

StatementStatus constructInsert(InputBuf* buffer, Statement* statement) {
    // int args = sscanf(buffer->buffer, "insert %d %31s %255s", &(statement->rowToInsert.id),
    //             statement->rowToInsert.username, statement->rowToInsert.email);
    // if (args < 3) return CONSTRUCTION_SYNTAX_ERROR;

    char* command = strtok(buffer->buffer, " ");
    char* idPrompt = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (idPrompt == NULL || username == NULL || email == NULL) {
        return CONSTRUCTION_SYNTAX_ERROR;
    }

    int id = atoi(idPrompt);
    if (id < 0) {
        return CONSTRUCTION_FAILURE_NEGATIVE_ID;
    }
    if (strlen(username) > USERNAME_MAX_LENGTH) {
        return CONSTRUCTION_FAILURE_TOO_LONG;
    }
    if (strlen(username) > USERNAME_MAX_LENGTH) {
        return CONSTRUCTION_FAILURE_TOO_LONG;
    }

    statement->rowToInsert.id = id;
    strcpy(statement->rowToInsert.username, username);
    strcpy(statement->rowToInsert.email, email);

    statement->type = STATEMENT_INSERT;
    return CONSTRUCTION_SUCCESS;
}

StatementStatus constructSelect(Statement* statement) {
    statement->type = STATEMENT_SELECT;
    return CONSTRUCTION_SUCCESS;
}


StatementStatus constructStatement(InputBuf* buffer, Statement* statement) {

    if (strncmp(buffer->buffer, "insert", 6) == 0) {

        return constructInsert(buffer, statement);
    }

    if (strncmp(buffer->buffer, "select", 6) == 0) {

        return constructSelect(statement);
    }

    return CONSTRUCTION_FAILURE_UNRECOGNIZED;
}

ExecuteStatus executeStatement(Statement* statement, Table* table) {

    switch(statement->type) {
        case STATEMENT_INSERT:
            return executeInsert(statement, table);

        case STATEMENT_SELECT:
            return executeSelect(statement, table);

        default:
            return EXECUTE_FAILURE;

    }
}

ExecuteStatus executeInsert(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    if (*leafNodeNumCells(node) >= LEAF_NODE_MAX_CELLS) {
        return EXECUTE_TABLE_FULL;
    }

    Row* rowToInsert = &(statement->rowToInsert);
    Cursor* cursor = tableEnd(table);

    leafNodeInsert(cursor, rowToInsert->id, rowToInsert);

    free(cursor);

    return EXECUTE_SUCCESS;

}

ExecuteStatus executeSelect(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);

    Row row;
    while (!cursor->endOfTable) {
        deserializeRow(cursorValue(cursor), &row);
        displayRow(&row);
        cursorAdvance(cursor);
    }

    free(cursor);

    return EXECUTE_SUCCESS;
}
