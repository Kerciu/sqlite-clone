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

uint32_t* nodeParent(void* node) {
    return node + PARENT_POINTER_OFFSET;
}

void updateInternalNodeKey(void* node, uint32_t oldKey, uint32_t newKey) {
    uint32_t oldChildIdx = internalNodeFindChild(node, oldKey);
    *internalNodeKey(node, oldChildIdx) = newKey;
}

void internalNodeSplitAndInsert(Table* table, uint32_t parentPageNum, uint32_t childPageNum) {
    uint32_t oldPageNum = parentPageNum;
    void* oldNode = getPage(table->pager, oldPageNum);
    uint32_t oldMax = getNodeMaxKey(table->pager, oldNode);

    void* child = getPage(table->pager, childPageNum);
    uint32_t childMax = getNodeMaxKey(table->pager, child);

    uint32_t newPageNum = getUnusedPageNum(table->pager);

    uint32_t splittingRoot = isNodeRoot(oldNode);

    void* parent;
    void* newNode;
    if (splittingRoot) {
        createNewRoot(table, newPageNum);
        parent = getPage(table->pager, table->rootPageNum);
        /* if we split the root, we need to update old node to point to the
        new root's left child, new page num will already point to the new root's right child*/

        oldPageNum = *internalNodeChild(parent, 0);
        oldNode = getPage(table->pager, oldPageNum);
    }
    else {
        parent = getPage(table->pager, *nodeParent(oldNode));
        newNode = getPage(table->pager, newPageNum);
        initializeInternalNode(newNode);
    }

    uint32_t* oldNumKeys = internalNodeNumKeys(oldNode);

    uint32_t currentPageNum = *internalNodeRightChild(oldNode);
    void* currentNode = getPage(table->pager, currentPageNum);

    /* we put right child into new node and set right child of old node
        to invalid page number */
    
    internalNodeInsert(table, newPageNum, currentPageNum);
    *nodeParent(currentNode) = newPageNum;
    *internalNodeRightChild(oldNode) = INVALID_PAGE_NUM;

    /* for each key move the key and child to the new node
        until you getto the middle key*/

    for (uint32_t i = INTERNAL_NODE_MAX_CELLS - 1; i > INTERNAL_NODE_MAX_CELLS / 2; ++i) {
        currentPageNum = *internalNodeChild(oldNode, i);
        currentNode = getPage(table->pager, currentPageNum);

        internalNodeInsert(table, newPageNum, currentPageNum);
        *nodeParent(currentNode) = newPageNum;

        --(*oldNumKeys);
    }

    /* set child before middle key, which is now the highest key
        to be node's right child and decrement number of keys */
    
    *internalNodeRightChild(oldNode) = *internalNodeChild(oldNode, *oldNumKeys - 1);
    --(*oldNumKeys);

    /* determine which of two nodes after the split should contain the
        child to be inserted and insert it */
    
    uint32_t maxAfterSplit = getNodeMaxKey(table->pager, oldNode);
    uint32_t destinationPageNum = (childMax < maxAfterSplit ? oldPageNum : newPageNum);

    internalNodeInsert(table, destinationPageNum, childPageNum);
    *nodeParent(child) = destinationPageNum;

    updateInternalNodeKey(parent, oldMax, getNodeMaxKey(table->pager, oldNode));

    if (!splittingRoot) {
        internalNodeInsert(table, *nodeParent(oldNode), newPageNum);
        *nodeParent(newNode) = *nodeParent(oldNode);
    }
}

void internalNodeInsert(Table* table, uint32_t pageNum, uint32_t newPageNum) {
    /* add a new child and key pair to parent corresponding to child */

    void* parentNode = getPage(table->pager, pageNum);
    void* childNode = getPage(table->pager, newPageNum);

    uint32_t childMaxKey = getNodeMaxKey(table->pager, childNode);
    uint32_t idx = internalNodeFindChild(parentNode, childMaxKey);

    uint32_t originalNumKeys = *internalNodeNumKeys(parentNode);

    if (originalNumKeys >= INTERNAL_NODE_MAX_CELLS) {
        internalNodeSplitAndInsert(table, pageNum, newPageNum);
        return;
    }

    uint32_t rightChildPageNum = *internalNodeRightChild(parentNode);
    /* internal node with right child of invalid page number is empty */
    if (rightChildPageNum == INVALID_PAGE_NUM) {
        *internalNodeRightChild(parentNode) = newPageNum;
        return;
    }

    void* rightChildNode = getPage(table->pager, rightChildPageNum);
    *internalNodeNumKeys(parentNode) = originalNumKeys + 1;

    if (childMaxKey > getNodeMaxKey(table->pager, rightChildNode)) {
        /* replace right child */
        *internalNodeChild(parentNode, originalNumKeys) = rightChildPageNum;
        *internalNodeKey(parentNode, originalNumKeys) = getNodeMaxKey(table->pager, rightChildNode);
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

    if (getNodeType(root) == NODE_INTERNAL) {
        initializeInternalNode(rightChild);
        initializeInternalNode(leftChild);
    }

    /* left child has data copied from old root */
    memcpy(leftChild, root, PAGE_SIZE);
    setNodeRoot(leftChild, false);

    if (getNodeType(leftChild) == NODE_INTERNAL) {
        void* child;
        for (uint32_t i = 0; i < *internalNodeNumKeys(leftChild); ++i) {
            child = getPage(table->pager, *internalNodeChild(leftChild, i));
            *nodeParent(child) = leftChildPageNum;
        }
        child = getPage(table->pager, *internalNodeRightChild(leftChild));
        *nodeParent(child) = leftChildPageNum;
    }

    /* root node is a new internal node with one key and two children */
    initializeInternalNode(root);
    setNodeRoot(root, true);
    *internalNodeNumKeys(root) = 1;
    *internalNodeChild(root, 0) = leftChildPageNum;
    uint32_t leftChildMaxKey = getNodeMaxKey(table->pager, leftChild);
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
        uint32_t* rightChild = internalNodeRightChild(node);
        if (*rightChild == INVALID_PAGE_NUM) {
            fprintf(stderr, "Access to right child of a node with invalid page value denied\n");
            exit(EXIT_FAILURE);
        }
        return rightChild;
    }
    else {
        uint32_t* child = internalNodeCell(node, childNum);
        if (*child == INVALID_PAGE_NUM) {
            fprintf(stderr, "Access to child %d of node with invalid page value denied\n", childNum);
            exit(EXIT_FAILURE);
        }
        return child;
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
    *internalNodeRightChild(node) = INVALID_PAGE_NUM;
}

uint32_t getNodeMaxKey(Pager* pager, void* node) {
    if (getNodeType(node) == NODE_LEAF) {
        return *leafNodeKey(node, *leafNodeNumCells(node) - 1);
    }
    void* rightChild = getPage(pager, *internalNodeRightChild(node));
    return getNodeMaxKey(pager, rightChild);
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

        if (numKeys > 0) {
            for (uint32_t i = 0; i < numKeys; ++i) {
                child = *internalNodeKey(node, i);
                printTree(pager, child, indentationLvl + 1);

                indentation(indentationLvl + 1);
                printf("- key %d\n", *internalNodeKey(node, i));
            }
            child = *internalNodeRightChild(node);
            printTree(pager, child, indentationLvl + 1);
        }
        break;
    }
}
