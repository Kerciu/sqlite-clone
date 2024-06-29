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

Cursor* leafNodeFind(Table* table, uint32_t pageNum, uint32_t key) {
    void* node = getPage(table->pager, pageNum);
    uint32_t numCells = *leafNodeNumCells(node);

    Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->pageNum = pageNum;
    
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

uint32_t internalNodeFindChild(void* node, uint32_t key) {
    /* return idx of child which should contain given key */

    uint32_t numKeys = *internalNodeNumKeys(node);

    /* binary search */
    uint32_t minIdx = 0;
    uint32_t maxIdx = numKeys;

    while (minIdx != maxIdx) {
        uint32_t idx = (minIdx + maxIdx) / 2;
        uint32_t keyToRight = *internalNodeKey(node, idx);

        if (keyToRight >= key) {
            maxIdx = idx;
        }
        else {
            minIdx = idx + 1;
        }
    }

    return minIdx;
}

Cursor* internalNodeFind(Table* table, uint32_t pageNum, uint32_t key) {
    void* node = getPage(table->pager, pageNum);

    uint32_t childIdx = internalNodeFindChild(node, key);
    uint32_t childNum = *internalNodeChild(node, childIdx);

    void* childNode = getPage(table->pager, childNum);    
    switch (getNodeType(childNode)) {
        case NODE_LEAF:
            return leafNodeFind(table, childNum, key);
        case NODE_INTERNAL:
            return internalNodeFind(table, childNum, key);
    }
}

void leafNodeSplitAndInsert(Cursor* cursor, uint32_t key, Row* value) {
    /* Create a new node and move half the cells over, insert the new value
    in one of two nodes, then update parent (or create new parent)
    */

    void* oldNode = getPage(cursor->table->pager, cursor->pageNum);
    uint32_t oldMax = getNodeMaxKey(oldNode);
    uint32_t newPageNum = getUnusedPageNum(cursor->table->pager);

    void* newNode = getPage(cursor->table->pager, newPageNum);
    initializeLeafNode(newNode);
    *nodeParent(newNode) = *nodeParent(oldNode);
    *leafNodeNextLeaf(newNode) = *leafNodeNextLeaf(oldNode);
    *leafNodeNextLeaf(oldNode) = newPageNum;

   /**copy every cell into its new location, all existing keys + new key
    * should be divided evetli between left and right (old and new) nodes,
    * starting from the right, move each key to correct position. */

    for (int32_t i = 0; i< LEAF_NODE_MAX_CELLS; ++i) {
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
        createNewRoot(cursor->table, newPageNum);
        return;
    }
    else {
        // fprintf(stderr, "Need to implement updating parent after split'\n");
        // exit(EXIT_FAILURE);
        uint32_t parentPageNum = *nodeParent(oldNode);
        uint32_t newMax = getNodeMaxKey(oldNode);
        void* parentNode = getPage(cursor->table->pager, parentPageNum);
        
        updateInternalNodeKey(parentNode, oldMax, newMax);
        internalNodeInsert(cursor->table, parentPageNum, newPageNum);
        return;
    }
}

uint32_t* nodeParent(void* node) {
    return node + PARENT_POINTER_OFFSET;
}

void updateInternalNodeKey(void* node, uint32_t oldKey, uint32_t newKey) {
    uint32_t oldChildIdx = internalNodeFindChild(node, oldKey);
    *internalNodeKey(node, oldChildIdx) = newKey;
}

void internalNodeInsert(Table* table, uint32_t pageNum, uint32_t newPageNum) {
    /* add a new child and key pair to parent corresponding to child */

    void* parentNode = getPage(table->pager, pageNum);
    void* childNode = getPage(table->pager, newPageNum);

    uint32_t childMaxKey = getNodeMaxKey(childNode);
    uint32_t idx = internalNodeFindChild(childNode, newPageNum);

    uint32_t originalNumKeys = *internalNodeNumKeys(parentNode);
    *internalNodeNumKeys(parentNode) = originalNumKeys + 1;

    if (originalNumKeys >= INTERNAL_NODE_MAX_CELLS) {
        printf("Need to implement splitting internal node\n");
        exit(EXIT_FAILURE);
    }

    uint32_t rightChildPageNum = *internalNodeRightChild(parentNode);
    void* rightChildNode = getPage(table->pager, rightChildPageNum);

    if (childMaxKey > getNodeMaxKey(rightChildNode)) {
        /* replace right child */
        *internalNodeChild(parentNode, originalNumKeys) = rightChildPageNum;
        *internalNodeKey(parentNode, originalNumKeys) = getNodeMaxKey(rightChildNode);
        *internalNodeRightChild(parentNode) = newPageNum;
    }
    else {
        /* make space for the new cell */
        for (uint32_t i = originalNumKeys; i > idx; --i) {
            void* destination = internalNodeCell(parentNode, i);
            void* source = internalNodeCell(parentNode, i - 1);
            memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
        }
        *internalNodeChild(parentNode, idx) = newPageNum;
        *internalNodeKey(parentNode, idx) = childMaxKey;
    }
}

uint32_t *leafNodeNextLeaf(void *node)
{
    return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

void createNewRoot(Table* table, uint32_t rightChildPageNum) {
    /* handle splitting the root, old root copied to new page, becomes left child
    addres of right child passed it,
    reinitialize root page to contain the new root node
    new root node points to two childer
    */

    void* root = getPage(table->pager, table->rootPageNum);
    void* rightChild = getPage(table->pager, rightChildPageNum);
    uint32_t leftChildPageNum = getUnusedPageNum(table->pager);
    void* leftChild = getPage(table->pager, leftChildPageNum);

    /* left child has data copied from old root */
    memcpy(leftChild, root, PAGE_SIZE);
    setNodeRoot(leftChild, false);

    /* root node is a new internal node with one key and two children */
    initializeInternalNode(root);
    setNodeRoot(root, true);
    *internalNodeNumKeys(root) = 1;
    *internalNodeChild(root, 0) = leftChildPageNum;
    uint32_t leftChildMaxKey = getNodeMaxKey(leftChild);
    *internalNodeKey(root, 0) = leftChildMaxKey;
    *internalNodeRightChild(root) = rightChildPageNum;
    *nodeParent(leftChild) = table->rootPageNum;
    *nodeParent(rightChild) = table->rootPageNum;
}

bool isNodeRoot(void* node) {
    uint8_t value = *((uint8_t*)(node + IS_ROOT_OFFSET));
    return (bool)value;
}

void setNodeRoot(void* node, bool isRoot) {
    uint8_t value = isRoot;
    *((uint8_t*)(node + IS_ROOT_OFFSET)) = value;
}

uint32_t* internalNodeNumKeys(void* node) {
    return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

uint32_t* internalNodeCell(void* node, uint32_t cellNum) {
    return node + INTERNAL_NODE_HEADER_SIZE + cellNum * INTERNAL_NODE_CELL_SIZE;
}

uint32_t* internalNodeChild(void* node, uint32_t childNum) {
    uint32_t numKeys = *internalNodeNumKeys(node);

    if (childNum > numKeys) {
        fprintf(stderr, "Tried to acces childNum %d > numKeys %d\n", childNum, numKeys);
        exit(EXIT_FAILURE);
    }
    else if (childNum == numKeys) {
        return internalNodeRightChild(node);
    }
    else {
        return internalNodeCell(node, childNum);
    }
}

uint32_t* internalNodeKey(void* node, uint32_t keyNum) {
    return (void*)internalNodeCell(node, keyNum) + INTERNAL_NODE_CHILD_SIZE;
}

uint32_t* internalNodeRightChild(void* node) {
    return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

void initializeInternalNode(void* node) {
    setNodeType(node, NODE_INTERNAL);
    setNodeRoot(node, false);
    *internalNodeNumKeys(node) = 0;
}

uint32_t getNodeMaxKey(void* node) {
    switch(getNodeType(node)) {
        case NODE_INTERNAL:
            return *internalNodeKey(node, *internalNodeNumKeys(node));
        case NODE_LEAF:
            return *leafNodeKey(node, *leafNodeNumCells(node) - 1);
    }
}

uint32_t getUnusedPageNum(Pager* pager) {
    return pager->numPages;
}

NodeType getNodeType(void* node) {
    uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));
    return (NodeType) value;
}

void setNodeType(void* node, NodeType type) {
    uint8_t value = type;
    *((uint8_t*)node + NODE_TYPE_OFFSET) = value;
}

void indentation(uint32_t level) {
    for (uint32_t i = 0; i < level; ++i) {
        printf("   ");
    }
}

void printTree(Pager *pager, uint32_t pageNum, uint32_t indentationLvl) {
    void* node = getPage(pager, pageNum);
    uint32_t numKeys;
    uint32_t child;

    switch (getNodeType(node))
    {
    case NODE_LEAF:

        numKeys = *leafNodeNumCells(node);

        indentation(indentationLvl);
        printf("- leaf (size %d)\n", numKeys);

        for (uint32_t i = 0; i < numKeys; ++i) {
            indentation(indentationLvl + 1);
            printf("- %d\n", *leafNodeKey(node, i));
        }

        break;

    case NODE_INTERNAL:

        numKeys = *internalNodeNumKeys(node);
        
        indentation(indentationLvl);
        printf("- internal node (size %d)\n", numKeys);

        for (uint32_t i = 0; i < numKeys; ++i) {
            child = *internalNodeChild(node, i);
            printTree(pager, child, indentationLvl + 1);

            indentation(indentationLvl + 1);
            printf("- key %d\n", *internalNodeKey(node, i));
        }

        child = *internalNodeRightChild(node);
        printTree(pager, child, indentationLvl + 1);
        break;
    }
}
