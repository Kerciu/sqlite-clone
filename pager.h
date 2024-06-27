#ifndef DATABASE_ROW
#define DATABASE_ROW

/*
    For now I will store data in pages
    later on I will implement B-Trees
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#define sizeOFAttribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define USERNAME_MAX_LENGTH 32
#define EMAIL_MAX_LENGTH 256
#define TABLE_MAX_PAGES 100

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

typedef struct {
    int fileDescriptor;
    uint32_t fileLength;
    void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
    uint32_t numRows;
    Pager* pager;
} Table;

typedef struct {
    uint32_t id;
    char username[USERNAME_MAX_LENGTH + 1];
    char email[EMAIL_MAX_LENGTH + 1];
} Row;

typedef struct {
    // represents location in a table
    Table* table;
    uint32_t rowNum;
    bool endOfTable;    // indicates position one past the last elem
} Cursor;

void serializeRow(Row* source, void* destination);
void deserializeRow(void* source, Row* destination);
void* cursorValue(Cursor* cursor);
void cursorAdvance(Cursor* cursor);

Table* openDataBase(const char* fileHandle);
void closeDataBase(Table* table);
void* getPage(Pager* pager, uint32_t pageNum);
void pagerFlush(Pager* pager, uint32_t pageNum, uint32_t size);
Pager* openPager(const char* fileHandle);

Cursor* tableStart(Table* table);
Cursor* tableEnd(Table* table);

void displayRow(Row* row);

#endif