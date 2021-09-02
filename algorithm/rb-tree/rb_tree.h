
#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void   PAIR;
typedef struct _node
{
    PAIR* data;
}Node;
typedef struct _RBTree RbTree;

typedef bool(*TypeLess)(const PAIR*, const PAIR*);

RbTree* CreateRbTree(uint16_t typelen, TypeLess cmp);
Node*   InsertNode(RbTree* rbtree, PAIR* pair);
Node*   Find(RbTree* rbtree, PAIR* keypair);
Node*   DeleteNode(RbTree* rbtree, Node* keypair);

Node* GetFirst(const RbTree* rbtree);
Node* GetNext(Node* node);

// is balance
bool CheckBalance(const RbTree* tree);
// check whether node num is correct
bool CheckNodeSum(const RbTree* tree);

void WalkTreeAsLevel(RbTree* tree);

#ifdef __cplusplus
}
#endif






