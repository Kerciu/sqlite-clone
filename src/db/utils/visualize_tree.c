#include "../tree/node.h"

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
                child = *internalNodeChild(node, i);
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
