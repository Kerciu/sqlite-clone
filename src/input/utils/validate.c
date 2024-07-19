#include "../statement/statement.h"

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
