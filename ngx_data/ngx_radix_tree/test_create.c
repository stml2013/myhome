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
#include <ngx_array.h>
#include <ngx_radix_tree.h>

////////////////////////////////////////////////////////////////////////////////////
//不加下面这两个定义编译会出错
volatile ngx_cycle_t  *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
            const char *fmt, ...)
{
}
////////////////////////////////////////////////////////////////////////////////////


int main()
{
	/*创建内存池对象*/
	ngx_pool_t* pool = ngx_create_pool(1024, NULL);
	if (pool == NULL)
	{
		printf("create pool failed!\n");
		exit(1);
	}
	
    //printf("xxxxx\n");
	////////////////////////////////////////////////////////////////////////////////
	//一定要初始化ngx_pagesize这个全局变量，
	//不初始化，会出现段错误（核心已转储），这也是全局变量的潜在危害：
	//你不知道你的程序中是否用到这个全局变量，如果用到，这个全局变量初始化了没有
	//在一些大的程序中，你根本无法快速知道这些，所以应尽量避免使用全局变量
    ngx_pagesize = getpagesize();
	printf("pagesize = %d\n", ngx_pagesize);
	////////////////////////////////////////////////////////////////////////////////

    /*创建基数树，prealloc=0时，只创建结构体ngx_radix_tree_t,没有创建任何基数树节点*/
    ngx_radix_tree_t* tree = (ngx_radix_tree_t*)ngx_radix_tree_create(pool, 0);
	//printf("xxxxxY\n");
	if (tree == NULL)
	{
		printf("crate radix tree failed!\n");
		exit(1);
	}

	ngx_array_t* array = ngx_array_create(pool, 10, sizeof(int));

	/*插入data*/
	ngx_uint_t deep = 5;//树的最大深度为4
	ngx_uint_t mask = 0;
	ngx_uint_t inc  = 0x80000000;
	ngx_uint_t key  = 0;
	ngx_uint_t cunt = 0;//data数组的索引



	return 0;
}




