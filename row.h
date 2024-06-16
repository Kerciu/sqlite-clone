#ifndef DATABASE_ROW
#define DATABASE_ROW

/*
    For now I will store data in pages
    later on I will implement B-Trees
*/

#include <stdlib.h>
#include <sys/types.h>

#define USERNAME_MAX_LENGTH 32
#define EMAIL_MAX_LENGTH 256
#define sizeOFAttribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

typedef struct {
    u_int32_t id;
    char username[USERNAME_MAX_LENGTH];
    char email[EMAIL_MAX_LENGTH];
} Row;

const u_int32_t ID_SIZE = sizeOFAttribute(Row, id);
const u_int32_t USERNAME_SIZE = sizeOFAttribute(Row, username);
const u_int32_t EMAIL_SIZE = sizeOFAttribute(Row, email);
const u_int32_t ID_OFFSET = 0;
const u_int32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const u_int32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const u_int32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void serializeRow(Row* source, void* destination);
void deserializeRow(Row* source, void* destination);

#endif