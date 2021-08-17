
#include "rb_tree.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _treenode
{
    struct _treenode* parent_;
    struct _treenode* left_;
    struct _treenode* right_;
    char              color_;       // true: red, false: black
    struct _RBTree*   tree_;
    void*             data;
}TreeNode;

typedef struct _RBTree
{
    TreeNode*   root_;
    size_t      count_;
    TreeNode*   max_;
    TreeNode*   min_;
    uint16_t    type_len_;
    TypeLess    less_;
}RbTree;


#define rb_black    0
#define rb_red      1

#define rb_left     234
#define rb_right    567


// 使用宏，对算法效率无损。
// 否则即使inline在debug模式也无效
#define IsRed(node) ((node)->color_ == rb_red)
#define IsBlack(node) ((node)->color_ == rb_black)
#define RelinkParent(parent, direction, newnode)    \
{                    \
    if ((parent))                             \
    {                                       \
        if ((direction) == rb_left)           \
            (parent)->left_ = (newnode);        \
        else                                \
            (parent)->right_ = (newnode);       \
    }                                       \
    (newnode)->parent_ = (parent);              \
} 


RbTree* CreateRbTree(uint16_t typelen, TypeLess cmp)
{
    RbTree* rbtree = (RbTree*)malloc(sizeof(RbTree));
    if (!rbtree)
        return NULL;
    memset(rbtree, 0, sizeof(RbTree));
    rbtree->count_ = 0;
    rbtree->root_ = NULL;
    rbtree->type_len_ = typelen;
    rbtree->less_ = cmp;

    return rbtree;
}


TreeNode* __Find(TreeNode* node, PAIR* keypair)
{
    TreeNode* p_node = node;
    RbTree* tree = node->tree_;

    while (p_node)
    {
        if (tree->less_(keypair, p_node->data))
            p_node = p_node->left_;
        else if (tree->less_(p_node->data, keypair))
            p_node = p_node->right_;
        else
            break;
    }
    return p_node;
}


PAIR* Find(RbTree* rbtree, PAIR* keypair)
{
    TreeNode* node = NULL;
    if (!rbtree || !rbtree->root_)
        return NULL;
    if (rbtree->less_(keypair, rbtree->min_->data))
        return NULL;
    if (rbtree->less_(rbtree->max_->data, keypair))
        return NULL;

    node = __Find(rbtree->root_, keypair);
    return node ? node->data : NULL;
}



TreeNode* LastLessOrEnd(TreeNode* node, PAIR* pair, bool* equal)
{
    // rbtree不能为空，以此确保该函数必返回一个节点
    assert(node);

    if (!node->tree_->less_(node->data, pair)) {
        if (!node->tree_->less_(pair, node->data))
        {
            *equal = true;
        }

        TreeNode* leftnode = node->left_;
        if (!leftnode) {
            return node;  // 返回边界节点
        }
        return LastLessOrEnd(node->left_, pair, equal);
    }
    else
    {
        TreeNode* rightnode = node->right_;
        if (!rightnode)
        {
            return node;
        }
        return LastLessOrEnd(rightnode, pair, equal);
    }

    // never to this
    return NULL;
}


static TreeNode* __FindInsertPos(const TreeNode* node, const PAIR* pair, bool* equal)
{
    TypeLess less_func = node->tree_->less_;
    TreeNode* nextnode = node;
    while (nextnode)
    {
        if (!less_func(pair, nextnode->data))
        {
            if (!less_func(nextnode->data, pair))
            {
                *equal = true;
                break;
            }

            if (!nextnode->right_)
                break;
            nextnode = nextnode->right_;
        }
        else
        {
            if (!nextnode->left_) {
                break;
            }
            nextnode = nextnode->left_;
        }
    }

    return nextnode;
}

TreeNode* FirstLargeOrEnd(TreeNode* node, PAIR* pair, bool* equal)
{
    // rbtree不能为空，以此确保该函数必返回一个节点
    assert(node);

    if (!node->tree_->less_(pair, node->data))
    {
        if (!node->tree_->less_(node->data, pair))
        {
            *equal = true;
        }

        TreeNode* rightnode = node->right_;
        if (!rightnode)
        {
            return node;  // 返回边界节点
        }
        return FirstLargeOrEnd(rightnode, pair, equal);
    }
    else
    {
        TreeNode* leftnode = node->left_;
        if (!leftnode) {
            return node;
        }
        return FirstLargeOrEnd(node->left_, pair, equal);
    }
    // never to this
    return NULL;
}


