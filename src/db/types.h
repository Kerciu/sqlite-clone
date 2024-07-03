#ifndef DATABASE_ESSENTIAL_TYPES
#define DATABASE_ESSENTIAL_TYPES

#include <stdint.h>

#define sizeOFAttribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define USERNAME_MAX_LENGTH 32
#define EMAIL_MAX_LENGTH 256
#define TABLE_MAX_PAGES 100

typedef enum {
    ALIGN_IN_BOUNDS, ALIGN_STARTING_FROM, ALIGN_END_TO
} AlignType;

typedef enum {
    BOUND_CREATION_SUCCESS, BOUND_CREATION_FAILURE
} BoundStatus;

typedef struct {
    uint32_t id;
    char username[USERNAME_MAX_LENGTH + 1];
    char email[EMAIL_MAX_LENGTH + 1];
} Row;

typedef struct {
    int fileDescriptor;
    uint32_t fileLength;
    uint32_t numPages;
    void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
    char* fileHandle;
    uint32_t rootPageNum;
    Pager* pager;
} Table;

typedef struct {
    // represents location in a table
    Table* table;
    uint32_t pageNum;
    uint32_t cellNum;
    bool endOfTable;    // indicates position one past the last elem
} Cursor;

typedef struct {
    AlignType type;
    uint32_t startIdx;
    uint32_t endIdx;
} Align;

#endif