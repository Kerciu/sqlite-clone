#ifndef STATEMENT_TOOLS
#define STATEMENT_TOOLS

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "input.h"
#include "../db/database.h"

typedef enum {
    CONSTRUCTION_SUCCESS, CONSTRUCTION_FAILURE_UNRECOGNIZED, CONSTRUCTION_FAILURE_NEGATIVE_ID,
    CONSTRUCTION_SYNTAX_ERROR, CONSTRUCTION_FAILURE_TOO_LONG, CONSTRUCTION_FAILURE_WRONG_BONDS
} StatementStatus;

typedef enum {
    STATEMENT_INSERT, STATEMENT_SELECT, STATEMENT_UPDATE, STATEMENT_ALIGN, STATEMENT_DELETE, STATEMENT_DROP, STATEMENT_CREATE_TABLE, STATEMENT_OPEN
} StatementType;

typedef enum {
    SPECIAL_EXEC_SUCCESS, SPECIAL_EXEC_FAILURE
} SpecialCommandStatus;

typedef enum {
    EXECUTE_SUCCESS, EXECUTE_TABLE_FULL, EXECUTE_FAILURE, EXECUTE_DUPLICATE_KEY_FOUND, EXECUTE_NO_ROW_FOUND, EXECUTE_DROP_FAILURE
} ExecuteStatus;

typedef struct {
    StatementType type;
    Row rowToInsert;            // Only to use by insert statement
    Row rowToChange;            // Only to use by update or delete statement
    Align alignBounds;          // Only to use by align statement
} Statement;

SpecialCommandStatus executeSpecialCommand(InputBuffer* buffer, Table* table);
StatementStatus constructStatement(InputBuffer* buffer, Statement* statement);
StatementStatus constructInsert(InputBuffer* buffer, Statement* statement);
StatementStatus constructSelect(InputBuffer* buffer, Statement* statement);
StatementStatus constructDelete(InputBuffer* buffer, Statement* statement);
StatementStatus constructUpdate(InputBuffer* buffer, Statement* statement);
StatementStatus constructAlign(InputBuffer* buffer, Statement* statement);
StatementStatus constructDrop(Statement* statement);
StatementStatus constructCreateTable(InputBuffer* buffer, Statement* statement);
StatementStatus constructOpen(InputBuffer* buffer, Statement* statement);
ExecuteStatus executeStatement(Statement* statement, Table* table);
ExecuteStatus executeInsert(Statement* statement, Table* table);
ExecuteStatus executeSelect(Statement* statement, Table* table);
ExecuteStatus executeDelete(Statement* statement, Table* table);
ExecuteStatus executeUpdate(Statement* statement, Table* table);
ExecuteStatus executeAlign(Statement* statement, Table* table);
ExecuteStatus executeDrop(Statement* statement, Table* table);
ExecuteStatus executeCreateTable(Statement* statement, Table* table);
ExecuteStatus executeOpen(Statement* statement, Table* table);

#endif