
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


static void *ngx_radix_alloc(ngx_radix_tree_t *tree);

/*
基数树有点类似字典树，树的最大深度为32层，因为key值是由二进制树，且插入的时候
从左到右检测每一位，如果为1向右孩子，为0向左孩子。
*/

/*
创建基数树
*/
ngx_radix_tree_t *
ngx_radix_tree_create(ngx_pool_t *pool, ngx_int_t preallocate)
{
    uint32_t           key, mask, inc;
    ngx_radix_tree_t  *tree;

    tree = ngx_palloc(pool, sizeof(ngx_radix_tree_t));//申请ngx_raidx_tree_t,这个tree是返回的指针
    if (tree == NULL) {
        return NULL;
    }
    //初始化ngx_radix_tree_t本身
    tree->pool = pool;
    tree->free = NULL;
    tree->start = NULL;
    tree->size = 0;

    tree->root = ngx_radix_alloc(tree);//申请一个基数节点
    if (tree->root == NULL) {
        return NULL;
    }
	//初始化root节点
    tree->root->right = NULL;
    tree->root->left = NULL;
    tree->root->parent = NULL;
    tree->root->value = NGX_RADIX_NO_VALUE;

	/*prealloc=0时，只创建结构体ngx_radix_tree_t,没有创建任何基数树节点*/
    if (preallocate == 0) {
        return tree;
    }

    /*
     * Preallocation of first nodes : 0, 1, 00, 01, 10, 11, 000, 001, etc.
     * increases TLB hits even if for first lookup iterations.
     * On 32-bit platforms the 7 preallocated bits takes continuous 4K,
     * 8 - 8K, 9 - 16K, etc.  On 64-bit platforms the 6 preallocated bits
     * takes continuous 4K, 7 - 8K, 8 - 16K, etc.  There is no sense to
     * to preallocate more than one page, because further preallocation
     * distributes the only bit per page.  Instead, a random insertion
     * may distribute several bits per page.
     *
     * Thus, by default we preallocate maximum
     *     6 bits on amd64 (64-bit platform and 4K pages)
     *     7 bits on i386 (32-bit platform and 4K pages)
     *     7 bits on sparc64 in 64-bit mode (8K pages)
     *     8 bits on sparc64 in 32-bit mode (8K pages)
     */

	/*prealloc=-1时,根据下面的情况创建基数树节点*/
    if (preallocate == -1) {
        switch (ngx_pagesize / sizeof(ngx_radix_tree_t)) {

        /* amd64 */
        case 128:
            preallocate = 6;
            break;

        /* i386, sparc64 */
        case 256:
            preallocate = 7;
            break;

        /* sparc64 in 32-bit mode */
        default:
            preallocate = 8;
        }
    }

    mask = 0;
    inc = 0x80000000;
	//加入preallocate=7,最终建的基数树的节点总个数为2^(preallocate+1)-1，每一层个数为2^(7-preallocate)
    //循环如下：
	//preallocate  =      7         6        5         4         3         2        1
	//mask(最左8位)=      10000000  11000000 11100000  11110000  11111000  11111100 11111110
	//inc          =      10000000  01000000 00100000  00010000  00001000  00000100 00000010
	//增加节点个数  =      2         4        8         16        32        64       128
    while (preallocate--) {

        key = 0;
        mask >>= 1;
        mask |= 0x80000000;

        do {//根据inc的值添加节点
            if (ngx_radix32tree_insert(tree, key, mask, NGX_RADIX_NO_VALUE)
                != NGX_OK)
            {
                return NULL;
            }

            key += inc;//当preallocate=0时，是最后一层，构建的节点个数为2^preallocate

        } while (key);

        inc >>= 1;
    }

    return tree;
}