inline static bool NodeLess(const TreeNode* l, const TreeNode* r)
{
    return l->tree_->less_(l->data, r->data);
}

inline static bool NodeEqual(const TreeNode* l, const TreeNode* r)
{
    return !l->tree_->less_(l->data, r->data) && !l->tree_->less_(r->data, l->data);
}

inline static TreeNode* CreateNewNode(RbTree* tree)
{
    TreeNode* newnode = (TreeNode*)malloc(sizeof(TreeNode));
    if (!newnode) {
        return NULL;
    }
    memset(newnode, 0, sizeof(TreeNode));
    newnode->color_ = rb_red;
    newnode->data = malloc(tree->type_len_);
    newnode->tree_ = tree;
    return newnode;
}


inline static TreeNode* SiblingNode(const TreeNode* node)
{
    if (!node->parent_)
        return NULL;
    if (node->parent_->left_ == node)
        return node->parent_->right_;
    return node->parent_->left_;
}

inline static bool IsLeftChild(const TreeNode* child)
{
    //assert(child->parent_);
    if (!child->parent_)
        return false;

    if (child->parent_->left_ == child)
        return true;
    return false;
}


// 判断node是否是2节点
// 必须知道哪个是新插入的节点
inline static bool Is2Node(const TreeNode* node, const TreeNode* newnode)
{
    TreeNode* sibling = SiblingNode(newnode);
    if (IsRed(node))
        return false;

    if (!sibling)
    {
        return true;
    }

    return sibling->color_ == rb_black;
}

inline static bool Is3Node(const TreeNode* tarnode, const TreeNode* newnode)
{
    TreeNode* sibling = SiblingNode(tarnode);
    if (IsRed(tarnode))
    {
        if (sibling)
            return IsBlack(sibling);
        // sibling is NULL
        return true;
    }
    else
    {
        sibling = SiblingNode(newnode);
        if (sibling)
            return IsRed(sibling);
        // sibling is NULL
        return false;
    }

    // never to this
    assert(false);
    return true;
}


// 重新链接阶段调整后的子树的父节点
/*
inline static void RelinkParent(TreeNode* parent, int direction, TreeNode* newnode)
{
    if (parent)
    {
        if (direction == rb_left)
            parent->left_ = newnode;
        else
            parent->right_ = newnode;
    }
    newnode->parent_ = parent;
    return;
}*/


inline static TreeNode* Insert2Node(TreeNode* tarnode, TreeNode* newnode)
{
    TreeNode* retnode = NULL;
    TreeNode* parent = tarnode->parent_;
    int direction = IsLeftChild(tarnode) ? rb_left : rb_right;

    if (!IsLeftChild(newnode)) {
        tarnode->parent_ = newnode;
        tarnode->right_ = newnode->left_;
        newnode->left_ = tarnode;
        if (tarnode->right_)
            tarnode->right_->parent_ = tarnode;
        newnode->color_ = rb_black;
        tarnode->color_ = rb_red;
        retnode = newnode;
    }
    else
    {
        newnode->color_ = rb_red;
        tarnode->color_ = rb_black;
        retnode = tarnode;
    }

    RelinkParent(parent, direction, retnode);

    return retnode;
}
// 
inline static TreeNode* __Insert3NodeFromLeft(TreeNode* rednode, TreeNode* newnode)
{
    TreeNode* parent = rednode->parent_;
    if (NodeLess(newnode, rednode))
    {
        rednode->left_ = newnode;
        parent->left_ = rednode->right_;
        rednode->right_ = parent;
        
        rednode->right_->parent_ = rednode;
        rednode->left_->parent_ = rednode;
        if (parent->left_) parent->left_->parent_ = parent;

        parent->color_ = rb_red;
        rednode->color_ = rb_black;
        newnode->color_ = rb_red;
        return rednode;
    }
    else
    {
        parent->left_ = newnode->right_;
        rednode->right_ = newnode->left_;

        newnode->left_ = rednode;
        newnode->right_ = parent;

        if (parent->left_)
            parent->left_->parent_ = parent;
        if (rednode->right_)
            rednode->right_->parent_ = rednode;
        newnode->left_->parent_ = newnode;
        newnode->right_->parent_ = newnode;

        parent->color_ = rb_red;
        rednode->color_ = rb_red;
        newnode->color_ = rb_black;
        return newnode;
    }

    // never to this
    return NULL;
}

