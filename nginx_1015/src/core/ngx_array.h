
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_ARRAY_H_INCLUDED_
#define _NGX_ARRAY_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


struct ngx_array_s {
    void        *elts;//数组的首地址
    ngx_uint_t   nelts;//数组中已经使用的元素个数
    size_t       size; //每个元素占用的内存大小
    ngx_uint_t   nalloc;//当前数组中能够容纳元素的个数
    ngx_pool_t  *pool;  //内存池对象
};

//创建一个动态数组，数组的大小为n，每个元素的大小为size
ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);

//销毁已分配的动态数组元素空间和动态数组对象
void ngx_array_destroy(ngx_array_t *a);

//向数组中添加一个元素，返回这个新元素的地址，如果数组空间已经用完，数组会自动扩充空间
void *ngx_array_push(ngx_array_t *a);

//向数组中添加n个元素，返回这n个元素中第一个元素的地址
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);

//和create函数的功能差不多，只不过这个array不能为空，返回值为是否初始化成功
static ngx_inline ngx_int_t
ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /*
     * set "array->nelts" before "array->elts", otherwise MSVC thinks
     * that "array->nelts" may be used without having been initialized
     */
    //初始化array，array不能为空
    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;

    array->elts = ngx_palloc(pool, n * size);//申请内存空间
    if (array->elts == NULL) {
        return NGX_ERROR;
    }

    return NGX_OK;
}


#endif /* _NGX_ARRAY_H_INCLUDED_ */
