
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


static void *ngx_palloc_block(ngx_pool_t *pool, size_t size);
static void *ngx_palloc_large(ngx_pool_t *pool, size_t size);


ngx_pool_t *
ngx_create_pool(size_t size, ngx_log_t *log)
{
    ngx_pool_t  *p;

    p = ngx_memalign(NGX_POOL_ALIGNMENT, size, log);//申请的内存按16对齐，内存大小必须是16的整数倍，并且大于等于size
    if (p == NULL) {
        return NULL;
    }

    p->d.last = (u_char *) p + sizeof(ngx_pool_t);//指向可用内存的起始位置，从这个位置开始分配内存给用户使用
    p->d.end = (u_char *) p + size;//指向可用内存的结束位置
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(ngx_pool_t);
    p->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;//4095

    p->current = p; //指向本身内存池
    p->chain = NULL;
    p->large = NULL;
    p->cleanup = NULL;
    p->log = log;

    return p;
}

/*
ngx_destroy_pool函数分为三步：
首先清理ngx_pool_cleanup_t结构的内存，这个结构上挂载了许多清理hander
然后清理大块内存，即ngx_poll_large_t这个结构体中存放的大块内存
最后清理ngx_pool_t的内存，即ngx_create_pool申请的内存
*/
void
ngx_destroy_pool(ngx_pool_t *pool)
{
    ngx_pool_t          *p, *n;
    ngx_pool_large_t    *l;
    ngx_pool_cleanup_t  *c;

	//第一步调用ngx_pool_clean_t上挂载的清理句柄来清理相应的data，也可能是关闭文件，socket连接等等
    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
                           "run cleanup: %p", c);
            c->handler(c->data);
        }
    }

	//第二步释放大块内存，实际上是ngx_pool_large这个函数申请的内存
    for (l = pool->large; l; l = l->next) {

        ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0, "free: %p", l->alloc);

        if (l->alloc) {
            ngx_free(l->alloc);
        }
    }

#if (NGX_DEBUG)

    /*
     * we could allocate the pool->log from this pool
     * so we cannot use this log while free()ing the pool
     */

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
                       "free: %p, unused: %uz", p, p->d.end - p->d.last);

        if (n == NULL) {
            break;
        }
    }

#endif
    //第三步释放ngx_create_pool申请的内存
    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        ngx_free(p);

        if (n == NULL) {
            break;
        }
    }
}

/*
重置内存池操作是把内存池恢复到初始状态，即没有把内存分配给用户的状态，这个操作释放了大块内存，
而把小块内存恢复到原来的状态，就是修改ngx_pool_data_t中的last成员的值，使其指向最初的可分配
给用户的内存位置。
*/

void
ngx_reset_pool(ngx_pool_t *pool)
{
    ngx_pool_t        *p;
    ngx_pool_large_t  *l;

	//释放大块内存
    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            ngx_free(l->alloc);
        }
    }

    pool->large = NULL;

    for (p = pool; p; p = p->d.next) {
        p->d.last = (u_char *) p + sizeof(ngx_pool_t); //修改last指针，使其指向最开始可分配的内存位置
    }
}

//从pool内存池分配以NGX_ALIGNMENT对齐的内存，而ngx_pnalloc分配适合size大小的内存，不考虑内存对齐
void *
ngx_palloc(ngx_pool_t *pool, size_t size)
{
    u_char      *m;
    ngx_pool_t  *p;

	//申请的内存小于max
    if (size <= pool->max) {

        p = pool->current;
        
        //在内存池链表中查找是否有>=size的空闲内存
        do {
			//执行对齐操作，  
            //即以last开始，计算以NGX_ALIGNMENT对齐的偏移位置指针
            m = ngx_align_ptr(p->d.last, NGX_ALIGNMENT);

            if ((size_t) (p->d.end - m) >= size) {//找到满足的内存块
                p->d.last = m + size;

                return m;
            }

            p = p->d.next;

        } while (p);

		//如果在内存池链表中没有找到>=size的内存，就调用ngx_palloc_block函数来申请
        //申请之后把这块内存连接到内存池链表的最后
        return ngx_palloc_block(pool, size);
    }
   
    //申请的内存大于max
    return ngx_palloc_large(pool, size);
}


void *
ngx_pnalloc(ngx_pool_t *pool, size_t size)
{
    u_char      *m;
    ngx_pool_t  *p;

    if (size <= pool->max) {

        p = pool->current;

        do {
            m = p->d.last;

            if ((size_t) (p->d.end - m) >= size) {
                p->d.last = m + size;

                return m;
            }

            p = p->d.next;

        } while (p);

        return ngx_palloc_block(pool, size);
    }

    return ngx_palloc_large(pool, size);
}


