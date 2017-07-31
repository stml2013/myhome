
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


/*
 * The red-black tree code is based on the algorithm described in
 * the "Introduction to Algorithms" by Cormen, Leiserson and Rivest.
 */


static ngx_inline void ngx_rbtree_left_rotate(ngx_rbtree_node_t **root,
    ngx_rbtree_node_t *sentinel, ngx_rbtree_node_t *node);
static ngx_inline void ngx_rbtree_right_rotate(ngx_rbtree_node_t **root,
    ngx_rbtree_node_t *sentinel, ngx_rbtree_node_t *node);


void
ngx_rbtree_insert(ngx_thread_volatile ngx_rbtree_t *tree,
    ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  **root, *temp, *sentinel;

    /* a binary tree insert */

    root = (ngx_rbtree_node_t **) &tree->root;
    sentinel = tree->sentinel;//哨兵节点

    if (*root == sentinel) {//如果根节点为空
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        ngx_rbt_black(node);//根节点设置为黑色
        *root = node;

        return;
    }

    tree->insert(*root, node, sentinel);//插入节点

    /* re-balance tree */  /*插入调整*/

    while (node != *root && ngx_rbt_is_red(node->parent)) {//当前节点不是根节点，并且节点的父节点颜色为红色
		//父节点为祖父节点的左孩子
        if (node->parent == node->parent->parent->left) {
            temp = node->parent->parent->right;//取叔叔节点
            //情况1：父节点为红，叔叔节点为红
            if (ngx_rbt_is_red(temp)) {//解决方法：
                ngx_rbt_black(node->parent);//父节点修改为黑色
                ngx_rbt_black(temp);        //叔叔节点改为黑色
                ngx_rbt_red(node->parent->parent);//祖父节点修改为红色
                node = node->parent->parent;//修改祖父节点为当前节点，重新开始算法

            } else {//情况2：父节点为红，叔叔节点为黑色，当前节点为父节点的右孩子
                if (node == node->parent->right) {//解决方法：
                    node = node->parent;//修改当前节点为父节点
                    ngx_rbtree_left_rotate(root, sentinel, node);//左旋当前节点
                }
                //情况3：父节点为红，叔叔节点为黑色，当前节点为父节点的左孩子
                ngx_rbt_black(node->parent);//解决方法：父节点修改为黑色
                ngx_rbt_red(node->parent->parent);//祖父节点修改为红色
                ngx_rbtree_right_rotate(root, sentinel, node->parent->parent);//右旋祖父节点
            }

        } else {//父节点为祖父节点的右孩子，这也有三种情况，和上述情况操作类似，只不过旋转方向相反
            temp = node->parent->parent->left;

            if (ngx_rbt_is_red(temp)) {
                ngx_rbt_black(node->parent);
                ngx_rbt_black(temp);
                ngx_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    ngx_rbtree_right_rotate(root, sentinel, node);
                }

                ngx_rbt_black(node->parent);
                ngx_rbt_red(node->parent->parent);
                ngx_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    ngx_rbt_black(*root);//根节点永远是黑色
}


void
ngx_rbtree_insert_value(ngx_rbtree_node_t *temp, ngx_rbtree_node_t *node,
    ngx_rbtree_node_t *sentinel)
{
    ngx_rbtree_node_t  **p;

    for ( ;; ) {

        p = (node->key < temp->key) ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }
	//每一个待插入的节点必须初始化为红色
    *p = node;
    node->parent = temp;//初始化父节点
    node->left = sentinel;//初始化左子节点
    node->right = sentinel;//初始化右子节点
    ngx_rbt_red(node);//颜色设置为红色
}


void
ngx_rbtree_insert_timer_value(ngx_rbtree_node_t *temp, ngx_rbtree_node_t *node,
    ngx_rbtree_node_t *sentinel)
{
    ngx_rbtree_node_t  **p;

    for ( ;; ) {

        /*
         * Timer values
         * 1) are spread in small range, usually several minutes,
         * 2) and overflow each 49 days, if milliseconds are stored in 32 bits.
         * The comparison takes into account that overflow.
         */

        /*  node->key < temp->key */

        p = ((ngx_rbtree_key_int_t) node->key - (ngx_rbtree_key_int_t) temp->key
              < 0)
            ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    ngx_rbt_red(node);
}


void
ngx_rbtree_delete(ngx_thread_volatile ngx_rbtree_t *tree,
    ngx_rbtree_node_t *node)
{
    ngx_uint_t           red;
    ngx_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

    /* a binary tree delete */

    root = (ngx_rbtree_node_t **) &tree->root;
    sentinel = tree->sentinel;//哨兵节点

    if (node->left == sentinel) {//左子节点为空
        temp = node->right;
        subst = node;//subst记录后继节点，用着个节点代替删除节点

    } else if (node->right == sentinel) {//右子节点为空
        temp = node->left;
        subst = node;

    } else {//左右子节点都不为空
        subst = ngx_rbtree_min(node->right, sentinel);//右子树中最小的节点

        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }

    if (subst == *root) {//根节点的情况
        *root = temp;
        ngx_rbt_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

        return;
    }

    red = ngx_rbt_is_red(subst);//如果后继是红色，就不需要调整红黑树
                                //因为实际上删除的是后继节点
    /*下面是一些指针的修改，不详细介绍，参看二叉排序树的删除*/
    if (subst == subst->parent->left) {
        subst->parent->left = temp;

    } else {
        subst->parent->right = temp;
    }

    if (subst == node) {

        temp->parent = subst->parent;

    } else {

        if (subst->parent == node) {
            temp->parent = subst;

        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        ngx_rbt_copy_color(subst, node);

        if (node == *root) {
            *root = subst;

        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) {
            subst->right->parent = subst;
        }
    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    if (red) {//
        return;
    }

    /* a delete fixup *///调整红黑树的四种情况，看注释

    while (temp != *root && ngx_rbt_is_black(temp)) {
        //当前节点为左节点的情况
        if (temp == temp->parent->left) {
            w = temp->parent->right;
            //情况1：当前节点为黑色，兄弟节点为红色
            if (ngx_rbt_is_red(w)) {
                ngx_rbt_black(w);//解决： 兄弟节点修改为黑色
                ngx_rbt_red(temp->parent);//父节点修改红色
                ngx_rbtree_left_rotate(root, sentinel, temp->parent);//左旋父节点
                w = temp->parent->right;
            }
            //情况2：当前节点为黑色，兄弟节点为黑色，兄弟节点的俩子节点也为黑色
            if (ngx_rbt_is_black(w->left) && ngx_rbt_is_black(w->right)) {
                ngx_rbt_red(w);//解决：兄弟节点修改为红色
                temp = temp->parent;//当前节点指向父节点

            } else {//情况3：当前节点为黑色，兄弟节点为黑色，兄弟节点右子节点为黑色，左子节点为红色
                if (ngx_rbt_is_black(w->right)) {
                    ngx_rbt_black(w->left);//解决：左子节点改为黑色
                    ngx_rbt_red(w);//兄弟节点改为红色
                    ngx_rbtree_right_rotate(root, sentinel, w);//右旋兄弟节点
                    w = temp->parent->right;
                }
                //情况4：当前节点为黑色，兄弟节点为黑色，兄弟节点右子节点为红色，左子节点任意色
                ngx_rbt_copy_color(w, temp->parent);//解决：兄弟节点修改为父节点的颜色
                ngx_rbt_black(temp->parent);        //父节点修给为黑色
                ngx_rbt_black(w->right);            //右子节点修改为黑色
                ngx_rbtree_left_rotate(root, sentinel, temp->parent);//左旋父节点
                temp = *root;
            }

        } else {//当前节点为右节点的情况
            w = temp->parent->left;

            if (ngx_rbt_is_red(w)) {
                ngx_rbt_black(w);
                ngx_rbt_red(temp->parent);
                ngx_rbtree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (ngx_rbt_is_black(w->left) && ngx_rbt_is_black(w->right)) {
                ngx_rbt_red(w);
                temp = temp->parent;

            } else {
                if (ngx_rbt_is_black(w->left)) {
                    ngx_rbt_black(w->right);
                    ngx_rbt_red(w);
                    ngx_rbtree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }

                ngx_rbt_copy_color(w, temp->parent);
                ngx_rbt_black(temp->parent);
                ngx_rbt_black(w->left);
                ngx_rbtree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }

    ngx_rbt_black(temp);
}


static ngx_inline void
ngx_rbtree_left_rotate(ngx_rbtree_node_t **root, ngx_rbtree_node_t *sentinel,
    ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {//node为根节点，这个地方容易出错
        *root = temp;   //设置新的根节点

    } else if (node == node->parent->left) {
        node->parent->left = temp;

    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}


static ngx_inline void
ngx_rbtree_right_rotate(ngx_rbtree_node_t **root, ngx_rbtree_node_t *sentinel,
    ngx_rbtree_node_t *node)
{
    ngx_rbtree_node_t  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {//node为根节点，这个地方容易出错
        *root = temp;   //设置新的根节点

    } else if (node == node->parent->right) {//假如是右子节点
        node->parent->right = temp;

    } else {//左子节点
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}
