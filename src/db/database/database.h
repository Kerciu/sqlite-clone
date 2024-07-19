#ifndef DATABASE
#define DATABASE

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
#include "../utils/types.h"
#include "../tree/node.h"

extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;
extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;
extern const uint32_t PAGE_SIZE;

/* Node Header Format*/
extern const uint32_t NODE_TYPE_SIZE;
extern const uint32_t NODE_TYPE_OFFSET;
extern const uint32_t IS_ROOT_SIZE;
extern const uint32_t IS_ROOT_OFFSET;
extern const uint32_t PARENT_POINTER_SIZE;
extern const uint32_t PARENT_POINTER_OFFSET;
extern const uint8_t COMMON_NODE_HEADER_SIZE;

/* Leaf Node Header Layout */
extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET;
extern const uint32_t LEAF_NODE_NEXT_LEAF_SIZE;
extern const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE;

/* Leaf Node Body Layout*/
extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern const uint32_t LEAF_NODE_VALUE_SIZE;
extern const uint32_t LEAF_NODE_VALUE_OFFSET;

extern const uint32_t LEAF_NODE_CELL_SIZE;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t LEAF_NODE_MAX_CELLS;
extern const uint32_t LEAF_NODE_MIN_CELLS;

/* Leaf node sizes */
extern const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT;
extern const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT;

/* Internal node header layout */
extern const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE;
extern const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET;
extern const uint32_t INTERNAL_NODE_HEADER_SIZE;

/* Internal node body layout */
extern const uint32_t INTERNAL_NODE_KEY_SIZE;
extern const uint32_t INTERNAL_NODE_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_CELL_SIZE;

extern const uint32_t INTERNAL_NODE_MAX_CELLS;

void serializeRow(Row* source, void* destination);
void deserializeRow(void* source, Row* destination);
void* cursorValue(Cursor* cursor);
void cursorAdvance(Cursor* cursor);
bool cursorKeyAcquired(Cursor* cursor, uint32_t key);

Table* openDataBase(const char* fileHandle);
void closeDataBase(Table* table);
void clearDataBase(Table* table);
void* getPage(Pager* pager, uint32_t pageNum);
void pagerFlush(Pager* pager, uint32_t pageNum);
Pager* openPager(const char* fileHandle);

Cursor* tableStart(Table* table);
Cursor* tableFind(Table* table, uint32_t key);

uint32_t getTableMinID(Table* table);
uint32_t getTableMaxID(Table* table);
void displayRow(Row* row);
void printConstants(void);

#endif