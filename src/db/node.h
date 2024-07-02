#ifndef B_TREE_NODE_DATA_STRUCTURE
#define B_TREE_NODE_DATA_STRUCTURE

#include <stdbool.h>
#include <stdint.h>
#include "types.h"
#include "database.h"

#define INVALID_PAGE_NUM UINT32_MAX

// https://www.programiz.com/dsa/b-tree

/* Node Header Format*/
typedef enum { NODE_INTERNAL, NODE_LEAF } NodeType;

/* Access Leaf Node Fields */

uint32_t* leafNodeNumCells(void* node);
void* leafNodeCell(void* node, uint32_t cellNum);
uint32_t* leafNodeKey(void* node, uint32_t cellNum);
void* leafNodeValue(void* node, uint32_t cellNum);
void initializeLeafNode(void* node);
void leafNodeInsert(Cursor* cursor, uint32_t key, Row* value);
void leafNodeUpdate(Cursor* cursor, uint32_t key, Row* value);
void leafNodeDelete(Cursor* cursor, uint32_t key);
Cursor* leafNodeFind(Table* table, uint32_t pageNum, uint32_t key);
void leafNodeSplitAndInsert(Cursor* cursor, uint32_t key, Row* value);
uint32_t* leafNodeNextLeaf(void* node);

void createNewRoot(Table* table, uint32_t newPage);
bool isNodeRoot(void* node);
uint32_t getUnusedPageNum(Pager* pager);

void setNodeRoot(void* node, bool isRoot);
uint32_t* internalNodeNumKeys(void* node);
uint32_t* internalNodeCell(void* node, uint32_t cellNum);
uint32_t* internalNodeChild(void* node, uint32_t childNum);
uint32_t* internalNodeKey(void* node, uint32_t keyNum);
uint32_t* internalNodeRightChild(void* node);
void initializeInternalNode(void* node);
uint32_t internalNodeFindChild(void* node, uint32_t key);
Cursor* internalNodeFind(Table* table, uint32_t pageNum, uint32_t key);
uint32_t* nodeParent(void* node);
void internalNodeSplitAndInsert(Table* table, uint32_t parentPageNum, uint32_t childPageNum) ;
void internalNodeInsert(Table* table, uint32_t pageNum, uint32_t newPageNum);
void updateInternalNodeKey(void* node, uint32_t oldKey, uint32_t newKey);

uint32_t getNodeMaxKey(Pager* pager, void* node);

NodeType getNodeType(void* node);
void setNodeType(void* node, NodeType type);

void indentation(uint32_t level);
void printTree(Pager* pager, uint32_t pageNum, uint32_t indentationLvl);

#endif