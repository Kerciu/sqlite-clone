#ifndef STATEMENT_TOOLS
#define STATEMENT_TOOLS

#include <stdlib.h>
#include <string.h>
#include "inputbuf.h"
#include "pager.h"

typedef enum {
    CONSTRUCTION_SUCCESS, CONSTRUCTION_FAILURE_UNRECOGNIZED, CONSTRUCTION_FAILURE_NEGATIVE_ID,
    CONSTRUCTION_SYNTAX_ERROR, CONSTRUCTION_FAILURE_TOO_LONG
} StatementStatus;

typedef enum {
    STATEMENT_INSERT, STATEMENT_SELECT
} StatementType;

typedef enum {
    SPECIAL_EXEC_SUCCESS, SPECIAL_EXEC_FAILURE
} SpecialCommandStatus;

typedef enum {
    EXECUTE_SUCCESS, EXECUTE_TABLE_FULL, EXECUTE_FAILURE
} ExecuteStatus;

typedef struct {
    StatementType type;
    Row rowToInsert;            // Only to use by insert statement
} Statement;

SpecialCommandStatus executeSpecialCommand(InputBuf* buffer, Table* table);
StatementStatus constructStatement(InputBuf* buffer, Statement* statement);
ExecuteStatus executeStatement(Statement* statement, Table* table);
ExecuteStatus executeInsert(Statement* statement, Table* table);
ExecuteStatus executeSelect(Statement* statement, Table* table);

#endif