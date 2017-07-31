#include <stdio.h>
#include <stdlib.h>
#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_conf_file.h>
#include <ngx_palloc.h>
#include <ngx_rbtree.h>

typedef struct
{
	ngx_rbtree_node_t node;
	ngx_int_t 		  num;
	void             *data; //存储数据的指针
}TestRBTNode;//自定义的数据结构体必须包含ngx_rbtree_node_t

//先序遍历
void travel_rbtree(ngx_rbtree_node_t* root, ngx_rbtree_node_t *sentinel);

int main()
{	
	ngx_rbtree_t      rbtree;
	ngx_rbtree_node_t sentinel;
	
	//初始化rbtree
	ngx_rbtree_init(&rbtree, &sentinel, ngx_rbtree_insert_value);

	TestRBTNode rbtNode[10];
	rbtNode[0].num = 1;
    rbtNode[1].num = 11;
	rbtNode[2].num = 22;
    rbtNode[3].num = 6;
	rbtNode[4].num = 13;
    rbtNode[5].num = 15;
	rbtNode[6].num = 27;
    rbtNode[7].num = 8;
	rbtNode[8].num = 25;
    rbtNode[9].num = 17;
	//注意下面的插入代码
	ngx_int_t i;
	for (i = 0; i < 10; ++i)
	{
		rbtNode[i].node.key = rbtNode[i].num;
		ngx_rbtree_insert(&rbtree, &rbtNode[i].node);//插入节点
	}
    
    travel_rbtree(rbtree.root, rbtree.sentinel);

	ngx_uint_t         lookupkey = 13;
	ngx_rbtree_node_t *tempNode  = rbtree.root;
	TestRBTNode        *lookupNode;
	//查找节点的代码
	while (tempNode != &sentinel)
	{
		if (tempNode->key != lookupkey)
		{
			tempNode = (lookupkey < tempNode->key) ? tempNode->left : tempNode->right;
			continue;
		}
		lookupNode = (TestRBTNode*)tempNode;//可以强制转化为TestRBTNode来访问自定义结构体里面的数据
		break;
	}
	
	if (tempNode != &sentinel)
	{
		printf("find the key: %d\n", tempNode->key);
	}
	
	ngx_rbtree_delete(&rbtree, &lookupNode->node);

	
	return 0;
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
