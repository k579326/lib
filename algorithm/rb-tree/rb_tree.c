
#include "rb_tree.h"

#include <assert.h>
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
    char              color_;       // true: red, false: black
    PAIR              pair_;
}TreeNode;

#define rb_black    0
#define rb_red      1
#define rb_white    -1      // use for create root node only

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

static TreeNode* CreateNewNode()
{
    TreeNode* newnode = (TreeNode*)malloc(sizeof(TreeNode));
    if (!newnode) {
        return NULL;
    }
    memset(newnode, 0, sizeof(TreeNode));
    newnode->color_ = rb_red;
    return newnode;
}

static void AddLeftNode(TreeNode* parent, TreeNode* left)
{
    parent->left_ = left;
    left->parent_ = parent;
}


TreeNode* Insert(TreeNode* rbtree, PAIR* pair)
{
    TreeNode* insert_pos = NULL;
    TreeNode* new_node = NULL;
    if (!rbtree) {
        return NULL;
    }

    /* 思路错了？ 应该优先往叶子节点上插入？
        查找到一个合适的插入点，但是这个点不一定比pair大，可能等于，也可能小于
        尝试先判断，根据判断结果来决定把新节点链到左边还是右边，然后再调整平衡？
    */
    bool flag_equal = false;
    insert_pos = FirstLargeOrEnd(rbtree, pair->key, &flag_equal);
    
    // 获取过程中发现与pair相等的节点
    if (flag_equal)
        return NULL;
    
    new_node = CreateNewNode();
    memcpy(&new_node->pair_, pair, sizeof(PAIR));
    // insert_pos肯定没有右节点，
    // 因为节点全部左倾斜，insert_pos又肯定是叶子节点
    // 所以只判断是否存在左节点

    // 左节点也不用判断了，一定没有
    // insert_pos是第一个比pair大的节点，说明它没有左子树
    if (insert_pos->left_)
    {
        // 顺序一定是 insert_pos > pair > insert_pos->left
    }

    if (insert_pos->color_ == rb_black) {
        AddLeftNode(insert_pos, new_node);
        return new_node;
    }

    // 现在insert_pos是红色的（红色不可能是根节点）
    // insert_pos即可能是左节点，又可能是右节点
    // 检测insert_pos如果没有右兄弟节点
    if (!insert_pos->parent_->right_)
    {
        // 没有右兄弟节点，所以 pair < insert_pos < insert_pos->parent
        // 旋转，使insert_pos变为父节点
        TreeNode* parent = insert_pos->parent_;
        TreeNode* gparent = insert_pos->parent_->parent_;
        if (gparent) 
        {
            if (gparent->left_ == parent)
            {
                gparent->left_ = insert_pos;
            }
            else {
                gparent->right_ = insert_pos;
            }
            insert_pos->parent_ = gparent;
        }
        insert_pos->right_ = parent;
        parent->parent_ = insert_pos;
        insert_pos->color_ = rb_black;
        parent->color_ = rb_red;
        AddLeftNode(insert_pos, new_node);
        return new_node;
    }
    else
    {// 右兄弟节点存在，分两种情况：1、insert_pos在右(左倾规则，有右必有左)；2、右兄弟存在，而insert_pos在左
     // 无论这两种情况的哪种，insert_pos, insert_pos->parent, sibing of inser_pos
     // 这三个节点必然是构成了一个4阶树中4节点，所以是一黑两红
        
     // 如果新插入的节点大于insert_pos的父节点，那么4个节点的大小关系如下：
     // sibing < parent < newnode < sibing，否则 sibling < newnode < parent < sibing
     // 根据4阶树扩展规则，升序排列第二位的节点将向上扩展

    }

    // 所有元素都比pair小
    if (PairLess(&insert_pos->pair_, pair)) {

    }

    return NULL;
}
