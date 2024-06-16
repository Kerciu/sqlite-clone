#ifndef STATEMENT_TOOLS
#define STATEMENT_TOOLS

#include <stdlib.h>
#include <string.h>
#include "inputbuf.h"
#include "row.h"

typedef enum {
    CONSTRUCTION_SUCCESS, CONSTRUCTION_FAILURE_UNRECOGNIZED, CONSTRUCTION_FAILURE, CONSTRUCTION_SYNTAX_ERROR
} StatementStatus;

typedef enum {
    STATEMENT_INSERT, STATEMENT_SELECT
} StatementType;

typedef enum {
    SPECIAL_EXEC_SUCCESS, SPECIAL_EXEC_FAILURE
} SpecialCommandStatus;

typedef struct {
    StatementType type;
    Row rowToInsert;            // Only to use by insert statement
} Statement;

SpecialCommandStatus executeSpecialCommand(InputBuf* buffer);
StatementStatus constructStatement(InputBuf* buffer, Statement* statement);
void executeStatement(Statement* statement);

#endif