static void *
ngx_palloc_block(ngx_pool_t *pool, size_t size)
{
    u_char      *m;
    size_t       psize;
    ngx_pool_t  *p, *new, *current;

    psize = (size_t) (pool->d.end - (u_char *) pool);

   //执行按NGX_POOL_ALIGNMENT对齐方式的内存分配
    m = ngx_memalign(NGX_POOL_ALIGNMENT, psize, pool->log);
    if (m == NULL) {
        return NULL;
    }

    new = (ngx_pool_t *) m;
    //初始化block
    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(ngx_pool_data_t);//m指向可分配内存的起始位置
    m = ngx_align_ptr(m, NGX_ALIGNMENT);//内存对齐
    new->d.last = m + size;//last指向下一次分配的位置，因为m到m+size的内存已分配

    current = pool->current;

    for (p = current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            current = p->d.next;//移动current，失败4次说明前面可用的内存块空间很小，以后直接从current这个位置来分配内存
        }
    }

    p->d.next = new;//将新分配的block连接到内存池

    pool->current = current ? current : new;//如果current为空的情况

    return m;
}


/*
这是一个static的函数，说明外部函数不会随便调用，而是提供给内部分配调用的，  
即nginx在进行内存分配需求时，不会自行去判断是否是大块内存还是小块内存，  
而是交由内存分配函数去判断，对于用户需求来说是完全透明的。
*/
  
static void *  
ngx_palloc_large(ngx_pool_t *pool, size_t size)  
{  
    void              *p;  
    ngx_uint_t         n;  
    ngx_pool_large_t  *large;  
      
    p = ngx_alloc(size, pool->log);    
    if (p == NULL) {  
        return NULL;  
    }  
      
    n = 0;  
      
    //以下几行，将分配的内存链入pool的large链中，  
    //这里指原始pool在之前已经分配过large内存的情况。  
    for (large = pool->large; large; large = large->next) {  
        if (large->alloc == NULL) {  
            large->alloc = p;  
            return p;  
        }  
          
        if (n++ > 3) {  
            break;  
        }  
    }  
      
    //如果该pool之前并未分配large内存，则就没有ngx_pool_large_t来管理大块内存  
    //执行ngx_pool_large_t结构体的分配，用于来管理large内存块。  
    large = ngx_palloc(pool, sizeof(ngx_pool_large_t));  
    if (large == NULL) {  
        ngx_free(p);  
        return NULL;  
    }  
      
    large->alloc = p;  
    large->next = pool->large;  
    pool->large = large;  
      
    return p;  
}  

//ngx_pmemalign将在分配size大小的内存并按alignment对齐，然后挂到large字段下，当做大块内存处理
void *
ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    ngx_pool_large_t  *large;

    p = ngx_memalign(alignment, size, pool->log);
    if (p == NULL) {
        return NULL;
    }

    large = ngx_palloc(pool, sizeof(ngx_pool_large_t));
    if (large == NULL) {
        ngx_free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

//释放大块内存
ngx_int_t
ngx_pfree(ngx_pool_t *pool, void *p)
{
    ngx_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
                           "free: %p", l->alloc);
            ngx_free(l->alloc);
            l->alloc = NULL;

            return NGX_OK;
        }
    }

    return NGX_DECLINED;
}


void *
ngx_pcalloc(ngx_pool_t *pool, size_t size)
{
    void *p;

    p = ngx_palloc(pool, size);
    if (p) {
        ngx_memzero(p, size);//初始化
    }

    return p;
}

//添加cleanup
ngx_pool_cleanup_t *
ngx_pool_cleanup_add(ngx_pool_t *p, size_t size)
{
    ngx_pool_cleanup_t  *c;

    c = ngx_palloc(p, sizeof(ngx_pool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = ngx_palloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, p->log, 0, "add cleanup: %p", c);

    return c;
}

//clean file 
void
ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd)
{
    ngx_pool_cleanup_t       *c;
    ngx_pool_cleanup_file_t  *cf;

    for (c = p->cleanup; c; c = c->next) {
        if (c->handler == ngx_pool_cleanup_file) {

            cf = c->data;

            if (cf->fd == fd) {
                c->handler(cf);
                c->handler = NULL;
                return;
            }
        }
    }
}


void
ngx_pool_cleanup_file(void *data)
{
    ngx_pool_cleanup_file_t  *c = data;

    ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, c->log, 0, "file cleanup: fd:%d",
                   c->fd);

    if (ngx_close_file(c->fd) == NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ALERT, c->log, ngx_errno,
                      ngx_close_file_n " \"%s\" failed", c->name);
    }
}


void
ngx_pool_delete_file(void *data)
{
    ngx_pool_cleanup_file_t  *c = data;

    ngx_err_t  err;

    ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, c->log, 0, "file cleanup: fd:%d %s",
                   c->fd, c->name);

    if (ngx_delete_file(c->name) == NGX_FILE_ERROR) {
        err = ngx_errno;

        if (err != NGX_ENOENT) {
            ngx_log_error(NGX_LOG_CRIT, c->log, err,
                          ngx_delete_file_n " \"%s\" failed", c->name);
        }
    }

    if (ngx_close_file(c->fd) == NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ALERT, c->log, ngx_errno,
                      ngx_close_file_n " \"%s\" failed", c->name);
    }
}


#if 0

static void *
ngx_get_cached_block(size_t size)
{
    void                     *p;
    ngx_cached_block_slot_t  *slot;

    if (ngx_cycle->cache == NULL) {
        return NULL;
    }

    slot = &ngx_cycle->cache[(size + ngx_pagesize - 1) / ngx_pagesize];

    slot->tries++;

    if (slot->number) {
        p = slot->block;
        slot->block = slot->block->next;
        slot->number--;
        return p;
    }

    return NULL;
}

#endif
