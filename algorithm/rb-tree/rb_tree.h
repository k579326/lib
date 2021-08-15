
#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _treenode TreeNode;
typedef struct _RBTree RbTree;
typedef void   PAIR;

typedef bool(*TypeLess)(const PAIR*, const PAIR*);

RbTree*     CreateRbTree(uint16_t typelen, TypeLess cmp);
TreeNode*   Insert(RbTree* rbtree, PAIR* pair);
PAIR*       Find(RbTree* retree, PAIR* keypair);


void WalkTreeAsLevel(RbTree* tree);

#ifdef __cplusplus
}
#endif






