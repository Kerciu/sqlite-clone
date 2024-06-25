#ifndef DATABASE_ROW
#define DATABASE_ROW

/*
    For now I will store data in pages
    later on I will implement B-Trees
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>

#define sizeOFAttribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define USERNAME_MAX_LENGTH 32
#define EMAIL_MAX_LENGTH 256
#define TABLE_MAX_PAGES 100

typedef struct {
    uint32_t rowNum;
    Pager* pager;
} Table;

typedef struct {
    uint32_t id;
    char username[USERNAME_MAX_LENGTH + 1];
    char email[EMAIL_MAX_LENGTH + 1];
} Row;

typedef struct {
    int fileDescriptor;
    uint32_t fileLength;
    void* pages[TABLE_MAX_PAGES];
} Pager;

extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;
extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;
extern const uint32_t PAGE_SIZE;

extern const uint32_t ROWS_PER_PAGE;
extern const uint32_t TABLE_MAX_ROWS;

void serializeRow(Row* source, void* destination);
void deserializeRow(void* source, Row* destination);
void* reserveRowSlot(Table* table, uint32_t rowNum);

Table* createTable(void);
Table* openDataBase(const char* fileHandle);
void closeDataBase(Table* table);
void pagerFlush(Pager* pager, uint32_t pageNum, uint32_t size);
Pager* openPager(const char* fileHandle);
void displayRow(Row* row);

#endif