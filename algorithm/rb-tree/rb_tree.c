
#include "rb_tree.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _pair
{
    int             key;
    void* v_;
}PAIR;

typedef struct _treenode
{
    struct _treenode* parent_;
    struct _treenode* left_;
    struct _treenode* right_;
    bool              color_;       // true: red, false: black
    PAIR              pair_;
}TreeNode;


bool less(int l, int r)
{
    return l < r;
}


TreeNode* Find(TreeNode* rbtree, int key)
{
    if (!rbtree) {
        return NULL;
    }
    if (less(key, rbtree->pair_.key)) {
        return Find(rbtree->left_, key);
    }
    else if (less(rbtree->pair_.key, key))
    {
        return Find(rbtree->right_, key);
    }   
    
    return rbtree;
}

TreeNode* FindLastLessNode(TreeNode* rbtree, int key)
{
    if (!rbtree) {
        return NULL;
    }

    if (!less(rbtree->pair_.key, key)) {
        TreeNode* leftnode = rbtree->left_;
        if (!leftnode) {
            return NULL;
        }
        return FindLastLessNode(rbtree->left_, key);
    }
    else
    {
        TreeNode* rightnode = rbtree->right_;
        if (!rightnode)
        {
            return rbtree;
        }
        return FindLastLessNode(rightnode, key);
    }

    // never to this
    return NULL;
}
TreeNode* FindFirstLargeNode(TreeNode* rbtree, int key)
{
    if (!rbtree) {
        return NULL;
    }

    if (!less(key, rbtree->pair_.key)) 
    {
        TreeNode* rightnode = rbtree->right_;
        if (!rightnode)
        {
            return NULL;
        }
        return FindFirstLargeNode(rightnode, key);
    }
    else
    {
        TreeNode* leftnode = rbtree->left_;
        if (!leftnode) {
            return rbtree;
        }
        return FindFirstLargeNode(rbtree->left_, key);
    }
    // never to this
    return NULL;
}


static void MakeBalance(TreeNode* node)
{

}

static bool PairLess(const PAIR* l, const PAIR* r)
{
    return less(l->key, r->key);
}
static bool PairEqual(const PAIR* l, const PAIR* r)
{
    return !PairLess(l, r) && !PairLess(r, l);
}


static bool NodeLess(const TreeNode* l, const TreeNode* r)
{
    return PairLess(&l->pair_, &r->pair_);
}

static bool NodeEqual(const TreeNode* l, const TreeNode* r)
{
    return !NodeLess(l, r) && !NodeLess(r, l);
}

TreeNode* Insert(TreeNode* rbtree, PAIR* pair)
{
    TreeNode* insert_pos = NULL;
    if (!rbtree) {
        return NULL;
    }

    /* ˼·���ˣ� Ӧ��������Ҷ�ӽڵ��ϲ��룿
        ���ҵ�һ�����ʵĲ���㣬��������㲻һ����pair�󣬿��ܵ��ڣ�Ҳ����С��
        �������жϣ������жϽ�����������½ڵ�������߻����ұߣ�Ȼ���ٵ���ƽ�⣿
    */
    insert_pos = FindFirstLargeNode(rbtree, pair->key, true);
    
    TreeNode* left = insert_pos->left_;
    TreeNode* parent = insert_pos->parent_;
    if (PairEqual(&insert_pos->pair_, pair))
        return NULL;
    if (left && PairEqual(&left->pair_, pair))
        return NULL;
    if (parent && PairEqual(&parent->pair_, pair))
        return NULL;
    
    // ����Ԫ�ض���pairС
    if (PairLess(&insert_pos->pair_, pair)) {

    }

    return NULL;
}
