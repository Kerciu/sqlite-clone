#include "node.h"

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


void updateInternalNodeKey(void* node, uint32_t oldKey, uint32_t newKey) {
    uint32_t oldChildIdx = internalNodeFindChild(node, oldKey);
    *internalNodeKey(node, oldChildIdx) = newKey;
}

void internalNodeSplitAndInsert(Table* table, uint32_t parentPageNum, uint32_t childPageNum) {
    uint32_t oldPageNum = parentPageNum;
    void* oldNode = getPage(table->pager, parentPageNum);
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

    for (uint32_t i = INTERNAL_NODE_MAX_CELLS - 1; i > INTERNAL_NODE_MAX_CELLS / 2; --i) {
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
