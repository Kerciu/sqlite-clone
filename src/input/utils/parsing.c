#include "../statement/statement.h"

bool isNumber(char* prompt) {
    for (uint32_t i = 0; i< strlen(prompt); ++i) {
        if (!isdigit(prompt[i])) return false;
    }
    return true;
}

StatementStatus parseLimitCommand(Statement* statement, char* prompt, char* bound) {
    if (!isNumber(bound) || strcmp(prompt, "LIMIT") != 0) return CONSTRUCTION_SYNTAX_ERROR;

    statement->operationBounds.type = OPERATION_IN_BOUNDS;
    statement->operationBounds.startIdx = 0;
    statement->operationBounds.endIdx = atoi(bound);
    return CONSTRUCTION_SUCCESS;
}

StatementStatus parseBetweenCommand(Statement* statement, char* betweenString) {
    char* betweenCommand = strtok(betweenString, " ");
    if (betweenCommand == NULL) return CONSTRUCTION_SYNTAX_ERROR;
    char* argOne = strtok(NULL, " ");
    if (argOne == NULL) return CONSTRUCTION_SYNTAX_ERROR;
    char* andCommand = strtok(NULL, " ");
    char* argTwo = strtok(NULL, " ");

    if (andCommand == NULL || argTwo == NULL) {
        return CONSTRUCTION_SYNTAX_ERROR;
    } 
    else {
        BoundStatus boundsStatus = validateBounds(argOne, argTwo);
        if (boundsStatus == BOUND_CREATION_FAILURE) {
            return CONSTRUCTION_FAILURE_WRONG_BONDS;
        } 
        if (strcmp(andCommand, "AND") != 0) {
            return CONSTRUCTION_FAILURE_WRONG_BONDS;
        }
        /* handle COMMAND BETWEEN ## AND ## situation*/
        statement->operationBounds.type = OPERATION_IN_BOUNDS;
        statement->operationBounds.startIdx = atoi(argOne);
        statement->operationBounds.endIdx = atoi(argTwo);
        return CONSTRUCTION_SUCCESS;
    }
}

StatementStatus parseStarCommand(Statement* statement) {
    statement->operationBounds.startIdx = 0;
    statement->operationBounds.endIdx = INT_MAX;
    statement->operationBounds.type = OPERATION_EVERY_ELEMENT;
    return CONSTRUCTION_SUCCESS;    
}

StatementStatus checkCommandRange(Statement* statement, StatementType type, char* prompt) {
    statement->type = type;

    if (isNumber(prompt)) {
        /* parse command COMMAND ## (for single id)*/
        uint32_t id = atoi(prompt);
        if (id < 0 || id > INT_MAX) {
            return CONSTRUCTION_FAILURE_NEGATIVE_ID;
        }

        statement->rowToChange.id = id;
        statement->operationBounds.type = OPERATION_SINGLE_ELEMENT;
        statement->operationBounds.startIdx = statement->operationBounds.endIdx = id;

        return CONSTRUCTION_SUCCESS;
    }
    else if (strncmp(prompt, "BETWEEN", 4) == 0) {
        /* parse command COMMAND BETWEEN ## AND ## */
        
        return parseBetweenCommand(statement, prompt);
    }
    else if (strncmp(prompt, "LIMIT", 2) == 0) {
        /* parse command COMMAND LIMIT ## */
        char limitPrompt[256];  // allocate sufficient space for toPrompt
        strcpy(limitPrompt, prompt);
        char* limitCommand = strtok(limitPrompt, " ");
        if (limitCommand == NULL) return CONSTRUCTION_SYNTAX_ERROR;
        char* toArg = strtok(NULL, " ");
        if (toArg == NULL) return CONSTRUCTION_SYNTAX_ERROR;
        
        return parseLimitCommand(statement, limitPrompt, toArg);
    }
    else if (strncmp(prompt, "*", 1) == 0) {
        return parseStarCommand(statement);
    }
    else {
        return CONSTRUCTION_SYNTAX_ERROR;
    }
}