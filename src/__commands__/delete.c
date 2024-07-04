#include "../__input__/statement.h"

bool isNumber(char* prompt) {
    for (uint32_t i = 0; i< strlen(prompt); ++i) {
        if (!isdigit(prompt[i])) return false;
    }
    return true;
}

StatementStatus parseFromCommand(Statement* statement, char* prompt, char* bound) {
    statement->operationBounds.type = OPERATION_STARTING_FROM;
    statement->operationBounds.startIdx = atoi(bound);
    statement->operationBounds.endIdx = INT_MAX;
    return CONSTRUCTION_SUCCESS;
}

StatementStatus parseToCommand(Statement* statement, char* prompt, char* bound) {
    if (!isNumber(bound) || strcmp(prompt, "TO") != 0) return CONSTRUCTION_SYNTAX_ERROR;

    statement->operationBounds.type = OPERATION_END_TO;
    statement->operationBounds.startIdx = 0;
    statement->operationBounds.endIdx = atoi(bound);
    return CONSTRUCTION_SUCCESS;


}

StatementStatus parseFromToCommand(Statement* statement, char* fromString) {
    char* fromCommand = strtok(fromString, NULL);
    char* argOne = strtok(NULL, " ");
    char* toCommand = strtok(NULL, " ");
    char* argTwo = strtok(NULL, " ");

    if (toCommand == NULL || argTwo == NULL) {
        if (!isNumber(argOne)) return CONSTRUCTION_SYNTAX_ERROR;
        return parseFromCommand(statement, fromCommand, argOne);
    } 
    else {
        if (!validateBounds(argOne, argTwo) == BOUND_CREATION_FAILURE || strcmp(toCommand, "TO") != 0) {
            return CONSTRUCTION_SYNTAX_ERROR;
        }
        statement->operationBounds.type = OPERATION_IN_BOUNDS;
        statement->operationBounds.startIdx = atoi(argOne);
        statement->operationBounds.endIdx = atoi(argTwo);
        return CONSTRUCTION_SUCCESS;
        

    }
}

StatementStatus constructDelete(InputBuffer* buffer, Statement* statement) {
    char* command = strtok(buffer->buffer, " ");
    char* prompt = strtok(NULL, "\n");

    if (prompt == NULL) {
        return CONSTRUCTION_SYNTAX_ERROR;
    }

    if (isNumber(prompt)) {
        /* parse command DELETE ## (for single id deletion)*/
        uint32_t id = atoi(prompt);
        if (id < 0 || id > INT_MAX) {
            return CONSTRUCTION_FAILURE_NEGATIVE_ID;
        }

        statement->rowToChange.id = id;
        statement->operationBounds.type = OPERATION_SIGNLE_ELEMENT;
        statement->operationBounds.startIdx = statement->operationBounds.endIdx = id;
        statement->type = STATEMENT_DELETE;
        return CONSTRUCTION_SUCCESS;
    }
    else if (strncmp(prompt, "FROM", 4) == 0) {
        /* parse command DELETE FROM ## or DELETE FROM ## TO ## */
        return parseFromToCommand(statement, prompt);
    }
    else if (strncmp(prompt, "TO", 2) == 0) {
        /* parse command DELETE TO ## */
        char* toPrompt;
        strcpy(toPrompt, prompt);
        char* toCommand = strtok(toPrompt, " ");
        char* toArg = strtok(NULL, " ");
        return parseToCommand(statement, prompt, toArg);
    }
}

ExecuteStatus executeDelete(Statement* statement, Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    Row* rowToDelete = &(statement->rowToChange);
    uint32_t keyToDelete = rowToDelete->id;
    Cursor* cursor = tableFind(table, keyToDelete);

    if (cursor->cellNum < numCells) {
        uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);

        if (keyAtIdx == keyToDelete) {
            leafNodeDelete(cursor);
            free(cursor);
            return EXECUTE_SUCCESS;
        }
        else return EXECUTE_NO_ROW_FOUND;
    }

    return EXECUTE_FAILURE;
}