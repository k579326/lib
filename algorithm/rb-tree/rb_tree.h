
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

typedef struct _Iterator
{
    TreeNode*   node_;
    PAIR*       data_;
}*Iterator;


typedef bool(*TypeLess)(const PAIR*, const PAIR*);

RbTree* CreateRbTree(uint16_t typelen, TypeLess cmp);
PAIR*   InsertNode(RbTree* rbtree, PAIR* pair);
PAIR*   Find(RbTree* rbtree, PAIR* keypair);
PAIR*   DeleteNode(RbTree* rbtree, PAIR* keypair);

const Iterator GetFirst(const RbTree* rbtree);
const Iterator GetNext(Iterator );

// is balance
bool CheckBalance(const RbTree* tree);
// check whether node num is correct
bool CheckNodeSum(const RbTree* tree);

void WalkTreeAsLevel(RbTree* tree);

#ifdef __cplusplus
}
#endif






