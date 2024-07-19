#ifndef STATEMENT_TOOLS
#define STATEMENT_TOOLS

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "../buffer/input_buffer.h"
#include "../../db/database/database.h"

typedef enum {
    OPERATION_IN_BOUNDS, OPERATION_EVERY_ELEMENT, OPERATION_SINGLE_ELEMENT
} OperationType;

typedef enum {
    BOUND_CREATION_SUCCESS, BOUND_CREATION_FAILURE
} BoundStatus;

typedef struct {
    OperationType type;
    uint32_t startIdx;
    uint32_t endIdx;
} Operation;

typedef enum {
    CONSTRUCTION_SUCCESS, CONSTRUCTION_FAILURE_UNRECOGNIZED, CONSTRUCTION_FAILURE_NEGATIVE_ID,
    CONSTRUCTION_SYNTAX_ERROR, CONSTRUCTION_FAILURE_TOO_LONG, CONSTRUCTION_FAILURE_WRONG_BONDS,
    CONSTRUCTION_FAILURE_NO_FILENAME
} StatementStatus;

typedef enum {
    STATEMENT_INSERT, STATEMENT_SELECT, STATEMENT_UPDATE,
    STATEMENT_DELETE, STATEMENT_DROP, STATEMENT_OPEN_TABLE,
    STATEMENT_HELP
} StatementType;

typedef enum {
    SPECIAL_EXEC_SUCCESS, SPECIAL_EXEC_FAILURE
} SpecialCommandStatus;

typedef enum {
    EXECUTE_SUCCESS, EXECUTE_SUCCESS_NO_RECORDS, EXECUTE_TABLE_FULL,
    EXECUTE_FAILURE, EXECUTE_DUPLICATE_KEY_FOUND, EXECUTE_NO_ROW_FOUND,
    EXECUTE_DROP_FAILURE, EXECUTE_TABLE_CREATION_FAILURE,
    EXECUTE_FAILURE_OUT_OF_RANGE
} ExecuteStatus;

typedef struct {
    StatementType type;
    Row rowToInsert;            // Only to use by insert statement
    Row rowToChange;            // Only to use by update or delete statement
    Operation operationBounds;  // Only to use by select statement
    char* workingFileName;      // Only to use by open and create statement
    char* commandManual;        // Only to use by help statement
} Statement;


BoundStatus validateBound(char* bound);
BoundStatus validateBounds(char* start, char* end);
bool isNumber(char* prompt);

StatementStatus parseLimitCommand(Statement* statement, char* prompt, char* bound);
StatementStatus parseBetweenCommand(Statement* statement, char* betweenString);
StatementStatus parseStarCommand(Statement* statement);
StatementStatus checkCommandRange(Statement* statement, StatementType type, char* prompt);

SpecialCommandStatus executeSpecialCommand(InputBuffer* buffer, Table* table);
StatementStatus constructStatement(InputBuffer* buffer, Statement* statement);
StatementStatus constructInsert(InputBuffer* buffer, Statement* statement);
StatementStatus constructSelect(InputBuffer* buffer, Statement* statement);
StatementStatus constructDelete(InputBuffer* buffer, Statement* statement);
StatementStatus constructUpdate(InputBuffer* buffer, Statement* statement);
StatementStatus constructDrop(InputBuffer* buffer, Statement* statement);
StatementStatus constructOpenTable(InputBuffer* buffer, Statement* statement);
StatementStatus constructHelp(InputBuffer* buffer, Statement* statement);
ExecuteStatus executeStatement(Statement* statement, Table* table);
ExecuteStatus executeInsert(Statement* statement, Table* table);
ExecuteStatus executeSelect(Statement* statement, Table* table);
ExecuteStatus executeDelete(Statement* statement, Table* table);
ExecuteStatus executeUpdate(Statement* statement, Table* table);
ExecuteStatus executeDrop(Statement* statement, Table* table);
ExecuteStatus executeOpenTable(Statement* statement, Table** table);
ExecuteStatus executeHelp(Statement* statement);

#endif