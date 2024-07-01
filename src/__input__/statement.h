#ifndef STATEMENT_TOOLS
#define STATEMENT_TOOLS

#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "../db/database.h"

typedef enum {
    CONSTRUCTION_SUCCESS, CONSTRUCTION_FAILURE_UNRECOGNIZED, CONSTRUCTION_FAILURE_NEGATIVE_ID,
    CONSTRUCTION_SYNTAX_ERROR, CONSTRUCTION_FAILURE_TOO_LONG
} StatementStatus;

typedef enum {
    STATEMENT_INSERT, STATEMENT_SELECT, STATEMENT_UPDATE, STATEMENT_SORT, STATEMENT_DELETE, STATEMENT_DROP
} StatementType;

typedef enum {
    SPECIAL_EXEC_SUCCESS, SPECIAL_EXEC_FAILURE
} SpecialCommandStatus;

typedef enum {
    EXECUTE_SUCCESS, EXECUTE_TABLE_FULL, EXECUTE_FAILURE, EXECUTE_DUPLICATE_KEY_FOUND, EXECUTE_NO_ROW_FOUND
} ExecuteStatus;

typedef struct {
    StatementType type;
    Row rowToInsert;            // Only to use by insert statement
    Row rowToUpdate;            // Only to use by update statement
} Statement;

SpecialCommandStatus executeSpecialCommand(InputBuffer* buffer, Table* table);
StatementStatus constructStatement(InputBuffer* buffer, Statement* statement);
StatementStatus constructInsert(InputBuffer* buffer, Statement* statement);
StatementStatus constructSelect(Statement* statement);
StatementStatus constructDelete(Statement* statement);
StatementStatus constructUpdate(InputBuffer* buffer, Statement* statement);
StatementStatus constructSort(Statement* statement);
StatementStatus constructDrop(Statement* statement);
ExecuteStatus executeStatement(Statement* statement, Table* table);
ExecuteStatus executeInsert(Statement* statement, Table* table);
ExecuteStatus executeSelect(Statement* statement, Table* table);
ExecuteStatus executeDelete(Statement* statement, Table* table);
ExecuteStatus executeUpdate(Statement* statement, Table* table);
ExecuteStatus executeSort(Statement* statement, Table* table);
ExecuteStatus executeDrop(Statement* statement, Table* table);

#endif