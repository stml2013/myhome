/*
author: smtl
date: 2014-09-24-23:17
http://blog.csdn.net/xiaoliangsky/article/details/39523875
*/

#include <stdio.h>
#include <stdlib.h>
#include "ngx_config.h"
#include "ngx_conf_file.h"
#include "nginx.h"
#include "ngx_core.h"
#include "ngx_string.h"
#include "ngx_palloc.h"

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
	ngx_pool_t* pool = ngx_create_pool(1024, NULL);
	if (pool == NULL)
	{
		printf("create pool failed!\n");	
	}
	
	ngx_int_t* array = ngx_palloc(pool, 128*sizeof(ngx_int_t));
	if (array == NULL)
	{
		printf("array alloc is failed!\n");
		exit(1);
	}

	ngx_int_t i;
	for (i=10; i<20; ++i)
	{
		array[i] = rand()%10000;
		printf("i = %d\n", array[i]);
	}

	ngx_str_t* str = ngx_palloc(pool, sizeof(ngx_str_t));
	if (str == NULL)
	{
		printf("str alloc is failed!\n");
		exit(1);
	}

	str->data = ngx_pcalloc(pool, 26*sizeof(char));
    if (str->data == NULL)
	{
		printf("data alloc is failed!\n");
		exit(1);
	}

	str->len = 10;
	for (i=97; i<123; ++i)
	{
		str->data[i-97] = (char)i;
	}

	printf("%s\n", str->data);

	ngx_destroy_pool(pool);

	return 0;
}