// 
inline static TreeNode* __Insert3NodeFromRight(TreeNode* rednode, TreeNode* newnode)
{
    TreeNode* parent = rednode->parent_;
    if (NodeLess(newnode, rednode))
    {
        parent->right_ = newnode->left_;
        rednode->left_ = newnode->right_;
        newnode->left_ = parent;
        newnode->right_ = rednode;

        if (parent->right_)
            parent->right_->parent_ = parent;
        if (rednode->left_)
            rednode->left_->parent_ = rednode;
        newnode->left_->parent_ = newnode;
        newnode->right_->parent_ = newnode;

        newnode->color_ = rb_black;
        parent->color_ = rb_red;
        rednode->color_ = rb_red;

        return newnode;
    }
    else
    {
        parent->right_ = rednode->left_;
        rednode->left_ = parent;

        if (parent->right_) parent->right_->parent_ = parent;
        rednode->left_->parent_ = rednode;

        parent->color_ = rb_red;
        rednode->color_ = rb_black;
        newnode->color_ = rb_red;
        return rednode;
    }

    // never to this
    return NULL;
}


/* 插入新节点至一个三节点，不会发生向上扩展 
    tar_node: 三节点中的黑节点
*/
inline static TreeNode* Insert3Node(TreeNode* newnode, TreeNode* insert_pos)
{
    TreeNode* parent = NULL, *pp = NULL;
    if (IsRed(insert_pos)) 
    {
        parent = insert_pos->parent_;
        pp = parent->parent_;
    }
    else
    {
        newnode->color_ = rb_red;
        return insert_pos;
    }

    int direction = IsLeftChild(parent) ? rb_left : rb_right;
    if (parent->left_ == insert_pos)
    {
        parent = __Insert3NodeFromLeft(insert_pos, newnode);
    }
    else
    {
        parent = __Insert3NodeFromRight(insert_pos, newnode);
    }

    RelinkParent(pp, direction, parent);
    return parent;
}


inline static TreeNode* Insert4NodeFromRight(TreeNode* expandnode, TreeNode* rednode)
{
    TreeNode* retnode = NULL;
    if (expandnode == rednode->left_)
    {
        expandnode->color_ = rb_red;
        retnode = rednode;
    }
    else { // expandnode == rednode->right_
        expandnode->parent_ = rednode->parent_;
        expandnode->parent_->right_ = expandnode;

        rednode->right_ = expandnode->left_;
        rednode->parent_ = expandnode;

        expandnode->left_ = rednode;
        if (rednode->right_) rednode->right_->parent_ = rednode;

        rednode->color_ = rb_red;
        expandnode->color_ = rb_black;
        retnode = expandnode;
    }
    
    retnode->parent_->left_->color_ = rb_black;
    retnode->parent_->right_->color_ = rb_black;
    return retnode->parent_;
}
inline static TreeNode* Insert4NodeFromLeft(TreeNode* expandnode, TreeNode* rednode)
{
    TreeNode* retnode = NULL;
    if (expandnode == rednode->left_)
    {
        expandnode->color_ = rb_red;
        retnode = rednode;
    }
    else { // expandnode == rednode->right_

        expandnode->parent_ = rednode->parent_;
        rednode->parent_->left_ = expandnode;

        rednode->right_ = expandnode->left_;
        expandnode->left_ = rednode;

        if (rednode->right_) rednode->right_->parent_ = rednode;
        expandnode->left_->parent_ = expandnode;

        rednode->color_ = rb_red;
        expandnode->color_ = rb_black;
        retnode = expandnode;
    }

    retnode->parent_->left_->color_ = rb_black;
    retnode->parent_->right_->color_ = rb_black;
    return retnode->parent_;
}


/* 4节点插入
    4节点具有平衡性，所以它的情况有限
    1、两个红节点都没有孩子节点
    2、两个红节点都有左右子树
*/
TreeNode* Insert4Node(TreeNode* new_node, TreeNode* red_node)
{
    TreeNode* parent = red_node->parent_;
    TreeNode* pp = parent->parent_;
    TreeNode* retnode = NULL;
    int direction = IsLeftChild(parent) ? rb_left : rb_right;

    if (parent->left_ == red_node)
        retnode = Insert4NodeFromLeft(new_node, red_node);
    else
        retnode = Insert4NodeFromRight(new_node, red_node);

    RelinkParent(pp, direction, retnode);
    return retnode;
}

