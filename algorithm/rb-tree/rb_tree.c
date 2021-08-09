
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

TreeNode* FindLastLessNode(TreeNode* rbtree, int key, bool force)
{
    if (!rbtree) {
        return NULL;
    }

    if (!less(rbtree->pair_.key, key)) {
        TreeNode* leftnode = rbtree->left_;
        if (leftnode && !less(leftnode->pair_.key, key))
            return FindLastLessNode(rbtree->left_, key, force);
        else if (leftnode)
            return leftnode;
        else {
            return force ? rbtree : NULL;
        }
    }
    else
    {
        TreeNode* rightnode = rbtree->right_;
        if (rightnode && less(rightnode->pair_.key, key))
            return FindLastLessNode(rbtree->right_, key, force);
        else
            return rbtree;
    }

    // never to this
    return NULL;
}
TreeNode* FindFirstLargeNode(TreeNode* rbtree, int key, bool force)
{
    if (!rbtree) {
        return NULL;
    }

    if (!less(key, rbtree->pair_.key)) {
        TreeNode* rightnode = rbtree->right_;
        if (rightnode && !less(key, rightnode->pair_.key))
            return FindFirstLargeNode(rightnode, key, force);
        else if (rightnode)
            return rightnode;
        else
            return force ? rbtree : NULL;
    }
    else
    {
        TreeNode* leftnode = rbtree->left_;
        if (leftnode && !less(leftnode->pair_.key, key))
            return FindFirstLargeNode(rbtree->left_, key, force);
        else
            return rbtree;
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

    /*
        查找到一个合适的插入点，但是这个点不一定比pair大，可能等于，也可能小于
        尝试先判断，根据判断结果来决定把新节点链到左边还是右边，然后再调整平衡？
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
    
    // 所有元素都比pair小
    if (PairLess(&insert_pos->pair_, pair)) {

    }





    // 下面代码不作数
    if (insert_pos->pair_.key == pair->key)
        return insert_pos;  // todo
    TreeNode* left = insert_pos->left_;
    TreeNode* parent = insert_pos->parent_;
    if (left && left->pair_.key == pair->key)
        return left;
    if (parent && parent->pair_.key == pair->key)
        return parent;

    TreeNode* new_node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!new_node) {
        return NULL;
    }
    memset(new_node, 0, sizeof(TreeNode));
    new_node->color_ = false;
    memcpy(&new_node->pair_, pair, sizeof(PAIR));

    if (left) {
        left->parent_ = new_node;
    }
    new_node->left_ = left;
    insert_pos->left_ = new_node;
    new_node->parent_ = insert_pos;

    TreeNode* adjust_node = NULL;
    if (left && left->color_ == true)
        adjust_node = left;
    else if (insert_pos->color_ == true)
        adjust_node = insert_pos;
    else
        // do nothing;


}