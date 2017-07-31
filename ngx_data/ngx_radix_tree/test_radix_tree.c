/********************************************************
author: smtl
date: 
*********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_conf_file.h>
#include <ngx_palloc.h>
#include <ngx_radix_tree.h>

////////////////////////////////////////////////////////////////////////////////////
//不加下面这两个定义编译会出错
volatile ngx_cycle_t  *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
            const char *fmt, ...)
{
}
////////////////////////////////////////////////////////////////////////////////////

//先序遍历radix_tree
void travel_radix_tree(ngx_radix_node_t *root);

int main()
{
	/*基数树节点的数据集:ngx_int_t类型，只是测试，实际上可以为任何类型*/
	ngx_int_t data[64];
	ngx_int_t i = 0;
	for (i = 0; i < 64; ++i)
	{
		data[i] = rand()%10000;
	}

	/*创建内存池对象*/
	ngx_pool_t* pool = ngx_create_pool(1024, NULL);
	if (pool == NULL)
	{
		printf("create pool failed!\n");
		exit(1);
	}
	
    //printf("xxxxx\n");
	////////////////////////////////////////////////////////////////////////////////
	//一定要初始化ngx_pagesize这个全局变量，调试了一个晚上
	//不初始化，会出现段错误（核心已转储），这也是全局变量的潜在危害：
	//你不知道你的程序中是否用到这个全局变量，如果用到，这个全局变量初始化了没有
	//在一些大的程序中，你根本无法快速知道这些，所以应尽量避免使用全局变量
    ngx_pagesize = getpagesize();
	printf("pagesize = %d\n", ngx_pagesize);
	////////////////////////////////////////////////////////////////////////////////

    /*创建基数树，prealloc=0时，只创建结构体ngx_radix_tree_t,没有创建任何基数树节点*/
	ngx_radix_tree_t *tree = ngx_radix_tree_create(pool, -1);
	//printf("xxxxxY\n");
	if (tree == NULL)
	{
		printf("crate radix tree failed!\n");
		exit(1);
	}
	
	
	/*插入data*/
	ngx_uint_t deep = 5;//树的最大深度为4
	ngx_uint_t mask = 0;
	ngx_uint_t inc  = 0x80000000;
	ngx_uint_t key  = 0;
	ngx_uint_t cunt = 0;//data数组的索引

	while (deep--)
	{
		key    = 0;
		mask >>= 1;
		mask  |= 0x80000000;
		do 
		{
			if (NGX_OK != ngx_radix32tree_insert(tree, key, mask, &data[cunt]))
			{
				printf("insert error\n");
				exit(1);
			}

			key += inc;

			++cunt;
			if (cunt > 63)
			{
				cunt = 63;
			}
		}while(key);

		inc >>= 1;
	}

	/*先序打印数据*/
	travel_radix_tree(tree->root);
	printf("\n");

	/*查找测试*/
	ngx_uint_t tkey  = 0x58000000;
	ngx_int_t* value = ngx_radix32tree_find(tree, 0x58000000);
	if (value != NGX_RADIX_NO_VALUE)
	{
		printf("find the value: %d with the key = %x\n", *value, tkey);
	}
	else
	{
		printf("not find the the value with the key = %x\n", tkey);
	}
    
	/*删除测试*/
	if (NGX_OK == ngx_radix32tree_delete(tree, tkey, mask))
	{
		printf("delete the the value with the key = %x is succeed\n", tkey);
	}
	else
	{
		printf("delete the the value with the key = %x is failed\n", tkey);
	}

    value = ngx_radix32tree_find(tree, 0x58000000);
	if (value != NGX_RADIX_NO_VALUE)
	{
		printf("find the value: %d with the key = %x\n", *value, tkey);
	}
	else
	{
		printf("not find the the value with the key = %x\n", tkey);
	}

	return 0;
}

void travel_radix_tree(ngx_radix_node_t *root)
{
	if (root->left != NULL)
	{
		travel_radix_tree(root->left);
	}
	
	if (root->value != NGX_RADIX_NO_VALUE)
	{
		ngx_int_t* value = root->value;
		printf("%d\n", *value);
	}

	if (root->right != NULL)
	{
		travel_radix_tree(root->right);
	}
}