TreeNode* Makebalance(TreeNode* newnode)
{
    TreeNode* rst = newnode;
    TreeNode* parent = rst->parent_;

    while (true)
    {
        if (!IsRed(parent) && Is2Node(parent, rst))
        {
            rst = Insert2Node(parent, rst);
            break;
        }

        if (Is3Node(parent, rst))
        {// 在3节点的从节色插入
            rst = Insert3Node(rst, parent);
            break;
        }
        // 插入四节点
        rst = Insert4Node(rst, parent);

        if (rst->parent_ == NULL)
            break;
        parent = rst->parent_;
    }

    return rst;
}

inline static void UpdateTree(RbTree* rbtree, TreeNode* root, TreeNode* newnode)
{
    newnode->tree_ = rbtree;
    rbtree->count_++;
    if (root)
        rbtree->root_ = root;
    if (!rbtree->min_ || NodeLess(newnode, rbtree->min_))
        rbtree->min_ = newnode;
    if (!rbtree->max_ || NodeLess(rbtree->max_, newnode))
        rbtree->max_ = newnode;
    
    return;
}

TreeNode* FindInsertPos(const RbTree* tree, const PAIR* pair, bool* equal)
{
    TreeNode* node = NULL;
    bool flag_equal = false;
    *equal = false;

    if (tree->less_(pair, tree->min_->data))
        return tree->min_;

    if (tree->less_(tree->max_->data, pair))
        return tree->max_;

    return __FindInsertPos(tree->root_, pair, equal);
}


TreeNode* Insert(RbTree* rbtree, PAIR* pair)
{
    TreeNode* insert_pos = NULL;
    TreeNode* new_node = NULL;
    if (!rbtree) {
        return NULL;
    }
    // 插入根节点
    if (!rbtree->root_)
    {
        new_node = CreateNewNode(rbtree);
        memcpy(new_node->data, pair, rbtree->type_len_);
        new_node->color_ = rb_black;
        UpdateTree(rbtree, new_node, new_node);
        return new_node;
    }

    /* 思路错了？ 应该优先往叶子节点上插入？
        查找到一个合适的插入点，但是这个点不一定比pair大，可能等于，也可能小于
        尝试先判断，根据判断结果来决定把新节点链到左边还是右边，然后再调整平衡？
    */
    bool flag_equal = false;
    insert_pos = FindInsertPos(rbtree, pair, &flag_equal);

    // 获取过程中发现与pair相等的节点
    if (flag_equal)
        return insert_pos;

    new_node = CreateNewNode(rbtree);
    memcpy(new_node->data, pair, rbtree->type_len_);

    // 先给它接上去
    if (NodeLess(new_node, insert_pos))
        insert_pos->left_ = new_node;
    else
        insert_pos->right_ = new_node;
    // 被优化了。。。。怎么办
    new_node->parent_ = insert_pos;

    TreeNode* finalchange = Makebalance(new_node);
    UpdateTree(
        rbtree, 
        finalchange->parent_ == NULL ? finalchange : NULL, 
        new_node);

    return new_node;
}









static TreeNode* g_array[10000] = { 0 };

int increase(int* x)
{
    *x += 1;
    if (*x >= 10000)
        *x = 0;
    return *x;
}

void WalkTreeAsLevel(RbTree* tree)
{
    int cursor = 0, next = cursor + 1;
    int queue_tail = cursor + 2;
    g_array[cursor] = tree->root_;
    g_array[next] = 0;
    while (g_array[cursor] != 0)
    {
        printf("<%p, %p> ", g_array[cursor]->data, g_array[cursor]->data);

        if (g_array[cursor]->left_) {
            g_array[queue_tail] = g_array[cursor]->left_;
            increase(&queue_tail);
        }
        if (g_array[cursor]->right_) {
            g_array[queue_tail] = g_array[cursor]->right_;
            increase(&queue_tail);
        }

        increase(&cursor);
        if (g_array[cursor] == 0) {
            increase(&cursor);
            printf("\n");
            g_array[queue_tail] = 0;
            increase(&queue_tail);
        }
    }
    return;
}




