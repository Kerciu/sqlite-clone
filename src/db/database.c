#include "database.h"

void serializeRow(Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserializeRow(void* source, Row* destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void* getPage(Pager* pager, uint32_t pageNum) {
    if (pageNum > TABLE_MAX_PAGES) {
        fprintf(stderr, "Unable to fetch page number in bounds (%d > %d)\n", pageNum, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[pageNum] == NULL) {        // Cache miss
        // Allocate memory and load from file
        void* page = malloc(PAGE_SIZE);
        uint32_t numOfPages = pager->fileLength / PAGE_SIZE;

        // Save partial page at the end of file
        if (pager->fileLength % PAGE_SIZE) {
            ++numOfPages;
        }

        if (pageNum <= numOfPages) {
            lseek(pager->fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);
            ssize_t bytesRead = read(pager->fileDescriptor, page, PAGE_SIZE);

            if (bytesRead == -1) {
                fprintf(stderr, "Failed to read file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[pageNum] = page;

        if (pageNum >= pager->numPages) {
            pager->numPages = pageNum + 1;
        }
    }

    return pager->pages[pageNum];
}

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

void pagerFlush(Pager* pager, uint32_t pageNum) {
    if (pager->pages[pageNum] == NULL) {
        fprintf(stderr, "Error: trying to flush NULL page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        fprintf(stderr, "Error occured while seeking %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytesWritten = write(pager->fileDescriptor, pager->pages[pageNum], PAGE_SIZE);
    if (bytesWritten == -1) {
        fprintf(stderr, "Error occured while writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
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

Pager* openPager(const char* fileHandle) {
    int fileDescript = open(fileHandle, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fileDescript == -1) {
        fprintf(stderr, "Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t fileLen = lseek(fileDescript, 0, SEEK_END);

    Pager* pager = (Pager*)malloc(sizeof(Pager));
    pager->fileDescriptor = fileDescript;
    pager->fileLength = fileLen;
    pager->numPages = (fileLen / PAGE_SIZE);

    if (fileLen % PAGE_SIZE != 0) {
        fprintf(stderr, "Data base file is not a whole number of pages. Corrunt file\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        pager->pages[i] = NULL;
    }

    return pager;
}

Cursor *tableStart(Table *table)
{
    Cursor* cursor = tableFind(table, 0);

    void* node = getPage(table->pager, cursor->pageNum);
    uint32_t numCells = *leafNodeNumCells(node);
    cursor->endOfTable = (numCells == 0);

    return cursor;
}

Cursor* tableFind(Table* table, uint32_t key)
{
    // search the tree for a given key
    // if not present, return position where it should be inserted

    uint32_t rootPageNum = table->rootPageNum;
    void* rootNode = getPage(table->pager, rootPageNum);

    if (getNodeType(rootNode) == NODE_LEAF) {
        return leafNodeFind(table, rootPageNum, key);
    }
    else {
        return internalNodeFind(table, rootPageNum, key);
    }
}

void* cursorValue(Cursor* cursor)
{
    uint32_t pageNum = cursor->pageNum;
    void* page = getPage(cursor->table->pager, pageNum);

    return leafNodeValue(page, cursor->cellNum);
}

void cursorAdvance(Cursor* cursor) {
    uint32_t pageNum = cursor->pageNum;
    void* node = getPage(cursor->table->pager, pageNum);

    ++(cursor->cellNum);
    if (cursor->cellNum >= (*leafNodeNumCells(node))) {
        
        /* advance to next leaf node */
        uint32_t nextPageNum = *leafNodeNextLeaf(node);
        if (nextPageNum == 0) { // rightmost leaf
            cursor->endOfTable = true;
        }
        else {
            cursor->pageNum = nextPageNum;
            cursor->cellNum = 0;
        }
    }
}
