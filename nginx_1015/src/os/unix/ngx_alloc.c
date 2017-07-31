
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


ngx_uint_t  ngx_pagesize;
ngx_uint_t  ngx_pagesize_shift;
ngx_uint_t  ngx_cacheline_size;


void *
ngx_alloc(size_t size, ngx_log_t *log)
{
    void  *p;

    p = malloc(size);//默认返回的是8的倍数
    if (p == NULL) {
        ngx_log_error(NGX_LOG_EMERG, log, ngx_errno,
                      "malloc(%uz) failed", size);
    }

    ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, log, 0, "malloc: %p:%uz", p, size);

    return p;
}


void *
ngx_calloc(size_t size, ngx_log_t *log)
{
    void  *p;

    p = ngx_alloc(size, log);

    if (p) {
        ngx_memzero(p, size);//对申请的内存进行初始化
    }

    return p;
}


#if (NGX_HAVE_POSIX_MEMALIGN)

void *
ngx_memalign(size_t alignment, size_t size, ngx_log_t *log)
{
    void  *p;
    int    err;

    err = posix_memalign(&p, alignment, size);

    if (err) {
        ngx_log_error(NGX_LOG_EMERG, log, err,
                      "posix_memalign(%uz, %uz) failed", alignment, size);
        p = NULL;
    }

    ngx_log_debug3(NGX_LOG_DEBUG_ALLOC, log, 0,
                   "posix_memalign: %p:%uz @%uz", p, size, alignment);

    return p;
}

#elif (NGX_HAVE_MEMALIGN)

void *
ngx_memalign(size_t alignment, size_t size, ngx_log_t *log)
{
    void  *p;

    p = memalign(alignment, size);
    if (p == NULL) {
        ngx_log_error(NGX_LOG_EMERG, log, ngx_errno,
                      "memalign(%uz, %uz) failed", alignment, size);
    }

    ngx_log_debug3(NGX_LOG_DEBUG_ALLOC, log, 0,
                   "memalign: %p:%uz @%uz", p, size, alignment);

    return p;
}

/*

在GNU系统中，malloc或realloc返回的内存块地址都是8的倍数（如果是64位系统，则为16的倍数）。如果你需要更大的粒度，请使用memalign或valloc。这些函数在头文件“stdlib.h”中声明。

在GNU库中，可以使用函数free释放memalign和valloc返回的内存块。但无法在BSD系统中使用，而且BSD系统中并未提供释放这样的内存块的途径。

函数：void * memalign (size_t boundary, size_t size)
函数memalign将分配一个由size指定大小，地址是boundary的倍数的内存块。参数boundary必须是2的幂！函数memalign可以分配较大的内存块，并且可以为返回的地址指定粒度。

函数：void * valloc (size_t size)
使用函数valloc与使用函数memalign类似，函数valloc的内部实现里，使用页的大小作为对齐长度，使用memalign来分配内存。它的实现如下所示：
void *
valloc (size_t size)
{
return memalign (getpagesize (), size);
} 

*/

#endif
