/*
author: smtl
date: 2014-09-25-11:29
*/

#include <stdio.h>
#include <stdlib.h>
#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_conf_file.h>
#include <nginx.h>
#include <ngx_string.h>
#include <ngx_palloc.h>
#include <ngx_array.h>

////////////////////////////////////////////////////////////////////////////////////
//不加下面这两个定义编译会出错
volatile ngx_cycle_t  *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
            const char *fmt, ...)
{
}
////////////////////////////////////////////////////////////////////////////////////

int main()//too many migical number
{
	ngx_pool_t* pool = ngx_create_pool(2048, NULL);
	if (pool == NULL)
	{
        printf("create pool failed\n");
        exit(1);
	}

	ngx_array_t* array = ngx_array_create(pool, 20, sizeof(ngx_int_t));
	if (array == NULL)
	{
        printf("array alloc failed\n");
        exit(1);
	}

    ngx_int_t i;
    ngx_int_t* elem;
    for (i=0; i<20; ++i)
    {
        elem = ngx_array_push(array);//添加元素
        *elem = i;
    }

    elem = (int*)array->elts;
    for (i=0; i<20; ++i)
    {
        printf("array[%d] = %d \n", i, elem[i]);
    }

    printf("添加10个元素：\n");
    ngx_int_t n = 10;
    elem = ngx_array_push_n(array, n);
    for (i=0; i<n; ++i)
    {
        elem[i] = 20 + i;
    }

    elem = (int*)array->elts;
    for (i=20; i<20+n; ++i)
    {
        printf("array[%d] = %d \n", i, elem[i]);
    }

    ngx_array_destroy(array);

    printf("ngx_array_int:\n");

    array = ngx_pcalloc(pool, sizeof(ngx_array_t));
    //注意这个函数和ngx_array_create的区别，array不能为空
    ngx_array_init(array, pool, 20, sizeof(ngx_int_t));

    for (i=0; i<20; ++i)
    {
        elem = ngx_array_push(array);//添加元素
        *elem = rand()%1000;
    }

    elem = array->elts;
    for (i=0; i<20; ++i)
    {
        printf("array[%d] = %d \n", i, elem[i]);
    }

    ngx_destroy_pool(pool);

    return 0;
}
