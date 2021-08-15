
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


// �ж�node�Ƿ���2�ڵ�
// ����֪���ĸ����²���Ľڵ�
inline static bool Is2Node(const TreeNode* node, const TreeNode* newnode)
{
    TreeNode* sibling = SiblingNode(newnode);
    if (node->color_ == rb_red)
        return false;

    if (!sibling)
    {
        return true;
    }

    return sibling->color_ == rb_black;
}

inline static bool Is3Node(TreeNode* tarnode, TreeNode* newnode)
{
    if (tarnode->color_ == rb_red)
    {
        TreeNode* sibling = SiblingNode(tarnode);
        if (sibling)
            return sibling->color_ == rb_black;
        // sibling is NULL
        return true;
    }
    else
    {
        TreeNode* sibling = SiblingNode(newnode);
        if (sibling)
            return sibling->color_ == rb_red;
        // sibling is NULL
        return false;
    }

    // never to this
    assert(false);
    return true;
}


// �������ӽ׶ε�����������ĸ��ڵ�
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

    // RelinkParent(parent, direction, retnode);
    if (parent)
    {
        if (direction == rb_left)
            parent->left_ = retnode;
        else
            parent->right_ = retnode;
    }
    retnode->parent_ = parent;

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


/* �����½ڵ���һ�����ڵ㣬���ᷢ��������չ 
    tar_node: ���ڵ��еĺڽڵ�
*/
inline static TreeNode* Insert3Node(TreeNode* newnode, TreeNode* insert_pos)
{
    TreeNode* parent = NULL, *pp = NULL;
    if (insert_pos->color_ == rb_red) {
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

    // RelinkParent(pp, direction, parent);
    if (pp)
    {
        if (direction == rb_left)
            pp->left_ = parent;
        else
            pp->right_ = parent;
    }
    parent->parent_ = pp;

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


/* 4�ڵ����
    4�ڵ����ƽ���ԣ����������������
    1��������ڵ㶼û�к��ӽڵ�
    2��������ڵ㶼����������
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

    // RelinkParent(pp, direction, retnode);
    if (pp)
    {
        if (direction == rb_left)
            pp->left_ = retnode;
        else
            pp->right_ = retnode;
    }
    retnode->parent_ = pp;
    return retnode;
}

TreeNode* Makebalance(TreeNode* insert_pos, TreeNode* newnode)
{
    TreeNode* parent = insert_pos->parent_;
    TreeNode* pp = parent ? parent->parent_ : NULL;
    TreeNode* sibling = SiblingNode(insert_pos);
    TreeNode* rst = NULL;

    int direction = rb_left;

    if (Is2Node(insert_pos, newnode))
    {
        rst = Insert2Node(insert_pos, newnode);
        return rst;
    }
    
    if (Is3Node(insert_pos, newnode))
    {// ��3�ڵ�Ĵӽ�ɫ����
        rst = Insert3Node(newnode, insert_pos);
        return rst;
    }
    // �����Ľڵ�
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

    return FirstLargeOrEnd(tree->root_, pair->key, equal);
}


TreeNode* Insert(RbTree* rbtree, PAIR* pair)
{
    TreeNode* insert_pos = NULL;
    TreeNode* new_node = NULL;
    if (!rbtree) {
        return NULL;
    }
    // ������ڵ�
    if (!rbtree->root_)
    {
        new_node = CreateNewNode();
        memcpy(&new_node->pair_, pair, sizeof(PAIR));
        new_node->color_ = rb_black;
        UpdateTree(rbtree, new_node, new_node);
        return new_node;
    }

    /* ˼·���ˣ� Ӧ��������Ҷ�ӽڵ��ϲ��룿
        ���ҵ�һ�����ʵĲ���㣬��������㲻һ����pair�󣬿��ܵ��ڣ�Ҳ����С��
        �������жϣ������жϽ�����������½ڵ�������߻����ұߣ�Ȼ���ٵ���ƽ�⣿
    */
    bool flag_equal = false;
    insert_pos = FindInsertPos(rbtree, pair, &flag_equal);

    // ��ȡ�����з�����pair��ȵĽڵ�
    if (flag_equal)
        return insert_pos;

    new_node = CreateNewNode();
    memcpy(&new_node->pair_, pair, sizeof(PAIR));

    // �ȸ�������ȥ
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


// 
TreeNode* Insert_test(RbTree* rbtree, PAIR* pair)
{
    TreeNode* insert_pos = NULL;
    TreeNode* new_node = NULL;
    if (!rbtree) {
        return NULL;
    }
    // ������ڵ�
    if (!rbtree->root_)
    {
        new_node = CreateNewNode();
        memcpy(&new_node->pair_, pair, sizeof(PAIR));
        new_node->color_ = rb_black;
        rbtree->count_ = 1;
        rbtree->root_ = new_node;
        return new_node;
    }

    /* ˼·���ˣ� Ӧ��������Ҷ�ӽڵ��ϲ��룿
        ���ҵ�һ�����ʵĲ���㣬��������㲻һ����pair�󣬿��ܵ��ڣ�Ҳ����С��
        �������жϣ������жϽ�����������½ڵ�������߻����ұߣ�Ȼ���ٵ���ƽ�⣿
    */
    bool flag_equal = false;
    insert_pos = FirstLargeOrEnd(rbtree->root_, pair->key, &flag_equal);
    
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

    TreeNode* sibling = SiblingNode(insert_pos);
    if (insert_pos->color_ == rb_black) {
        Insert2Node(insert_pos, new_node);
        return new_node;
    }


    // ����insert_pos�Ǻ�ɫ�ģ���ɫ�������Ǹ��ڵ㣩
    // insert_pos����������ڵ㣬�ֿ������ҽڵ�
    // ���insert_pos���û�����ֵܽڵ�
    if (!insert_pos->parent_->right_)
    {
        // û�����ֵܽڵ㣬���� pair < insert_pos < insert_pos->parent
        // ��ת��ʹinsert_pos��Ϊ���ڵ�
        
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

    }

    // ����Ԫ�ض���pairС
    if (PairLess(&insert_pos->pair_, pair)) {

    }

    return NULL;
}




