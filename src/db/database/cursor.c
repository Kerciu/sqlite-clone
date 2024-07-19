#include "database.h"

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

bool cursorKeyAcquired(Cursor* cursor, uint32_t key) {
    Row row;
    deserializeRow(cursorValue(cursor), &row);
    return (row.id == key);
}
