#include "../input/statement/statement.h"

ExecuteStatus clearDatabasContents(char* fileHandle) {
    int fileDescriptor = open(fileHandle, O_WRONLY | O_TRUNC);
    if (fileDescriptor == -1) {
        fprintf(stderr, "Unable to open file for truncation: %d\n", errno);
        return EXECUTE_FAILURE;
    }

    if (close(fileDescriptor) == -1) {
        fprintf(stderr, "Error closing file after truncation: %d\n", errno);
        return EXECUTE_FAILURE;
    }
}

StatementStatus constructDrop(InputBuffer* buffer, Statement* statement) {
    statement->type = STATEMENT_DROP;
    return CONSTRUCTION_SUCCESS;
}

ExecuteStatus executeDrop(Statement* statement, Table* table) {
    clearDatabasContents(table->fileHandle);
    table->rootPageNum = 0;
    table->pager->numPages = 0;

    Table* newTable = openDataBase(table->fileHandle);
    if (newTable == NULL) {
        fprintf(stderr, "Failed to reinitialize the database\n");
        return EXECUTE_FAILURE;
    }

    closeDataBase(table);
    *table = *newTable;
    free(newTable);

    return EXECUTE_SUCCESS;
}