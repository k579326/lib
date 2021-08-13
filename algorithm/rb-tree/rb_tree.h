
#pragma once

#include <stddef.h>

typedef struct _treenode TreeNode;
typedef struct _RBTree RbTree;

RbTree* CreateRbTree();
TreeNode* Insert(RbTree* rbtree, PAIR* pair);
TreeNode* Find(RbTree* retree, int key);








