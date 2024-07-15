#include "node.h"

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

}