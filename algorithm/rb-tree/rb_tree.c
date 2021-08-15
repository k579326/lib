
#include "rb_tree.h"

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
    PAIR              pair_;
}TreeNode;

typedef struct _RBTree
{
    TreeNode*   root_;
    size_t      count_;
    TreeNode*   max_;
    TreeNode*   min_;
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
    
inline static bool less(int l, int r)
{
    return l < r;
}

RbTree* CreateRbTree()
{
    RbTree* rbtree = (RbTree*)malloc(sizeof(RbTree));
    if (!rbtree)
        return NULL;
    memset(rbtree, 0, sizeof(RbTree));
    rbtree->count_ = 0;
    rbtree->root_ = NULL;
    return rbtree;
}


TreeNode* __Find(TreeNode* node, int key)
{
    TreeNode* p_node = node;

    while (p_node)
    {
        if (less(key, p_node->pair_.key))
            p_node = p_node->left_;
        else if (less(p_node->pair_.key, key))
            p_node = p_node->right_;
        else
            break;
    }
    return p_node;
}

/*
TreeNode* __Find(TreeNode* node, int key)
{
    if (!node) {
        return NULL;
    }
    if (less(key, node->pair_.key)) {
        return __Find(node->left_, key);
    }
    else if (less(node->pair_.key, key))
    {
        return __Find(node->right_, key);
    }

    return node;
}
*/

PAIR* Find(RbTree* rbtree, int key)
{
    TreeNode* node = NULL;
    if (!rbtree || !rbtree->root_)
        return NULL;
    if (less(key, rbtree->min_->pair_.key))
        return NULL;
    if (less(rbtree->max_->pair_.key, key))
        return NULL;

    node = __Find(rbtree->root_, key);
    return node ? &node->pair_ : NULL;
}



TreeNode* LastLessOrEnd(TreeNode* rbtree, int key, bool* equal)
{
    // rbtree不能为空，以此确保该函数必返回一个节点
    assert(rbtree);

    if (!less(rbtree->pair_.key, key)) {
        if (!less(key, rbtree->pair_.key))
        {
            *equal = true;
        }

        TreeNode* leftnode = rbtree->left_;
        if (!leftnode) {
            return rbtree;  // 返回边界节点
        }
        return LastLessOrEnd(rbtree->left_, key, equal);
    }
    else
    {
        TreeNode* rightnode = rbtree->right_;
        if (!rightnode)
        {
            return rbtree;
        }
        return LastLessOrEnd(rightnode, key, equal);
    }

    // never to this
    return NULL;
}


static TreeNode* __FindInsertPos(TreeNode* rbtree, int key, bool* equal)
{
    TreeNode* nextnode = rbtree;
    while (nextnode)
    {
        if (!less(key, nextnode->pair_.key))
        {
            if (!less(nextnode->pair_.key, key))
            {
                *equal = true;
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

TreeNode* FirstLargeOrEnd(TreeNode* rbtree, int key, bool* equal)
{
    // rbtree不能为空，以此确保该函数必返回一个节点
    assert(rbtree);

    if (!less(key, rbtree->pair_.key)) 
    {
        if (!less(rbtree->pair_.key, key))
        {
            *equal = true;
        }

        TreeNode* rightnode = rbtree->right_;
        if (!rightnode)
        {
            return rbtree;  // 返回边界节点
        }
        return FirstLargeOrEnd(rightnode, key, equal);
    }
    else
    {
        TreeNode* leftnode = rbtree->left_;
        if (!leftnode) {
            return rbtree;
        }
        return FirstLargeOrEnd(rbtree->left_, key, equal);
    }
    // never to this
    return NULL;
}


inline static bool PairLess(const PAIR* l, const PAIR* r)
{
    return less(l->key, r->key);
}
inline static bool PairEqual(const PAIR* l, const PAIR* r)
{
    return !less(l->key, r->key) && !less(r->key, l->key);
}


inline static bool NodeLess(const TreeNode* l, const TreeNode* r)
{
    return less(l->pair_.key, r->pair_.key);
}

inline static bool NodeEqual(const TreeNode* l, const TreeNode* r)
{
    return !less(l->pair_.key, r->pair_.key) && !less(r->pair_.key, l->pair_.key);
}

inline static TreeNode* CreateNewNode()
{
    TreeNode* newnode = (TreeNode*)malloc(sizeof(TreeNode));
    if (!newnode) {
        return NULL;
    }
    memset(newnode, 0, sizeof(TreeNode));
    newnode->color_ = rb_red;
    return newnode;
}


inline static TreeNode* SiblingNode(TreeNode* node)
{
    if (!node->parent_)
        return NULL;
    if (node->parent_->left_ == node)
        return node->parent_->right_;
    return node->parent_->left_;
}

inline static bool IsLeftChild(TreeNode* child)
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

inline static bool Is3Node(TreeNode* tarnode, TreeNode* newnode)
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

TreeNode* Makebalance(TreeNode* insert_pos, TreeNode* newnode)
{
    TreeNode* parent = insert_pos->parent_;
    TreeNode* pp = parent ? parent->parent_ : NULL;
    TreeNode* sibling = SiblingNode(insert_pos);
    TreeNode* rst = NULL;

    int direction = rb_left;

    if (!IsRed(insert_pos) && Is2Node(insert_pos, newnode))
    {
        rst = Insert2Node(insert_pos, newnode);
        return rst;
    }
    
    if (Is3Node(insert_pos, newnode))
    {// 在3节点的从节色插入
        rst = Insert3Node(newnode, insert_pos);
        return rst;
    }
    // 插入四节点
    rst = Insert4Node(newnode, insert_pos);

    if (rst->parent_ == NULL)
        return rst;
    return Makebalance(rst->parent_, rst);
}

inline static void UpdateTree(RbTree* rbtree, TreeNode* root, TreeNode* newnode)
{
    rbtree->count_++;
    if (root)
        rbtree->root_ = root;
    if (!rbtree->min_ || NodeLess(newnode, rbtree->min_))
        rbtree->min_ = newnode;
    if (!rbtree->max_ || NodeLess(rbtree->max_, newnode))
        rbtree->max_ = newnode;
    
    return;
}

TreeNode* FindInsertPos(RbTree* tree, PAIR* pair, bool* equal)
{
    TreeNode* node = NULL;
    bool flag_equal = false;
    *equal = false;

    if (PairLess(pair, &tree->min_->pair_))
        return tree->min_;
    else if (!PairLess(&tree->min_->pair_, pair))
    {
        // equal
        *equal = true;
        return tree->min_;
    }

    if (PairLess(&tree->max_->pair_, pair))
        return tree->max_;
    else if (!PairLess(pair, &tree->max_->pair_))
    {
        // equal
        *equal = true;
        return tree->max_;
    }

    return __FindInsertPos(tree->root_, pair->key, equal);
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
        new_node = CreateNewNode();
        memcpy(&new_node->pair_, pair, sizeof(PAIR));
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

    new_node = CreateNewNode();
    memcpy(&new_node->pair_, pair, sizeof(PAIR));

    // 先给它接上去
    if (NodeLess(new_node, insert_pos))
        insert_pos->left_ = new_node;
    else
        insert_pos->right_ = new_node;
    new_node->parent_ = insert_pos;

    TreeNode* finalchange = Makebalance(insert_pos, new_node);
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
        printf("<%d, %p> ", g_array[cursor]->pair_.key, g_array[cursor]->pair_.v_);

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




