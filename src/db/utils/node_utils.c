#include "../tree/node.h"

uint32_t* nodeParent(void* node) {
    return node + PARENT_POINTER_OFFSET;
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

uint32_t getNodeMaxKey(Pager* pager, void* node) {
    if (getNodeType(node) == NODE_LEAF) {
        return *leafNodeKey(node, *leafNodeNumCells(node) - 1);
    }
    void* rightChild = getPage(pager, *internalNodeRightChild(node));
    return getNodeMaxKey(pager, rightChild);
}

NodeType getNodeType(void* node) {
    uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));
    return (NodeType) value;
}

void setNodeType(void* node, NodeType type) {
    uint8_t value = type;
    *((uint8_t*)node + NODE_TYPE_OFFSET) = value;
}
