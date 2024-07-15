#include "node.h"

typedef enum {
    SIBLING_LEFT, SIBLING_RIGHT
} SiblingSide;

void treeDeleteKey(Table* table, uint32_t key) {
    Cursor* cursor = tableFind(table, key);
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    if (cursor->cellNum >= numCells) return;    // key not found

    uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);
    if (keyAtIdx != key) return;    // key not found

    leafNodeDelete(cursor);

    if (*leafNodeNumCells(node) < LEAF_NODE_MIN_CELLS) {
        balanceTreeAfterDeletion(cursor);
    }
}

void leafNodeDelete(Cursor* cursor) {
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    if (cursor->cellNum >= numCells) return;

    for (uint32_t i = cursor->cellNum; i < numCells - 1; ++i) {
        memcpy(leafNodeCell(node, i), leafNodeCell(node, i + 1), LEAF_NODE_CELL_SIZE);
    }

    --(*leafNodeNumCells(node));

}

void balanceTreeAfterDeletion(Cursor* cursor) {
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t parentPageNum = *nodeParent(node);
    if (parentPageNum == INVALID_PAGE_NUM) return;      // if root, nothing is needed to be done

    void* parent = getPage(cursor->table->pager, parentPageNum);
    uint32_t leftSiblingPageNum = getSiblingPageNum(cursor, SIBLING_LEFT);
    uint32_t rightSiblingPageNum = getSiblingPageNum(cursor, SIBLING_RIGHT);

    if (leftSiblingPageNum != INVALID_PAGE_NUM) {
        void* leftSibling = getPage(cursor->table->pager, leftSiblingPageNum);
        if (*leafNodeNumCells(leftSibling) > LEAF_NODE_MIN_CELLS) {
            // rotate key with left sibling
            rotateKeysFromLeft(cursor, leftSibling);
            return;
        }
        else {  // merge with left sibling
            leafNodeMerge(cursor->table, leftSiblingPageNum, cursor->pageNum);
            return;
        }
    }
    if (rightSiblingPageNum != INVALID_PAGE_NUM) {
        void* rightSibling = getPage(cursor->table->pager, rightSiblingPageNum);
        if (*leafNodeNumCells(rightSibling) > LEAF_NODE_MIN_CELLS) {
            // rotate key with right sibling
            rotateKeysFromRight(cursor, rightSibling);
            return;
        }
        else {  // merge with right sibling
            leafNodeMerge(cursor->table, cursor->pageNum, rightSibling);
            return;
        }
    }
}

uint32_t getSiblingPageNum(Cursor* cursor, SiblingSide side) {

}

void rotateKeysFromLeft(Cursor* cursor, void* leftSibling) {

}

void rotateKeysFromRight(Cursor* cursor, void* rightSibling) {

}

void leafNodeMerge(Table* table, uint32_t leftPageNum, uint32_t rightPageNum) {
    
}