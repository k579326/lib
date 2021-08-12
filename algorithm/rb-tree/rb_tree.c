
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

RbTree* CreateRbTree()
{
    TreeNode* node = CreateNewNode();
    memset(node, 0, sizeof(TreeNode));
    node->color_ = rb_white;
    return node;
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
static TreeNode* SiblingNode(TreeNode* node)
{
    if (!node->parent_)
        return NULL;
    if (node->parent_->left_ == node)
        return node->parent_->right_;
    return node->parent_->left_;
}

static bool IsLeftChild(TreeNode* child)
{
    assert(child->parent_);
    if (child->parent_->left_ == child)
        return true;
    return false;
}

/* 祖父节点在该函数前先保存，返回后再设置 */
void __ChangeParentOf3Node(TreeNode* first, TreeNode* second, TreeNode* third)
{
    TreeNode* left = NULL;
    TreeNode* right = NULL;
    TreeNode* pp = NULL;
    if (second->left_ == first)
    {
        pp = third->parent_;
        right = second->right_;
        second->right_ = third;
        second->parent_ = pp;

        third->left_ = right;
        third->left_->parent_ = third;
        third->parent_ = second;

        first->color_ = rb_red;
        second->color_ = rb_black;
        third->color_ = rb_red;
        return;
    }

    if (second->right_ == third)
    {
        pp = first->parent_;
        left = second->left_;

        second->left_ = first;
        second->parent_ = pp;

        first->right_ = left;
        first->right_->parent_ = first;
        first->parent_ = second;

        first->color_ = rb_red;
        second->color_ = rb_black;
        third->color_ = rb_red;
        return;
    }

    left = second->left_;
    right = second->right_;

    second->left_ = first;
    second->right_ = third;
    first->parent_ = second;
    third->parent_ = second;

    left->parent_ = first;
    first->right_ = left;
    right->parent_ = third;
    third->left_ = right;

    first->color_ = rb_red;
    second->color_ = rb_black;
    third->color_ = rb_red;
    return;
}

/* 插入新节点至一个三节点，不会发生向上扩展 
    tar_node: 三节点中的黑节点
*/
void Insert3Node(TreeNode* new_node, TreeNode* black, TreeNode* red)
{
    // 插入三节点，至少tar_node及其父节点是存在的
    TreeNode* array[3] = { new_node, red, black };
    TreeNode* pp = black->parent_;
    bool left = (pp && IsLeftChild(black));
    TreeNode* tmp;
    // 排序
    for (int i = 0; i < 3; i++)
    {
        for (int j = i; j < 3; j++)
        {
            if (NodeLess(array[j], array[i])) {
                tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
    }

    // 黑节点没变化，按照查询插入点的规则，说明插入的节点比所有节点都大
    if (black == array[1]) {
        // newnode变红即可
        new_node->color_ = rb_red;
        return;
    }

    __ChangeParentOf3Node(array[0], array[1], array[2]);

    // pp
    if (!pp) {
        array[1]->parent_ = NULL;
        return;
    }
    array[1]->parent_ = pp;
    if (left)
    {
        pp->left_ = array[1];
    }
    else {
        pp->right_ = array[1];
    }
    return;
}



// 仅适用于新插入位置出现4节点的情况
TreeNode* Split4Node(RbTree* rbtree, TreeNode* new_node, TreeNode* target_node)
{
    TreeNode* parent = target_node->parent_;
    TreeNode* sibling = SiblingNode(target_node);
    TreeNode* ret = NULL;
    // target_node 是右孩子
    if (NodeLess(parent, target_node)) {
        parent->right_ = new_node;
        new_node->parent_ = parent;
        new_node->left_ = target_node;
        target_node->parent_ = new_node;
        target_node->left_ = NULL;
        target_node->right_ = NULL;
        target_node->color_ = rb_red;
        new_node->color_ = rb_black;
        sibling->color_ = rb_black;
        parent->color_ = rb_black;

        ret = parent;
    }
    else {

        // 父节点要改变了
        if (parent->parent_ == rbtree->left_)
        {
            rbtree->left_ = target_node;
        }
        else
        {
            if (parent->parent_->left_ == parent)
                parent->parent_->left_ = target_node;
            else
                parent->parent_->right_ = target_node;
        }

        target_node->left_ = new_node;
        target_node->right_ = sibling;

        new_node->parent_ = target_node;
        sibling->parent_ = target_node;
        sibling->left_ = parent;
        sibling->right_ = NULL;
        parent->parent_ = sibling;
        parent->left_ = NULL;
        parent->right_ = NULL;
        
        target_node->color_ = rb_black;
        new_node->color_ = rb_black;
        parent->color_ = rb_red;
        sibling->color_ = rb_black;

        ret = target_node;
    }
    return ret;
}


TreeNode* Insert(RbTree* rbtree, PAIR* pair)
{
    TreeNode* insert_pos = NULL;
    TreeNode* new_node = NULL;
    if (!rbtree) {
        return NULL;
    }
    if (rbtree->color_ == rb_white &&
        rbtree->left_ == NULL)
    {
        new_node = CreateNewNode();
        memcpy(&new_node->pair_, pair, sizeof(PAIR));
        new_node->left_ = NULL;
        new_node->right_ = NULL;
        new_node->parent_ = NULL;
        new_node->color_ = rb_black;
        rbtree->left_ = new_node;
        return new_node;
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
    // 光杆司令 insert_pos
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
        parent->left_ = NULL;
        parent->right_ = NULL;
        AddLeftNode(insert_pos, new_node);
        return new_node;
    }
    else
    {// 右兄弟节点存在，分两种情况：1、insert_pos在右(左倾规则，有右必有左)；2、右兄弟存在，而insert_pos在左

     // 有没有可能左红右黑呢？不可能，左红右黑说明他们的父节点是一个3节点，右兄弟是黑色，
     // 说明它是3节点的孩子节点，这个3节点必然存在另外两个孩子节点，应该在红色左兄弟上，这是不可能的，
     // 因为红色节点是查找到的插入节点，它一定不会同时有两个孩子
      
     // 所以无论1、2这两种情况的哪种，insert_pos, insert_pos->parent, sibing of inser_pos
     // 这三个节点必然是构成了一个4阶树中4节点，所以是一黑两红
        
     // 新插入的节点要么最大，要么最小，那么4个节点的大小关系如下：
     // sibing < parent < insertnode < newnode，否则 newnode < insertnode < parent < sibing
     // 根据4阶树扩展规则，升序排列第二位的节点将向上扩展

        TreeNode* newparent = Split4Node(rbtree, new_node, insert_pos);

    }

    // 所有元素都比pair小
    if (PairLess(&insert_pos->pair_, pair)) {

    }

    return NULL;
}




