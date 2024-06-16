#include "btree.h"

typedef struct {
    int* key;
    int t;
    TreeNode** C;
    int n;
    bool leaf;
} TreeNode;