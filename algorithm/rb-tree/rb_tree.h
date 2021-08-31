
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
TreeNode*   InsertNode(RbTree* rbtree, PAIR* pair);
PAIR*       Find(RbTree* rbtree, PAIR* keypair);
void        DeleteNode(RbTree* rbtree, PAIR* keypair);

// is balance
bool regular_2(const RbTree* tree);
// check whether node num is correct
bool regular_3(const RbTree* tree);

void WalkTreeAsLevel(RbTree* tree);

#ifdef __cplusplus
}
#endif






