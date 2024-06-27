#ifndef B_TREE_NODE_DATA_STRUCTURE
#define B_TREE_NODE_DATA_STRUCTURE

#include <stdbool.h>
#include <stdint.h>
#include "types.h"
#include "database.h"

// https://www.programiz.com/dsa/b-tree

/* Access Leaf Node Fields */

uint32_t* leafNodeNumCells(void* node);
void* leafNodeCell(void* node, uint32_t cellNum);
uint32_t* leafNodeKey(void* node, uint32_t cellNum);
void* leafNodeValue(void* node, uint32_t cellNum);
void initializeLeafNode(void* node);
void leafNodeInsert(Cursor* cursor, uint32_t key, Row* value);
void printLeafNode(void* node);

#endif