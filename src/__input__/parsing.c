#include "statement.h"

bool isNumber(char* prompt) {
    for (uint32_t i = 0; i< strlen(prompt); ++i) {
        if (!isdigit(prompt[i])) return false;
    }
    return true;
}

StatementStatus parseFromCommand(Statement* statement, char* prompt, char* bound) {
    printf("Inside parseFromCommand\n");
    if (!isNumber(bound) || strcmp(prompt, "FROM") != 0) return CONSTRUCTION_SYNTAX_ERROR;

    statement->operationBounds.type = OPERATION_STARTING_FROM;
    statement->operationBounds.startIdx = atoi(bound);
    statement->operationBounds.endIdx = INT_MAX;
    printf("parseFrom:startidx:%d , endIdx:%d\n", statement->operationBounds.startIdx, statement->operationBounds.endIdx);
    return CONSTRUCTION_SUCCESS;
}

StatementStatus parseToCommand(Statement* statement, char* prompt, char* bound) {
    printf("inside parseToCommand: bound: %s, prompt: %s\n", bound, prompt);
    if (!isNumber(bound) || strcmp(prompt, "TO") != 0) return CONSTRUCTION_SYNTAX_ERROR;

    statement->operationBounds.type = OPERATION_END_TO;
    statement->operationBounds.startIdx = 0;
    statement->operationBounds.endIdx = atoi(bound);
    return CONSTRUCTION_SUCCESS;
}

StatementStatus parseFromToCommand(Statement* statement, char* fromString) {
    printf("fromString:%s\n", fromString);
    char* fromCommand = strtok(fromString, " ");
    printf("fromCommand:%s\n", fromCommand);
    if (fromCommand == NULL) return CONSTRUCTION_SYNTAX_ERROR;
    char* argOne = strtok(NULL, " ");
    if (argOne == NULL) return CONSTRUCTION_SYNTAX_ERROR;
    char* toCommand = strtok(NULL, " ");
    char* argTwo = strtok(NULL, " ");

    printf("argOne: %s, toCommand: %s, argTwo: %s\n", argOne, toCommand, argTwo);
    if (toCommand == NULL || argTwo == NULL) {
        printf("handle DELETE FROM ## situation\n");
        /* handle DELETE FROM ## situation */
        if (!isNumber(argOne)) return CONSTRUCTION_SYNTAX_ERROR;
        return parseFromCommand(statement, fromCommand, argOne);
    } 
    else {
        BoundStatus boundsStatus = validateBounds(argOne, argTwo);
        if (boundsStatus == BOUND_CREATION_FAILURE) {
            printf("Error: Invalid bounds provided: %s -> %s\n", argOne, argTwo);
            return CONSTRUCTION_FAILURE_WRONG_BONDS;
        } 
        if (strcmp(toCommand, "TO") != 0) {
            printf("Error: cannot recognize TO command: %s\n", toCommand);
            printf("Why are you in this block? argOne: %s, toCommand: %s, argTwo: %s\n", argOne, toCommand, argTwo);
            return CONSTRUCTION_FAILURE_WRONG_BONDS;
        }
        /* handle DELETE FROM ## TO ## situation*/
        printf("handle DELETE FROM ## TO ## situation\n");
        statement->operationBounds.type = OPERATION_IN_BOUNDS;
        statement->operationBounds.startIdx = atoi(argOne);
        statement->operationBounds.endIdx = atoi(argTwo);
        return CONSTRUCTION_SUCCESS;
    }
}

StatementStatus checkIfFromToCommand(Statement* statement, StatementType type, char* prompt) {
    if (isNumber(prompt)) {
        /* parse command DELETE ## (for single id deletion)*/
        uint32_t id = atoi(prompt);
        if (id < 0 || id > INT_MAX) {
            return CONSTRUCTION_FAILURE_NEGATIVE_ID;
        }

        statement->rowToChange.id = id;
        statement->operationBounds.type = OPERATION_SINGLE_ELEMENT;
        statement->operationBounds.startIdx = statement->operationBounds.endIdx = id;
        statement->type = type;

        printf("Start idx: %d, End idx: %d\n", statement->operationBounds.startIdx, statement->operationBounds.endIdx); // DEBUG
        return CONSTRUCTION_SUCCESS;
    }
    else if (strncmp(prompt, "FROM", 4) == 0) {
        /* parse command DELETE FROM ## or DELETE FROM ## TO ## */
        printf("Recognized FROM statment, prompt: %s\n", prompt);
        statement->type = type;
        return parseFromToCommand(statement, prompt);
    }
    else if (strncmp(prompt, "TO", 2) == 0) {
        /* parse command DELETE TO ## */
        printf("Recognized TO statement, prompt: %s\n", prompt);
        char toPrompt[256];  // allocate sufficient space for toPrompt
        strcpy(toPrompt, prompt);
        char* toCommand = strtok(toPrompt, " ");
        if (toCommand == NULL) return CONSTRUCTION_SYNTAX_ERROR;
        char* toArg = strtok(NULL, " ");
        if (toArg == NULL) return CONSTRUCTION_SYNTAX_ERROR;
        statement->type = type;
        return parseToCommand(statement, toPrompt, toArg);
    }
    else return CONSTRUCTION_SYNTAX_ERROR;
}