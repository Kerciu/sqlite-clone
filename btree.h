#ifndef B_TREE_DATA_STRUCTURE
#define B_TREE_DATA_STRUCTURE

#include <stdbool.h>
#include <stdint.h>
#include "database.h"

// https://www.programiz.com/dsa/b-tree

/* Node Header Format*/
typedef enum { NODE_INTERNAL, NODE_LEAF } NodeType;

extern const uint32_t NODE_TYPE_SIZE;
extern const uint32_t NODE_TYPE_OFFSET;
extern const uint32_t IS_ROOT_SIZE;
extern const uint32_t IS_ROOT_OFFSET;
extern const uint32_t PARENT_POINTER_SIZE;
extern const uint32_t PARENT_POINTER_OFFSET;
extern const uint8_t COMMON_NODE_HEADER_SIZE;

/* Leaf Node Header Layout */
extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE;

/* Leaf Node Body Layout*/
extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern const uint32_t LEAF_NODE_VALUE_SIZE;
extern const uint32_t LEAF_NODE_VALUE_OFFSET;

extern const uint32_t LEAF_NODE_CELL_SIZE;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t LEAF_NODE_MAX_CELLS;

/* Access Leaf Node Fields */

uint32_t* leafNodeNumCells(void* node);
void* leafNodeCell(void* node, uint32_t cellNum);
uint32_t* leafNodeKey(void* node, uint32_t cellNum);
void* leafNodeValue(void* node, uint32_t cellNum);
void initializeLeafNode(void* node);

#endif