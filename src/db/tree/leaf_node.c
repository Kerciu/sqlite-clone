#include "node.h"

uint32_t *leafNodeNumCells(void *node)
{
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void *leafNodeCell(void *node, uint32_t cellNum)
{
    return node + LEAF_NODE_HEADER_SIZE + cellNum * LEAF_NODE_CELL_SIZE;
}

uint32_t *leafNodeKey(void *node, uint32_t cellNum)
{
    return leafNodeCell(node, cellNum);
}

void *leafNodeValue(void *node, uint32_t cellNum)
{
    return leafNodeCell(node, cellNum) + LEAF_NODE_KEY_SIZE;
}

void initializeLeafNode(void *node)
{
    setNodeType(node, NODE_LEAF);
    setNodeRoot(node, false);
    *leafNodeNumCells(node) = 0;
    *leafNodeNextLeaf(node) = 0; // no sibling
}

void leafNodeInsert(Cursor* cursor, uint32_t key, Row* value) {
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    if (numCells >= LEAF_NODE_MAX_CELLS) {
        // Node is full
        leafNodeSplitAndInsert(cursor, key, value);
        return;
    }

    if (cursor->cellNum < numCells) {
        // save space for new cell
        for (uint32_t i = numCells; i > cursor->cellNum; --i) {
            memcpy(leafNodeCell(node, i), leafNodeCell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }

    ++(*(leafNodeNumCells(node)));
    *(leafNodeKey(node, cursor->cellNum)) = key;
    serializeRow(value, leafNodeValue(node, cursor->cellNum));
}

void leafNodeUpdate(Cursor* cursor, uint32_t key, Row* value) {
    void* node = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t numCells = *leafNodeNumCells(node);
    
    *(leafNodeKey(node, cursor->cellNum)) = key;
    serializeRow(value, leafNodeValue(node, cursor->cellNum));
}

Cursor* leafNodeFind(Table* table, uint32_t pageNum, uint32_t key) {
    void* node = getPage(table->pager, pageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->pageNum = pageNum;
    cursor->endOfTable = false;
    
    // Perform binary search
    uint32_t minIdx = 0;
    uint32_t onePastMaxIdx = numCells;

    while (onePastMaxIdx != minIdx) {
        uint32_t idx = (minIdx + onePastMaxIdx) / 2;
        uint32_t keyAtIdx = *leafNodeKey(node, idx);

        if (key == keyAtIdx) {
            cursor->cellNum = idx;
            return cursor;
        }
        if (key < keyAtIdx) {
            onePastMaxIdx = idx;
        }
        else {
            minIdx = idx + 1;
        }
    }

    cursor->cellNum = minIdx;

    /* this return either position of the key, position of another key
    that we will need to move if we want to insert the new key, or
    position one past the last key
    */
    return cursor;
}

void leafNodeSplitAndInsert(Cursor* cursor, uint32_t key, Row* value) {
    /* Create a new node and move half the cells over, insert the new value
    in one of two nodes, then update parent (or create new parent)
    */
    void* oldNode = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t oldMax = getNodeMaxKey(cursor->table->pager ,oldNode);
    uint32_t newPageNum = getUnusedPageNum(cursor->table->pager);

    void* newNode = getPage(cursor->table->pager, newPageNum);
    initializeLeafNode(newNode);
    *nodeParent(newNode) = *nodeParent(oldNode);
    *leafNodeNextLeaf(newNode) = *leafNodeNextLeaf(oldNode);
    *leafNodeNextLeaf(oldNode) = newPageNum;

   /**copy every cell into its new location, all existing keys + new key
    * should be divided evetli between left and right (old and new) nodes,
    * starting from the right, move each key to correct position. */

    for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; --i) {
        void* destinationNode;

        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT) {
            destinationNode = newNode;
        }
        else {
            destinationNode = oldNode;
        }

        uint32_t IdxWithinNode = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void* destination = leafNodeCell(destinationNode, IdxWithinNode);

        if (i == cursor->cellNum) {
            serializeRow(value, leafNodeValue(destinationNode, IdxWithinNode));
            *leafNodeKey(destinationNode, IdxWithinNode) = key;
        }
        else if (i > cursor->cellNum) {
            memcpy(destination, leafNodeCell(oldNode, i - 1), LEAF_NODE_CELL_SIZE);
        }
        else {
            memcpy(destination, leafNodeCell(oldNode, i), LEAF_NODE_CELL_SIZE);
        }
    }

    /* update cell count on both leaf nodes */
    *(leafNodeNumCells(oldNode)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(leafNodeNumCells(newNode)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    if (isNodeRoot(oldNode)) {
        return createNewRoot(cursor->table, newPageNum);
    }
    else {
        // fprintf(stderr, "Need to implement updating parent after split'\n");
        // exit(EXIT_FAILURE);
        uint32_t parentPageNum = *nodeParent(oldNode);
        uint32_t newMax = getNodeMaxKey(cursor->table->pager, oldNode);
        void* parentNode = getPage(cursor->table->pager, parentPageNum);
        
        updateInternalNodeKey(parentNode, oldMax, newMax);
        internalNodeInsert(cursor->table, parentPageNum, newPageNum);
        return;
    }
}

uint32_t *leafNodeNextLeaf(void *node)
{
    return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}
