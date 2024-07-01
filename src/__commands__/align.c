#include "../__input__/statement.h"

BoundStatus validateBound(char* bound) {
    if (bound == NULL || *bound == '\0') return BOUND_CREATION_FAILURE;

    for (int i = 0; bound[i] != '\0'; ++i) {
        if (!isdigit(bound[i])) {
            return BOUND_CREATION_FAILURE;
        }
    }

    long int intBound = strtol(bound, NULL, 10);
    if (intBound < 0 || intBound > INT_MAX) return BOUND_CREATION_FAILURE;

    return BOUND_CREATION_SUCCESS;
}

BoundStatus validateBounds(char* start, char* end) {
    bool boundsValidated = (validateBound(start) == BOUND_CREATION_SUCCESS &&
                            validateBound(end) == BOUND_CREATION_SUCCESS);

    long int startInt = strtol(start, NULL , 10);
    long int endInt = strtol(end, NULL, 10);
    bool boundRanged = (startInt < endInt);

    return (boundsValidated && boundRanged) ? BOUND_CREATION_SUCCESS : BOUND_CREATION_FAILURE;
}

StatementStatus constructAlign(InputBuffer* buffer, Statement* statement) {
    char* command = strtok(buffer->buffer, " ");
    char* argOne = strtok(NULL, " ");
    char* argTwo = strtok(NULL, " ");

    if (argOne == NULL || argTwo == NULL) {
        return CONSTRUCTION_SYNTAX_ERROR;
    }

    Align align;

    if (strcmp(argOne, "end") == 0) {
        align.type = ALIGN_END_TO;
        
        if (validateBound(argTwo) == BOUND_CREATION_SUCCESS) {
            uint32_t endBound = atoi(argTwo);
            align.endIdx = atoi(argTwo);
        }
        else {
            return CONSTRUCTION_FAILURE_WRONG_BONDS;
        }

        statement->alignBounds = align;
        statement->type = STATEMENT_ALIGN;
        return CONSTRUCTION_SUCCESS;
    }

    if (strcmp(argOne, "start") == 0) {
        align.type = ALIGN_STARTING_FROM;

        if (validateBound(argTwo) == BOUND_CREATION_SUCCESS) {
            uint32_t endBound = atoi(argTwo);
            align.startIdx = atoi(argTwo);
        }
        else {
            return CONSTRUCTION_FAILURE_WRONG_BONDS;
        }

        statement->alignBounds = align;
        statement->type = STATEMENT_ALIGN;
        return CONSTRUCTION_SUCCESS;
    }

    if (validateBounds(argOne, argTwo) == BOUND_CREATION_SUCCESS) {
        align.type = ALIGN_IN_BOUNDS;

        align.startIdx = atoi(argOne);
        align.endIdx = atoi(argTwo);
        
        statement->alignBounds = align;
        statement->type = STATEMENT_ALIGN;
        return CONSTRUCTION_SUCCESS;
    }

    return CONSTRUCTION_FAILURE_WRONG_BONDS;
}

ExecuteStatus executeAlign(Statement* statement, Table* table) {
    
}