#include "database.h"

Table* openDataBase(const char* fileHandle) {
    Table* table = (Table*)malloc(sizeof(Table));
    if (table == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    Pager* pager = openPager(fileHandle);

    table->fileHandle = fileHandle;
    table->pager = pager;
    table->rootPageNum = 0;

    if (pager->numPages == 0) {
        //New db file, initialize page 0 as leaf node
        void* rootNode = getPage(pager, 0);
        initializeLeafNode(rootNode);
        setNodeRoot(rootNode, true);
    }

    return table;
}

void closeDataBase(Table* table) {
    // Flushes the page cache to disk
    // Closes the database file
    // Frees memory from Pager and Table structs

    Pager* pager = table->pager;

    for (uint32_t i = 0; i < pager->numPages; ++i) {
        if (pager->pages[i] == NULL) continue;

        pagerFlush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    int result = close(pager->fileDescriptor);
    if (result == -1) {
        fprintf(stderr, "Error while closing database file\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        void * page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}

void clearDataBase(Table *table)
{
    Pager* pager = table->pager;
    
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        void * page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }

    pager->numPages = 0;
    table->rootPageNum = 0;
    fclose(fopen(table->fileHandle, "w"));
}

void displayRow(Row* row) {
    printf("(%d %s %s)\n", row->id, row->username, row->email);
}

uint32_t getTableMaxID(Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);
    uint32_t maxID = 0;

    if (numCells == 0) {
        return 0;
    }

    Cursor* cursor = tableStart(table);

    Row row;
    while (!cursor->endOfTable) {
        deserializeRow(cursorValue(cursor), &row);
        if (row.id > maxID) { maxID = row.id; }
        cursorAdvance(cursor);
    }

    free(cursor);
    return maxID;
}

uint32_t getTableMinID(Table* table) {
    void* node = getPage(table->pager, table->rootPageNum);
    uint32_t numCells = *leafNodeNumCells(node);
    uint32_t minID = INT_MAX;

    if (numCells == 0) {
        return 0;
    }

    Cursor* cursor = tableStart(table);

    Row row;
    while (!cursor->endOfTable) {
        deserializeRow(cursorValue(cursor), &row);
        if (row.id < minID) { minID = row.id; }
        cursorAdvance(cursor);
    }

    free(cursor);
    return minID;
}
