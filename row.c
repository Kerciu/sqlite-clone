#include "row.h"

void serializeRow(Row *source, void *destination)
{
    memcpy(source + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(source + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(source + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserializeRow(void *source, Row *destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}
