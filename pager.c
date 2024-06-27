#include "pager.h"

const uint32_t ID_SIZE = sizeOFAttribute(Row, id);
const uint32_t USERNAME_SIZE = sizeOFAttribute(Row, username);
const uint32_t EMAIL_SIZE = sizeOFAttribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

void serializeRow(Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
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
    uint32_t numRows = pager->fileLength / ROW_SIZE;

    table->numRows = numRows;
    table->pager = pager;

    return table;
}

void closeDataBase(Table* table) {
    // Flushes the page cache to disk
    // Closes the database file
    // Frees memory from Pager and Table structs

    Pager* pager = table->pager;
    uint32_t fullPageNum = table->numRows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < fullPageNum; ++i) {
        if (pager->pages[i] == NULL) continue;

        pagerFlush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // There might be partial page to write at the end of file
    uint32_t additionalsRowsNum = table->numRows % ROWS_PER_PAGE;
    if (additionalsRowsNum > 0) {
        uint32_t pageNum = fullPageNum;

        if (pager->pages[pageNum] != NULL) {
            pagerFlush(pager, pageNum, additionalsRowsNum * ROW_SIZE);
            free(pager->pages[pageNum]);
        }
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

void pagerFlush(Pager* pager, uint32_t pageNum, uint32_t size) {
    if (pager->pages[pageNum] == NULL) {
        fprintf(stderr, "Error: trying to flush NULL page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fileDescriptor, pageNum * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        fprintf(stderr, "Error occured while seeking %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytesWritten = write(pager->fileDescriptor, pager->pages[pageNum], size);
    if (bytesWritten == -1) {
        fprintf(stderr, "Error occured while writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

void displayRow(Row* row) {
    printf("(%d %s %s)\n", row->id, row->username, row->email);
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

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        pager->pages[i] = NULL;
    }

    return pager;
}

Cursor *tableStart(Table *table)
{
    Cursor* startCursor = (Cursor*)malloc(sizeof(Cursor));
    startCursor->table = table;
    startCursor->rowNum = 0;
    startCursor->endOfTable = (table->numRows == 0);

    return startCursor;
}

Cursor *tableEnd(Table *table)
{
    Cursor* endCursor = (Cursor*)malloc(sizeof(Cursor));
    endCursor->table = table;
    endCursor->rowNum = table->numRows;
    endCursor->endOfTable = true;

    return endCursor;
}

void* cursorValue(Cursor* cursor)
{
    uint32_t pageNum = cursor->rowNum / ROWS_PER_PAGE;
    void* page = getPage(cursor->table->pager, pageNum);
    uint32_t rowOffset = cursor->rowNum % ROWS_PER_PAGE;
    uint32_t byteOffset = rowOffset * ROW_SIZE;

    return page + byteOffset;
}

void cursorAdvance(Cursor* cursor) {
    ++(cursor->rowNum);
    if (cursor->rowNum >= cursor->table->numRows) {
        cursor->endOfTable = true;
    }
}
