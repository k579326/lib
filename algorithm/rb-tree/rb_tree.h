
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _treenode TreeNode;
typedef struct _RBTree RbTree;

typedef struct _pair
{
    int             key;
    void* v_;
}PAIR;


RbTree* CreateRbTree();
TreeNode* Insert(RbTree* rbtree, PAIR* pair);
PAIR* Find(RbTree* retree, int key);

void WalkTreeAsLevel(RbTree* tree);

#ifdef __cplusplus
}
#endif






