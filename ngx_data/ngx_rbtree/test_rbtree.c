#include <stdio.h>
#include <stdlib.h>
#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_conf_file.h>
#include <ngx_string.h>
#include <ngx_palloc.h>
#include <ngx_rbtree.h>

#define ngx_rbtee_data(p, type, member) (type*)((u_char*)p - offset(type, member))

//节点数据结构
typedef struct
{
    ngx_rbtree_node_t  node;//方便类型转化
    ngx_str_t          str;
}ngx_string_node_t;

//自定义插入函数
void ngx_string_rbtree_insert_value(ngx_rbtree_node_t *temp, ngx_rbtree_node_t *node,
    ngx_rbtree_node_t *sentinel);

//先序遍历红黑树
void trave_rbtree(ngx_rbtree_node_t *root, ngx_rbtree_node_t *sentinel);

int main()
{
    ngx_rbtree_t      tree;
    ngx_rbtree_node_t sentinel;

    ngx_rbtree_init(&tree, &sentinel, ngx_string_rbtree_insert_value);

    ngx_string_node_t strNode[10];

    ngx_str_set(&strNode[0].str, "abc0");
    strNode[0].node.key = 1;

    ngx_str_set(&strNode[1].str, "abc1");
    strNode[1].node.key = 6;

    ngx_str_set(&strNode[2].str, "abc2");
    strNode[2].node.key = 8;

	ngx_str_set(&strNode[3].str, "abc35");
    strNode[3].node.key = 11;

	ngx_str_set(&strNode[4].str, "abd4");
    strNode[4].node.key = 8;

	ngx_str_set(&strNode[5].str, "abc5");
    strNode[5].node.key = 1;

    ngx_str_set(&strNode[6].str, "abc11");
    strNode[6].node.key = 11;

	ngx_str_set(&strNode[7].str, "a6");
    strNode[7].node.key = 1;

	ngx_str_set(&strNode[8].str, "a8");
    strNode[8].node.key = 6;

    ngx_str_set(&strNode[9].str, "abc0");
    strNode[9].node.key = 6;

	ngx_int_t i;
	for (i = 0; i < 10; ++i)
	{
		ngx_rbtree_insert(&tree, &strNode[i].node);
	}

    travel_rbtree(tree.root, tree.sentinel);

    return 0;
}

void
ngx_string_rbtree_insert_value(ngx_rbtree_node_t *temp, ngx_rbtree_node_t *node,
    ngx_rbtree_node_t *sentinel)
{
    ngx_rbtree_node_t  **p;
    ngx_string_node_t      *strNodeX;
    ngx_string_node_t      *strNodeY;

    for ( ;; )
    {
        if (node->key != temp->key)
        {
             p = (node->key < temp->key) ? &temp->left : &temp->right;
        }
        else
        {
            strNodeX = (ngx_string_node_t*)node;//类型转化，因为node的地址和ngx_string_node_t的地址相同
            //strNodeY = (ngx_string_node_t*)temp;
            //或者这样用
			strNodeY = ngx_rbtree_data(temp, ngx_string_node_t, node);

            if (strNodeX->str.len != strNodeY->str.len)
            {
                p = (strNodeX->str.len < strNodeY->str.len) ? &temp->left : &temp->right;
            }
            else
            {
                p = (ngx_memcmp(strNodeX->str.data, strNodeY->str.data, strNodeX->str.len) < 0) ? &temp->left : &temp->right;
            }
        }

        if (*p == sentinel)
        {
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
travel_rbtree(ngx_rbtree_node_t* root, ngx_rbtree_node_t *sentinel)
{
	if (root->left != sentinel) 
	{
		travel_rbtree(root->left, sentinel);
	}

	ngx_string_node_t* strNode = (ngx_string_node_t*)root;
	printf("key = %d , str=%s\n", root->key, strNode->str.data);

	if (root->right != sentinel)
	{
		travel_rbtree(root->right, sentinel);
	}
}



