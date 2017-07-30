#include <ngx_config.h>
#include <ngx_http.h>
#include <ngx_core.h>

static char* ngx_http_hello(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_hello_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_hello_commands[] = {
    {
       ngx_string("test_hello"),
       NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
       ngx_http_hello,
       NGX_HTTP_LOC_CONF_OFFSET,
       0,
       NULL
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_hello_module_ctx = {
    NULL,
    NULL,

    NULL,
    NULL,
    
    NULL,
    NULL,

    NULL,
    NULL,
};

/* static 加上static有错误 */
ngx_module_t ngx_http_hello_module = {
    NGX_MODULE_V1,
    &ngx_http_hello_module_ctx,
    ngx_http_hello_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

static char* ngx_http_hello(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_hello_handler;
    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_hello_handler(ngx_http_request_t *r)
{
    if (!(r->method & (NGX_HTTP_HEAD | NGX_HTTP_GET))) 
    {
       return NGX_HTTP_NOT_ALLOWED;
    }

    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK)
    {
       return rc;
    }
    
    ngx_str_t type = ngx_string("text/html");
    ngx_str_t response = ngx_string("hello world!");
    r->headers_out.content_type = type;
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = response.len;
    if (r->method == NGX_HTTP_HEAD)
    {
       return ngx_http_send_header(r);
    }
    
    ngx_buf_t* buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (buf == NULL)
    {
       return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    buf->pos = ngx_pcalloc(r->pool, response.len);
    buf->last = buf->pos + response.len;
    buf->last_buf = 1;
    buf->memory = 1;
    ngx_memcpy(buf->pos, response.data, response.len);
    
    ngx_chain_t out;
    out.buf = buf;
    out.next = NULL;
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)
    {
       return rc;
    }

    return ngx_http_output_filter(r, &out);
}




