
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

/* �游�ڵ��ڸú���ǰ�ȱ��棬���غ������� */
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

/* �����½ڵ���һ�����ڵ㣬���ᷢ��������չ 
    tar_node: ���ڵ��еĺڽڵ�
*/
void Insert3Node(TreeNode* new_node, TreeNode* black, TreeNode* red)
{
    // �������ڵ㣬����tar_node���丸�ڵ��Ǵ��ڵ�
    TreeNode* array[3] = { new_node, red, black };
    TreeNode* pp = black->parent_;
    bool left = (pp && IsLeftChild(black));
    TreeNode* tmp;
    // ����
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

    // �ڽڵ�û�仯�����ղ�ѯ�����Ĺ���˵������Ľڵ�����нڵ㶼��
    if (black == array[1]) {
        // newnode��켴��
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



// ���������²���λ�ó���4�ڵ�����
TreeNode* Split4Node(RbTree* rbtree, TreeNode* new_node, TreeNode* target_node)
{
    TreeNode* parent = target_node->parent_;
    TreeNode* sibling = SiblingNode(target_node);
    TreeNode* ret = NULL;
    // target_node ���Һ���
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

        // ���ڵ�Ҫ�ı���
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
    // ���˾�� insert_pos
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
        parent->left_ = NULL;
        parent->right_ = NULL;
        AddLeftNode(insert_pos, new_node);
        return new_node;
    }
    else
    {// ���ֵܽڵ���ڣ������������1��insert_pos����(����������ұ�����)��2�����ֵܴ��ڣ���insert_pos����

     // ��û�п�������Һ��أ������ܣ�����Һ�˵�����ǵĸ��ڵ���һ��3�ڵ㣬���ֵ��Ǻ�ɫ��
     // ˵������3�ڵ�ĺ��ӽڵ㣬���3�ڵ��Ȼ���������������ӽڵ㣬Ӧ���ں�ɫ���ֵ��ϣ����ǲ����ܵģ�
     // ��Ϊ��ɫ�ڵ��ǲ��ҵ��Ĳ���ڵ㣬��һ������ͬʱ����������
      
     // ��������1��2��������������֣�insert_pos, insert_pos->parent, sibing of inser_pos
     // �������ڵ��Ȼ�ǹ�����һ��4������4�ڵ㣬������һ������
        
     // �²���Ľڵ�Ҫô���Ҫô��С����ô4���ڵ�Ĵ�С��ϵ���£�
     // sibing < parent < insertnode < newnode������ newnode < insertnode < parent < sibing
     // ����4������չ�����������еڶ�λ�Ľڵ㽫������չ

        TreeNode* newparent = Split4Node(rbtree, new_node, insert_pos);

    }

    // ����Ԫ�ض���pairС
    if (PairLess(&insert_pos->pair_, pair)) {

    }

    return NULL;
}




