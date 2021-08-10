
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
    // rbtree����Ϊ�գ��Դ�ȷ���ú����ط���һ���ڵ�
    assert(rbtree);

    if (!less(rbtree->pair_.key, key)) {
        if (!less(key, rbtree->pair_.key))
        {
            *equal = true;
        }

        TreeNode* leftnode = rbtree->left_;
        if (!leftnode) {
            return rbtree;  // ���ر߽�ڵ�
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
    // rbtree����Ϊ�գ��Դ�ȷ���ú����ط���һ���ڵ�
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
            return rbtree;  // ���ر߽�ڵ�
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

    /* ˼·���ˣ� Ӧ��������Ҷ�ӽڵ��ϲ��룿
        ���ҵ�һ�����ʵĲ���㣬��������㲻һ����pair�󣬿��ܵ��ڣ�Ҳ����С��
        �������жϣ������жϽ�����������½ڵ�������߻����ұߣ�Ȼ���ٵ���ƽ�⣿
    */
    bool flag_equal = false;
    insert_pos = FirstLargeOrEnd(rbtree, pair->key, &flag_equal);
    
    // ��ȡ�����з�����pair��ȵĽڵ�
    if (flag_equal)
        return NULL;
    
    new_node = CreateNewNode();
    memcpy(&new_node->pair_, pair, sizeof(PAIR));
    // insert_pos�϶�û���ҽڵ㣬
    // ��Ϊ�ڵ�ȫ������б��insert_pos�ֿ϶���Ҷ�ӽڵ�
    // ����ֻ�ж��Ƿ������ڵ�

    // ��ڵ�Ҳ�����ж��ˣ�һ��û��
    // insert_pos�ǵ�һ����pair��Ľڵ㣬˵����û��������
    if (insert_pos->left_)
    {
        // ˳��һ���� insert_pos > pair > insert_pos->left
    }

    if (insert_pos->color_ == rb_black) {
        AddLeftNode(insert_pos, new_node);
        return new_node;
    }

    // ����insert_pos�Ǻ�ɫ�ģ���ɫ�������Ǹ��ڵ㣩
    // insert_pos����������ڵ㣬�ֿ������ҽڵ�
    // ���insert_pos���û�����ֵܽڵ�
    if (!insert_pos->parent_->right_)
    {
        // û�����ֵܽڵ㣬���� pair < insert_pos < insert_pos->parent
        // ��ת��ʹinsert_pos��Ϊ���ڵ�
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
    {// ���ֵܽڵ���ڣ������������1��insert_pos����(����������ұ�����)��2�����ֵܴ��ڣ���insert_pos����
     // ������������������֣�insert_pos, insert_pos->parent, sibing of inser_pos
     // �������ڵ��Ȼ�ǹ�����һ��4������4�ڵ㣬������һ������
        
     // ����²���Ľڵ����insert_pos�ĸ��ڵ㣬��ô4���ڵ�Ĵ�С��ϵ���£�
     // sibing < parent < newnode < sibing������ sibling < newnode < parent < sibing
     // ����4������չ�����������еڶ�λ�Ľڵ㽫������չ

    }

    // ����Ԫ�ض���pairС
    if (PairLess(&insert_pos->pair_, pair)) {

    }

    return NULL;
}