ngx_int_t
ngx_radix32tree_insert(ngx_radix_tree_t *tree, uint32_t key, uint32_t mask,
    uintptr_t value)
{
    uint32_t           bit;
    ngx_radix_node_t  *node, *next;

    bit = 0x80000000;

    node = tree->root;
    next = tree->root;

    while (bit & mask) {
        if (key & bit) {//等于1向右查找
            next = node->right;

        } else {//等于0向左查找
            next = node->left;
        }

        if (next == NULL) {
            break;
        }

        bit >>= 1;
        node = next;
    }

    if (next) {//如果next不为空
        if (node->value != NGX_RADIX_NO_VALUE) {//如果数据不为空
            return NGX_BUSY;//返回NGX_BUSY
        }

        node->value = value;//直接赋值
        return NGX_OK;
    }

	//如果next为中间节点，且为空，继续查找且申请路径上为空的节点
	//比如找key=1000111，在找到10001时next为空，那要就要申请三个节点分别存10001,100011,1000111,
	//1000111最后一个节点为key要插入的节点
    while (bit & mask) {//没有到达最深层，继续向下申请节点
        next = ngx_radix_alloc(tree);//申请一个节点
        if (next == NULL) {
            return NGX_ERROR;
        }

		//初始化节点
        next->right = NULL;
        next->left = NULL;
        next->parent = node;
        next->value = NGX_RADIX_NO_VALUE;

        if (key & bit) {
            node->right = next;

        } else {
            node->left = next;
        }

        bit >>= 1;
        node = next;
    }

    node->value = value;//指向数据区

    return NGX_OK;
}


ngx_int_t
ngx_radix32tree_delete(ngx_radix_tree_t *tree, uint32_t key, uint32_t mask)
{
    uint32_t           bit;
    ngx_radix_node_t  *node;

    bit = 0x80000000;
    node = tree->root;
    //根据key和掩码查找
    while (node && (bit & mask)) {
        if (key & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit >>= 1;
    }

    if (node == NULL) {//没有找到
        return NGX_ERROR;
    }

	//node不为叶节点直接把value置为空
    if (node->right || node->left) {
        if (node->value != NGX_RADIX_NO_VALUE) {//value不为空
            node->value = NGX_RADIX_NO_VALUE;//置空value
            return NGX_OK;
        }

        return NGX_ERROR;//value为空，返回error
    }

	//node为叶子节点，直接放到free区域
    for ( ;; ) {//删除叶子节点
        if (node->parent->right == node) {
            node->parent->right = NULL;//

        } else {
            node->parent->left = NULL;
        }

		//把node链入free链表
        node->right = tree->free;//放到free区域
        tree->free = node;//free指向node
        //假如删除node以后，父节点是叶子节点，就继续删除父节点，
		//一直到node不是叶子节点
        node = node->parent;

        if (node->right || node->left) {//node不为叶子节点
            break;
        }

        if (node->value != NGX_RADIX_NO_VALUE) {//node的value不为空
            break;
        }

        if (node->parent == NULL) {//node的parent为空
            break;
        }
    }

    return NGX_OK;
}

/*
根据key值查找数据,很简单
*/
uintptr_t
ngx_radix32tree_find(ngx_radix_tree_t *tree, uint32_t key)
{
    uint32_t           bit;
    uintptr_t          value;
    ngx_radix_node_t  *node;

    bit = 0x80000000;
    value = NGX_RADIX_NO_VALUE;
    node = tree->root;

    while (node) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            value = node->value;
        }

        if (key & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit >>= 1;//往下层查找
    }

    return value;
}

/*
申请一个基数树节点，首先在free中查找，free中没有空闲节点再申请空间
*/
static void *
ngx_radix_alloc(ngx_radix_tree_t *tree)
{
    char  *p;

    if (tree->free) {//如果free中有可利用的空间节点
        p = (char *) tree->free;//指向第一个可利用的空间节点
        tree->free = tree->free->right;//修改free
        return p;
    }

    if (tree->size < sizeof(ngx_radix_node_t)) {//如果空闲内存大小不够分配一个节点就申请一页大小的内存
        tree->start = ngx_pmemalign(tree->pool, ngx_pagesize, ngx_pagesize);
        if (tree->start == NULL) {
            return NULL;
        }

        tree->size = ngx_pagesize;//修改空闲内存大小
    }

    //分配一个节点的空间
    p = tree->start;
    tree->start += sizeof(ngx_radix_node_t);
    tree->size -= sizeof(ngx_radix_node_t);

    return p;
}

/*
应该看看nginx的geo模块
*/
