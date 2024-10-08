#include "node.h"

void treeDeleteKey(Table* table, uint32_t key) {
    Cursor* cursor = tableFind(table, key);
    if (cursor == NULL) {
        return;
    }

    if (cursor->pageNum >= table->pager->numPages) {
        free(cursor);
        return;
    }

    if (!cursorKeyAcquired(cursor, key)) return;

    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    if (cursor->cellNum >= numCells) {
        free(cursor);
        return;
    }

    uint32_t keyAtIdx = *leafNodeKey(node, cursor->cellNum);
    if (keyAtIdx != key) {
        free(cursor);
        return;
    }
    leafNodeDelete(cursor);

    if (*leafNodeNumCells(node) < LEAF_NODE_MIN_CELLS) {
        balanceTreeAfterDeletion(cursor);
    }

    free(cursor);
}

void leafNodeDelete(Cursor* cursor) {
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    if (cursor->cellNum >= numCells) {
        return;
    }

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
            leafNodeMerge(cursor->table, cursor->pageNum, rightSiblingPageNum);
            return;
        }
    }
}

uint32_t getSiblingPageNum(Cursor* cursor, SiblingSide side) {
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t parentPageNum = *nodeParent(node);
    void* parent = getPage(cursor->table->pager, parentPageNum);

    uint32_t idxInParent = getIdxInParent(parent, cursor->pageNum);

    if (side == SIBLING_LEFT) {
        if (idxInParent == 0) {
            return INVALID_PAGE_NUM;
        }
        else {
            return *internalNodeChild(parent, idxInParent - 1);
        }
    }
    else {
        uint32_t numChildren = *internalNodeNumKeys(parent) + 1;
        if (idxInParent >= numChildren - 1) {
            return INVALID_PAGE_NUM;
        }
        else {
            return *internalNodeChild(parent, idxInParent + 1);
        }
    }
}

uint32_t getIdxInParent(void* parent, uint32_t childPageNum) {
    // find index of child in a parent
    uint32_t numKeys = *internalNodeNumKeys(parent);
    for (uint32_t i = 0; i <= numKeys; ++i) {
        if (*internalNodeChild(parent, i) == childPageNum) {
            return i;
        }
    }

    return INVALID_INDEX;
}

void rotateKeysFromLeft(Cursor* cursor, void* leftSibling) {
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t leftNumCells = *leafNodeNumCells(node);

    // move keys to the right in a node
    for (uint32_t i = *leafNodeNumCells(node); i > 0; --i) {
        memcpy(leafNodeCell(node, i), leafNodeCell(node, i - 1), LEAF_NODE_CELL_SIZE);
    }

    // copy last key from left sibling to the current node
    memcpy(leafNodeCell(node, 0), leafNodeCell(leftSibling, leftNumCells - 1), LEAF_NODE_CELL_SIZE);

    ++(*leafNodeNumCells(node));
    --(*leafNodeNumCells(leftSibling));

    // update key in the parent
    uint32_t parentPageNum = *nodeParent(node);
    void* parent = getPage(cursor->table->pager, parentPageNum);
    uint32_t idxInParent = getIdxInParent(parent, cursor->pageNum);
    *internalNodeKey(parent, idxInParent - 1) = *leafNodeKey(node, 0);
}

void rotateKeysFromRight(Cursor* cursor, void* rightSibling) {
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t rightNumCells = *leafNodeNumCells(rightSibling);

    // copy first key from the left parent to the current node
    memcpy(leafNodeCell(node, *leafNodeNumCells(node)), leafNodeCell(rightSibling, 0), LEAF_NODE_CELL_SIZE);
    ++(*leafNodeNumCells(node));

    // move keys to the left in a right sibling
    for (uint32_t i = 0; i < rightNumCells - 1; ++i) {
        memcpy(leafNodeCell(rightSibling, i), leafNodeCell(rightSibling, i + 1), LEAF_NODE_CELL_SIZE);
    }

    --(*leafNodeNumCells(rightSibling));

    // update key in parent
    uint32_t parentPageNum = *nodeParent(node);
    void* parent = getPage(cursor->table->pager, parentPageNum);
    uint32_t idxInParent = getIdxInParent(parent, cursor->pageNum);
    *internalNodeKey(parent, idxInParent) = *leafNodeKey(rightSibling, 0);
}

void leafNodeMerge(Table* table, uint32_t leftPageNum, uint32_t rightPageNum) {
    void* leftNode = getPage(table->pager, leftPageNum);
    void* rightNode = getPage(table->pager, rightPageNum);

    uint32_t leftNumCells = *leafNodeNumCells(leftNode);
    uint32_t rightNumCells = *leafNodeNumCells(rightNode);

    // move all keys from right node to the left node
    for (uint32_t i = 0; i < rightNumCells; ++i) {
        void* dest = leafNodeCell(leftNode, leftNumCells + i);
        void* src = leafNodeCell(rightNode, i);
        memcpy(dest, src, LEAF_NODE_CELL_SIZE);
    }

    *leafNodeNumCells(leftNode) += rightNumCells;
    *leafNodeNextLeaf(leftNode) = *leafNodeNextLeaf(rightNode);     // update pointers

    uint32_t parentPageNum = *nodeParent(leftNode);
    void* parent = getPage(table->pager, parentPageNum);
    internalNodeDeleteChild(parent, rightPageNum);

    // update the parent
    updateInternalNodeKey(parent, getNodeMaxKey(table->pager, leftNode), getNodeMaxKey(table->pager, rightNode));
}

void internalNodeDeleteChild(void* node, uint32_t childPageNum) {
    // find and delete child from internal node
    uint32_t numKeys = *internalNodeNumKeys(node);
    uint32_t i;

    for (i = 0; i <= numKeys; ++i) {
        if (*internalNodeChild(node, i) == childPageNum) break;
    }

    if (i > numKeys) return;

    for (uint32_t j = i; j < numKeys; ++j) {
        *internalNodeCell(node, j) = *internalNodeChild(node, j + 1);
        if (j < numKeys - 1) {
            *internalNodeKey(node, j) = *internalNodeKey(node, j + 1);
        }
    }

    --(*internalNodeNumKeys(node));
}
