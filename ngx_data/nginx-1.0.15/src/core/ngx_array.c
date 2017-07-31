
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

/*
返回一个ngx_array_t的指针，ngx_array_create最好和ngx_array_destroy成对使用就像使用new和delete。
分为两个步骤：
第一步：申请数组本身的内存
第二步：申请数组存储元素的内存
*/
ngx_array_t *
ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size)
{
    ngx_array_t *a;

	//1：创建ngx_array_t指针，这个array的内存也是在p上申请的
    a = ngx_palloc(p, sizeof(ngx_array_t));
    if (a == NULL) {
        return NULL;
    }

	//2：申请数组存储元素的内存
    a->elts = ngx_palloc(p, n * size);
    if (a->elts == NULL) {
        return NULL;
    }

    //初始化成员
    a->nelts = 0;
    a->size = size;
    a->nalloc = n;
    a->pool = p;

    return a;//返回数组指针
}

/*
会销毁分配给数组的空间，并且销毁数组本身
注意使用这个函数时，a数组必须是由ngx_create_array创建的
a的内存和a的成员elts的内存都是在同一个pool上申请的。
*/
void
ngx_array_destroy(ngx_array_t *a)
{
    ngx_pool_t  *p;

    p = a->pool;

   //1：销毁数组存储元素的内存，即数据区的内存
    if ((u_char *) a->elts + a->size * a->nalloc == p->d.last) {
        p->d.last -= a->size * a->nalloc;
    }

	//2：销毁数组本身的内存，即结构体array本身的内存
	//a = ngx_palloc(p, sizeof(ngx_array_t));这句代码申请的内存
    if ((u_char *) a + sizeof(ngx_array_t) == p->d.last) {
        p->d.last = (u_char *) a;
    }
}

/*
向数组中新增加一个元素，如果数组已满，数组会自动扩充内存，返回新增加元素的地址
*/
void *
ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
    size_t       size;
    ngx_pool_t  *p;

    if (a->nelts == a->nalloc) {//数组已满
        size = a->size * a->nalloc;
        p = a->pool;

        if ((u_char *) a->elts + size == p->d.last
            && p->d.last + a->size <= p->d.end)//如果p的剩余空间>=一个数组元素的空间，就分配一个空间给数组
        {
            p->d.last += a->size;//调整pool的last，即修改下一次可分配空间的其实地址
            a->nalloc++;

        } else {
            new = ngx_palloc(p, 2 * size);//申请新的空间，大小是原来的2倍，假如pool的内存不足够分配一个新的数组元素
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, size);//把原有的元素拷贝到新分配的内存区
            a->elts = new;//修改数组数据区的地址，使其指向新分配的内存区
            a->nalloc *= 2;//修改数组可容纳的元素个数，是原来容纳元素的2倍
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;//新增加元素的地址
    a->nelts++;//数组中元素的个数加1

    return elt;//返回新增加元素的地址
}

/*向数组中添加n个元素，如果数组空间不够容纳这n个元素，就自动扩充内存空间，返回这n个元素中第一个元素的地址*/
void *
ngx_array_push_n(ngx_array_t *a, ngx_uint_t n)
{
    void        *elt, *new;
    size_t       size;
    ngx_uint_t   nalloc;
    ngx_pool_t  *p;

    size = n * a->size;

    if (a->nelts + n > a->nalloc) {//数组已满

        p = a->pool;

        if ((u_char *) a->elts + a->size * a->nalloc == p->d.last
            && p->d.last + size <= p->d.end)//如果pool剩余的内存能够容纳这n个元素，就不用重新分配内存
        {
            p->d.last += size;//修改last使其指向可分配内存的起始地址
            a->nalloc += n;//数组容纳元素个数+n

        } else {//如果pool剩余的内存不能够容纳这n个元素，就重新分配内存

            nalloc = 2 * ((n >= a->nalloc) ? n : a->nalloc);//申请2倍的内存

            new = ngx_palloc(p, nalloc * a->size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, a->nelts * a->size);//把原有的元素拷贝到新申请的内存中
            a->elts = new;//修改数组元素区的地址
            a->nalloc = nalloc;//修改数组能够容纳的元素个数
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;//新增元素的首地址
    a->nelts += n;//已存储元素个数+n

    return elt;
}
