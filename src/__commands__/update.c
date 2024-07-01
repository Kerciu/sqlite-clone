#include "../__input__/statement.h"

StatementStatus constructUpdate(InputBuffer* buffer, Statement* statement) {
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
    if (strlen(email) > EMAIL_MAX_LENGTH) {
        return CONSTRUCTION_FAILURE_TOO_LONG;
    }

    statement->rowToUpdate.id = id;
    strcpy(statement->rowToUpdate.username, username);
    strcpy(statement->rowToUpdate.email, email);

    statement->type = STATEMENT_UPDATE;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeUpdate(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    Row* rowToUpdate = &(statement->rowToUpdate);
    uint32_t keyToUpdate = rowToUpdate->id;
    Cursor* cursor = tableFind(table, keyToUpdate);
    
    if (cursor->cellNum < numCells) {
        uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);

        if (keyAtIdx == keyToUpdate) {
            leafNodeUpdate(cursor, rowToUpdate->id, rowToUpdate);
            free(cursor);
            return EXECUTE_SUCCESS;
        } 
    }

    free(cursor);
    return EXECUTE_NO_ROW_FOUND;
}