#include <stdio.h>
#include <stdlib.h>
#include "ngx_config.h"
#include "ngx_conf_file.h"
#include "ngx_core.h"
#include "nginx.h"
#include "ngx_palloc.h"
#include "ngx_string.h"


/*
typedef struct {
    size_t   len;
    u_char  *data;
}ngx_str_t;

/********************************************************/
/*/只能用于赋值时初始化的操作,
#define ngx_string(str) { sizeof(str) -1, (u_char*)str }
#define ngx_null_string { 0, NULL }
//下面的情况用这连个操作
ngx_str_t tmp = ngx_string("test"");
ngx_str_t thi = ngx_null_string;
//下面这样用会出错
ngx_str_t err, wrong;
err = ngx_string("error");
wrong = ngx_null_string;
这中情况要用到下面的两种函数
#define ngx_str_set(str, text) (str)->len = sizeof(text) - 1, (str)->data = (u_char*)text
#define ngx_str_null(str) (str)->len = 0, (str)->data = NULL;

size_t ngx_strlen(u_char *str);
放回字符串的长度

void ngx_strlow(u_char *dst, uchar* src, size_t n);
将src的前n个字符转换成小写存放在dst字符串当中，调用者需要保证dst指向的空间大于等于n，
且指向的空间必须可写。操作不会对原字符串产生变动。如要更改原字符串，可以：
ngx_strlow(str->data, str->data, str->len);

ngx_strncmp(s1, s2, n);
区分大小写的字符串比较，只比较前n个字符。

ngx_strcmp(s1, s2)
区分大小写的不带长度的字符串比较

ngx_strcasecmp(u_char *s1, u_char* s2);
不区分大小写的不带长度的字符串比较。

ngx_int_t ngx_strncasecmp(u_char *s1, u_char *s2, size_t n);
不区分大小写的带长度的字符串比较，只比较前n个字符。

u_char * ngx_cdecl ngx_sprintf(u_char *buf, const char *fmt, ...);
u_char * ngx_cdecl ngx_snprintf(u_char *buf, size_t max, const char *fmt, ...);
u_char * ngx_cdecl ngx_slprintf(u_char *buf, u_char *last, const char *fmt, ...);
上面这三个函数用于字符串格式化，ngx_snprintf的第二个参数max指明buf的空间大小，ngx_slprintf
则通过last来指明buf空间的大小。推荐使用第二个或第三个函数来格式化字符串，ngx_sprintf函数
还是比较危险的，容易产生缓冲区溢出漏洞。

对于格式化ngx_str_t结构，器对应的的转义符是必须是%V，传给函数的一定要是指针类型：
ngx_str_t str = ngx_string("hello world");
char buf[1024];
ngx_snprintf(buf, 1024, "%V", &str);
/********************************************************/
volatile ngx_cycle_t  *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
            const char *fmt, ...)
{
}

int main()
{
    ngx_pool_t* pool = NULL;
    pool = ngx_create_pool(1024, NULL);
    if (pool == NULL)
    {
       printf("creating pool is failed\n");
       return 1;
    }

    //这两种用法是有区别的
    ngx_str_t str;
    ngx_str_set(&str, "");
    if (str.data)
    {
       printf("str.len = %d str = %s\n", str.len, str.data);
       //特别是用ngx_conf_set_str_slot函数时，用ngx_str_set这样初始化一个ngx_str_t会出错，返回“is duplicate”
       //呵呵，当初在这个地方调试了一整天
       //但是用ngx_str_null就不会有这样的错误
    }
   
    ngx_str_t type;
    ngx_str_null(&type);
    if (type.data)
    { 
       printf("str.len = %d str = %s\n", type.len, type.data); 
    }

    //ngx_str_t estr;
    //u_char* ps = "hello world";
    //ngx_str_set(estr, ps); //错误,ngx_str_set用的是sizeof(text);
    //ngx_str_t wrg = ngx_string(ps); //错误ngx_string内部也用的是sizeof,因此这两个函数的参数必须是常量字符串

    //再来看一个经典的错误
    /*
    ngx_str_t set_null_error;
    int cond = 1;
    if (cond) ngx_str_set(&set_null_error, "ture");
    else      ngx_str_null(&set_null_error);
    //由于ngx_str_set与ngx_str_null实际上是两行语句，故在if/for/while等语句中单独使用需要用花括号括起来
    */

    ngx_destroy_pool(pool);

    return 0;
}
