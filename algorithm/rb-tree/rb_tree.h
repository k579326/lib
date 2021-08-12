
#pragma once

#include <stddef.h>

typedef struct _treenode TreeNode;
typedef TreeNode RbTree;

RbTree* CreateRbTree();
TreeNode* Insert(RbTree* rbtree, PAIR* pair);
TreeNode* Find(TreeNode* retree, int key);








