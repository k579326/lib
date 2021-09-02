
#include "rb_tree.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _treenode
{
    void*             data;     // 放在第一个位置，方便遍历查找时返回

    struct _treenode* parent_;
    struct _treenode* left_;
    struct _treenode* right_;
    char              color_;       // true: red, false: black
    // struct _RBTree*   tree_;
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
// #define RelinkParent(parent, direction, newnode)    \
// {                    \
//     if ((parent))                             \
//     {                                       \
//         if ((direction) == rb_left)           \
//             (parent)->left_ = (newnode);      \
//         else                                  \
//             (parent)->right_ = (newnode);     \
//     }                                         \
//     else                                      \
//     {                                         \
//         (newnode)->tree_->root_ = (newnode);  \
//         (newnode)->color_ = rb_black;         \
//     }                                         \
//     (newnode)->parent_ = (parent);            \
// } 


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


TreeNode* __Find(TreeNode* node, PAIR* keypair, TypeLess less)
{
    TreeNode* p_node = node;
    while (p_node)
    {
        if (less(keypair, p_node->data))
            p_node = p_node->left_;
        else if (less(p_node->data, keypair))
            p_node = p_node->right_;
        else
            break;
    }
    return p_node;
}


Node* Find(RbTree* rbtree, PAIR* keypair)
{
    TreeNode* node = NULL;
    if (!rbtree || !rbtree->root_)
        return NULL;
    //if (rbtree->less_(keypair, rbtree->min_->data))
    //    return NULL;
    //if (rbtree->less_(rbtree->max_->data, keypair))
    //    return NULL;

    node = __Find(rbtree->root_, keypair, rbtree->less_);
    return node ? (Node*)node : NULL;
}

inline static const TreeNode* __FindInsertPos(const TreeNode* node, const PAIR* pair, TypeLess less, bool* equal)
{
    const TreeNode* nextnode = node;
    while (nextnode)
    {
        if (!less(pair, nextnode->data))
        {
            if (!less(nextnode->data, pair))
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


inline static TreeNode* CreateNewNode(RbTree* tree)
{
    TreeNode* newnode = (TreeNode*)malloc(sizeof(TreeNode));
    if (!newnode) {
        return NULL;
    }
    memset(newnode, 0, sizeof(TreeNode));
    newnode->color_ = rb_red;
    newnode->data = malloc(tree->type_len_);
    // newnode->tree_ = tree;
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
}


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
    if (newnode == rednode->left_)
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
    if (newnode == rednode->left_)
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

inline static TreeNode* Makebalance(TreeNode* newnode)
{
    TreeNode* rst = newnode;
    TreeNode* parent = rst->parent_;

    while (true)
    {
        if (Is2Node(parent, rst))
        {
            return Insert2Node(parent, rst);
        }

        if (Is3Node(parent, rst))
        {// 在3节点的从节色插入
            return Insert3Node(rst, parent);
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
    // newnode->tree_ = rbtree;
    rbtree->count_++;
    if (root)
        rbtree->root_ = root;
    
    return;
}

inline static TreeNode* FindInsertPos(const RbTree* tree, const PAIR* pair, bool* equal)
{
    *equal = false;
    return (TreeNode*)__FindInsertPos(tree->root_, pair, tree->less_, equal);
}


Node* InsertNode(RbTree* rbtree, PAIR* pair)
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
        return (Node*)new_node;
    }

    /* 思路错了？ 应该优先往叶子节点上插入？
        查找到一个合适的插入点，但是这个点不一定比pair大，可能等于，也可能小于
        尝试先判断，根据判断结果来决定把新节点链到左边还是右边，然后再调整平衡？
    */
    bool flag_equal = false;
    insert_pos = FindInsertPos(rbtree, pair, &flag_equal);

    // 获取过程中发现与pair相等的节点
    if (flag_equal)
        return (Node*)insert_pos;

    new_node = CreateNewNode(rbtree);
    memcpy(new_node->data, pair, rbtree->type_len_);

    // 先给它接上去
    if (rbtree->less_(new_node->data, insert_pos->data))
        insert_pos->left_ = new_node;
    else
        insert_pos->right_ = new_node;
    new_node->parent_ = insert_pos;

    TreeNode* finalchange = Makebalance(new_node);
    UpdateTree(
        rbtree, 
        finalchange->parent_ == NULL ? finalchange : NULL, 
        new_node);

    return (Node*)new_node;
}


inline static void _DeleteNode(RbTree* tree, TreeNode* node, bool remove_data)
{
    TreeNode* parent = node->parent_;
    if (!parent)
    {
        tree->root_ = NULL;
        tree->max_ = NULL;
        tree->min_ = NULL;
    }
    else
    {
        if (IsLeftChild(node))
            parent->left_ = NULL;
        else
            parent->right_ = NULL;
    }

    if (remove_data)
        free(node->data);
    free(node);
    return;
}

static TreeNode* _Next(TreeNode* node)
{
    TreeNode* avater = node;
    if (!node)
        return NULL;
    avater = node->right_;
    while (avater)
    {
        if (!avater->left_)
            break;
        avater = avater->left_;
    }
    if (avater) {
        return avater;
    }

    avater = node;
    while (avater->parent_)
    {
        if (avater == avater->parent_->left_)
        {
            break;
        }
        avater = avater->parent_;
    }
    if (!avater->parent_)
        return NULL;
    
    return avater->parent_;
}

inline static bool HasChildren(TreeNode* node)
{
    return node->left_ || node->right_;
}

Node* DeleteNode(RbTree* rbtree, Node* node)
{
    TreeNode* tar = NULL;
    TreeNode* del_node = NULL;
    TreeNode* retnode = NULL;
    TreeNode* ltree = NULL;
    TreeNode* rtree = NULL;
    TreeNode* parent = NULL;
    TreeNode* sibling = NULL;
    TreeNode* pp = NULL;
    int direction = rb_left;

    if (!rbtree ||
        !rbtree->count_ ||
        !node ||
        !node->data
        ) 
    {
        return (Node*)retnode;
    }

    tar = __Find(rbtree->root_, node->data, rbtree->less_);
    if (tar)
    {
        // 先释放掉这个被删除节点的数据
        free(tar->data);
        tar->data = NULL;
    }
    else {
        return (Node*)retnode;
    }
    rbtree->count_--;

    ltree = tar->left_;
    rtree = tar->right_;

    // 找被删除节点的替代节点
    while (ltree || rtree)
    {
        if (ltree) {
            del_node = ltree;
            ltree = ltree->right_;
        }
        if (rtree) {
            del_node = rtree;
            retnode = rtree;
            rtree = rtree->left_;
        }
    }

    // 计算返回的节点
    if (retnode)
    {
        if (retnode == del_node)
            retnode = tar;
    }
    else {
        retnode = _Next(tar);
    }

    if (del_node) 
    {
        tar->data = del_node->data;
        del_node->data = NULL;
        tar = del_node;
    }
    else {
        del_node = tar;
    }

    // 如果tar还存在孩子节点（最多一个，而且一定是红色）
    // 直接替换，然后这个孩子会被删除
    if (tar->left_) {
        tar->data = tar->left_->data;
        tar->left_->data = NULL;
        tar = tar->left_;
        del_node = tar;
    }
    else if (tar->right_) {
        tar->data = tar->right_->data;
        tar->right_->data = NULL;
        tar = tar->right_;
        del_node = tar;
    }

    if (IsRed(tar))
    {
        _DeleteNode(rbtree, tar, true);
        return (Node*)retnode;
    }

    while (tar->parent_)
    {
        parent = tar->parent_;
        pp = parent->parent_;
        direction = (pp && pp->left_ == parent ? rb_left : rb_right);
        sibling = SiblingNode(tar);
        if (IsRed(sibling))
        {
            if (IsLeftChild(tar))
            {
                parent->right_ = sibling->left_;
                sibling->left_ = parent;
                if (parent->right_) parent->right_->parent_ = parent;
                sibling->left_->parent_ = sibling;
            }
            else {
                parent->left_ = sibling->right_;
                sibling->right_ = parent;
                if (parent->left_) parent->left_->parent_ = parent;
                sibling->right_->parent_ = sibling;
            }
            if (pp)
                RelinkParent(pp, direction, sibling);
            else
            {
                rbtree->root_ = sibling;
                sibling->color_ = rb_black;
                sibling->parent_ = NULL;
            }
            parent->color_ = rb_red;
            sibling->color_ = rb_black;
            // 下一次tar的sibling一定是黑色
            continue;
        }
        if ((sibling->left_ && IsRed(sibling->left_)) ||
            (sibling->right_ && IsRed(sibling->right_))
            )
        {
            TreeNode* rnode = NULL;
            if (IsLeftChild(tar))
            {
                if (sibling->right_ && IsRed(sibling->right_))
                {
                    // 把sibling的右红色节点偏转到左边
                    parent->right_ = sibling->right_;
                    sibling->right_ = sibling->right_->left_;
                    parent->right_->left_ = sibling;
                    if (sibling->right_) sibling->right_->parent_ = sibling;
                    parent->right_->left_->parent_ = parent->right_;
                    
                    // 设置sibling
                    sibling = parent->right_;
                    sibling->left_->color_ = rb_red;
                    sibling->color_ = rb_black;
                }
                // 将sibling的左红色节点去替代parent做左旋转
                rnode = sibling->left_;
                parent->right_ = rnode->left_;
                sibling->left_ = rnode->right_;
                rnode->left_ = parent;
                rnode->right_ = sibling;
                if (parent->right_) parent->right_->parent_ = parent;
                if (sibling->left_) sibling->left_->parent_ = sibling;
                rnode->left_->parent_ = rnode;
                rnode->right_->parent_ = rnode;
            }
            else
            {
                if (sibling->left_ && IsRed(sibling->left_))
                {
                    // 把sibling的左红色节点偏转到右边
                    parent->left_ = sibling->left_;
                    sibling->left_ = sibling->left_->right_;
                    parent->left_->right_ = sibling;
                    if (sibling->left_) sibling->left_->parent_ = sibling;
                    parent->left_->right_->parent_ = parent->left_;

                    // 设置sibling
                    sibling = parent->left_;
                    sibling->right_->color_ = rb_red;
                    sibling->color_ = rb_black;
                }
                // 将sibling的右红色节点去替代parent做右旋转
                rnode = sibling->right_;
                parent->left_ = rnode->right_;
                sibling->right_ = rnode->left_;
                rnode->right_ = parent;
                rnode->left_ = sibling;
                if (parent->left_) parent->left_->parent_ = parent;
                if (sibling->right_) sibling->right_->parent_ = sibling;
                rnode->right_->parent_ = rnode;
                rnode->left_->parent_ = rnode;
            }
            
            int origin_color = parent->color_;
            parent->color_ = rb_black;
            rnode->color_ = rb_red;
            if (pp)
                RelinkParent(pp, direction, rnode);
            else
            {
                rbtree->root_ = rnode;
                rnode->color_ = rb_black;
                rnode->parent_ = NULL;
            }

            tar = rnode;
            if (origin_color == rb_black) {
                continue;
            }
            break;
        }
        if (IsRed(tar->parent_))
        {
            tar->parent_->color_ = rb_black;
            sibling->color_ = rb_red;
            break;
        }

        sibling->color_ = rb_red;
        tar = tar->parent_;
    }

    _DeleteNode(rbtree, del_node, true);
    return (Node*)retnode;
}

Node* GetFirst(const RbTree* rbtree)
{
    TreeNode* cursor = NULL;
    if (!rbtree)
        return NULL;

    if (!rbtree->count_)
        return NULL;

    cursor = rbtree->root_;
    while (cursor->left_)
        cursor = cursor->left_;
    return (Node*)cursor;
}

Node* GetNext(Node* node)
{
    return (Node*)_Next((TreeNode*)node);
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

    printf("\n\n");
    while (g_array[cursor] != 0)
    {
        int key = *(int*)g_array[cursor]->data;
        printf("<%d, %s> ", key, g_array[cursor]->color_ == rb_red ? "red" : "black");

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


bool regular_1(const RbTree* tree)
{
    if (!tree->root_)
        return true;
    return tree->root_->color_ == rb_black;
}


static bool output_high(const TreeNode* tree, int height, int count)
{
    bool rst = true;

    if (!tree)
        return;
    if (tree->color_ == rb_black)
        count++;

    if (!tree->left_ && !tree->right_)
    {
        return count == height;
    }
    if (tree->left_)
        rst = output_high(tree->left_, height, count);
    if (!rst)
        return false;

    if (tree->right_)
        rst = output_high(tree->right_, height, count);
    return rst;
}

// is balance
bool CheckBalance(const RbTree* tree)
{
    TreeNode* next = 0;
    int height = 0, count = 0;
    if (!tree)
        return true;

    next = tree->root_;
    while (next) {
        if (next->color_ == rb_black)
            height++;
        next = next->left_;
    }

    return output_high(tree->root_, height, count);
}


static int calc_node_sum(TreeNode* root)
{
    int leftsum = 0, rightsum = 0;
    if (!root)
        return 0;
    leftsum = calc_node_sum(root->left_);
    rightsum = calc_node_sum(root->right_);

    return leftsum + rightsum + 1;
}
// check node sum
bool CheckNodeSum(const RbTree* tree)
{
    if (!tree)
        return true;

    return tree->count_ == calc_node_sum(tree->root_);
}



