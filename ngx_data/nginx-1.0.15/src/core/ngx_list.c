
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

/*
创建链表
*/
ngx_list_t *
ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    ngx_list_t  *list;

    list = ngx_palloc(pool, sizeof(ngx_list_t));//申请表头
    if (list == NULL) {
        return NULL;
    }

    list->part.elts = ngx_palloc(pool, n * size);//申请数据区
    if (list->part.elts == NULL) {
        return NULL;
    }

	//初始化链表
    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return list;
}


void *
ngx_list_push(ngx_list_t *l)
{
    void             *elt;
    ngx_list_part_t  *last;

    last = l->last;

    if (last->nelts == l->nalloc) {//链表已满

        last = ngx_palloc(l->pool, sizeof(ngx_list_part_t));//分配结点
        if (last == NULL) {
            return NULL;
        }

        last->elts = ngx_palloc(l->pool, l->nalloc * l->size);//分配数据区
        if (last->elts == NULL) {
            return NULL;
        }

        last->nelts = 0;
        last->next = NULL;

        l->last->next = last;//将结点插入链表
        l->last = last;
    }

    elt = (char *) last->elts + l->size * last->nelts;//新添加元素的地址
    last->nelts++;//已存储元素的个数加1

    return elt;//返回新添加元素的地址
}
