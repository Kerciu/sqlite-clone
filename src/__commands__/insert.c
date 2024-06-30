#include "../__input__/statement.h"

StatementStatus constructInsert(InputBuffer* buffer, Statement* statement) {
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

ExecuteStatus executeInsert(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = (*leafNodeNumCells(node));

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

    return EXECUTE_SUCCESS;

}
