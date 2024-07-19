#include "../input/statement/statement.h"

StatementStatus constructOpenTable(InputBuffer* buffer, Statement* statement) {
    char* command = strtok(buffer->buffer, " ");
    char* tableKeyWord = strtok(NULL, " ");
    if (tableKeyWord == NULL || strcmp(tableKeyWord, "TABLE") != 0) {
        return CONSTRUCTION_SYNTAX_ERROR;
    }
    char* fileHandle = strtok(NULL, " ");
    if (fileHandle == NULL) {
        return CONSTRUCTION_FAILURE_NO_FILENAME;
    }

    statement->workingFileName = fileHandle;
    statement->type = STATEMENT_OPEN_TABLE;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeOpenTable(Statement* statement, Table** table) {
    /* TODO make this switch databases while using program */
    *table = openDataBase(statement->workingFileName);
    if (*table == NULL) {
        return EXECUTE_TABLE_CREATION_FAILURE;
    }
    
    return EXECUTE_SUCCESS;
}
