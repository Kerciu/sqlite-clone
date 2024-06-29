#include "statement.h"

SpecialCommandStatus executeSpecialCommand(InputBuf* buffer, Table* table) {

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
    uint32_t numCells = (*leafNodeNumCells(node));
    printf("executeInsert: numCells before insert=%d\n", numCells); // Debug

    Row* rowToInsert = &(statement->rowToInsert); 
    uint32_t keyToInsert = rowToInsert->id;
    Cursor* cursor = tableFind(table, keyToInsert);

    if (cursor->cellNum < numCells) {
        uint32_t keyAtIndex = *leafNodeKey(node, cursor->cellNum);
        if (keyAtIndex == keyToInsert) {
            return EXECUTE_DUPLICATE_KEY_FOUND;
        }
    }
    
    leafNodeInsert(cursor, rowToInsert->id, rowToInsert);
    free(cursor);

    numCells = *leafNodeNumCells(node);
    printf("executeInsert: numCells after insert=%d\n", numCells); // Debug

    return EXECUTE_SUCCESS;

}

ExecuteStatus executeSelect(Statement* statement, Table* table) {
    Cursor* cursor = tableStart(table);

    Row row;
    while (!cursor->endOfTable) {
        deserializeRow(cursorValue(cursor), &row);
        printf("executeSelect: cursor->cellNum=%d, row id=%d, username=%s, email=%s\n", 
               cursor->cellNum, row.id, row.username, row.email); // Debug
        displayRow(&row);
        cursorAdvance(cursor);
    }

    free(cursor);

    return EXECUTE_SUCCESS;
}
