#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct
{
    ngx_str_t    stock[6];
}ngx_http_mysubrequest_ctx_t;

static char* ngx_http_mysubrequest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_mysubrequest_handler(ngx_http_request_t *r);
static ngx_int_t mysubrequest_post_handler(ngx_http_request_t *r, void *data, ngx_int_t rc);
static void parent_request_post_handler(ngx_http_request_t *r);
                     
static ngx_command_t ngx_http_mysubrequest_commands[] = 
{
    {
        ngx_string("test_request"),
        NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
        ngx_http_mysubrequest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};
                         
static ngx_http_module_t ngx_http_mysubrequest_module_ctx = 
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
	NULL,
	NULL
};

ngx_module_t ngx_http_mysubrequest_module = 
{
	NGX_MODULE_V1,
    &ngx_http_mysubrequest_module_ctx,
	ngx_http_mysubrequest_commands,
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

static char* ngx_http_mysubrequest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_mysubrequest_handler;
    
	return NGX_CONF_OK;
}

//修改上下文和设置父请求的后处理handler
static ngx_int_t mysubrequest_post_handler(ngx_http_request_t *r, void *data, ngx_int_t rc)
{
	ngx_http_request_t* pr = r->parent;
    //ngx_http_mysubrequest_ctx_t* mctx = data;
    ngx_http_mysubrequest_ctx_t* mctx = ngx_http_get_module_ctx(pr, ngx_http_mysubrequest_module);
    pr->headers_out.status = r->headers_out.status;
    if (r->headers_out.status == NGX_HTTP_OK)
    {
        int count_num = 0;
        ngx_buf_t* rc_buf = &r->upstream->buffer;//buffer存储的接受上游的数据
        for (; rc_buf->pos!=rc_buf->last; ++rc_buf->pos)
        {
            if (*rc_buf->pos == ',' || *rc_buf->pos == '\"')
            {
                if (count_num > 0)
                {
                    mctx->stock[count_num-1].len = rc_buf->pos - mctx->stock[count_num].data;
                }
                ++count_num;
                mctx->stock[count_num-1].data = rc_buf->pos + 1;
            }

            if (count_num > 6) break;
        }
    }

    r->write_event_handler = parent_request_post_handler;
    return NGX_OK;
}

//主要是发送数据（上下文）
//其他的就是注意一些标记的设置
//主要增加的一项就是要手动调用ngx_http_finalize_request();
//
static void parent_request_post_handler(ngx_http_request_t *r)
{
	if (r->headers_out.status != NGX_HTTP_OK)
    {
        ngx_http_finalize_request(r, r->headers_out.status);
        return ;
    }

    ngx_http_mysubrequest_ctx_t* mctx = ngx_http_get_module_ctx(r, ngx_http_mysubrequest_module);
    ngx_str_t str_format = ngx_string("stock[%V], Today current price: %v, volumn: %V");
    ngx_int_t content_len = str_format.len + mctx->stock[0].len + mctx->stock[1].len 
                                           + mctx->stock[4].len - 6;
    ngx_buf_t* buf = ngx_create_temp_buf(r->pool, content_len);
    if (buf == NULL)
    {
        ngx_http_finalize_request(r, NGX_ERROR);
        return ;
    }
    ngx_snprintf(buf->pos, content_len, (char*)str_format.data, mctx->stock[0].data, mctx->stock[1].data, mctx->stock[4].data);
    buf->last = buf->pos + content_len;
    buf->last_buf = 1;
    buf->memory = 1;
    
    ngx_chain_t out;
    out.buf = buf;
    out.next = NULL;

    ngx_str_t type = ngx_string("text/plain;charset=GBK"); //这个字符编码原则
    r->headers_out.content_type = type;
    r->headers_out.content_length_n = content_len;
    r->headers_out.status = NGX_HTTP_OK;
    r->connection->buffered |= NGX_HTTP_WRITE_BUFFERED; //buffered的标识
    
    ngx_int_t rc = ngx_http_send_header(r);
    rc = ngx_http_output_filter(r, &out);

    ngx_http_finalize_request(r, rc);//必须手动调用ngx_http_finalize_request，结束请求，因为这时http框架不会再帮忙调用它
}

//hander处理主要有两个部分
//1为上下文申请内存空间
//2用ngx_http_subrquest创建子请求
static ngx_int_t ngx_http_mysubrequest_handler(ngx_http_request_t *r)
{
	ngx_http_mysubrequest_ctx_t* mctx = ngx_http_get_module_ctx(r, ngx_http_mysubrequest_module);
	if (mctx == NULL)
	{
	    mctx = ngx_palloc(r->pool, sizeof(ngx_http_mysubrequest_ctx_t));
        if (mctx == NULL)
        {
            return NGX_ERROR;
        } 
        ngx_http_set_ctx(r, mctx, ngx_http_mysubrequest_module);        
	}
	
    //构造ngx_http_subrequest的参数，创造子请求
	ngx_http_post_subrequest_t* psr = ngx_palloc(r->pool, sizeof(ngx_http_post_subrequest_t));
    if (psr == NULL)
    {
         return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    psr->data = mctx;
    psr->handler = mysubrequest_post_handler;
    
	ngx_str_t sub_prefix = ngx_string("/list=");
    ngx_str_t sub_location;
    sub_location.len = sub_prefix.len + r->args.len;
    sub_location.data = ngx_palloc(r->pool, sub_location.len);
    if (sub_location.data == NULL)
    {
        return NGX_ERROR;
    }
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "%s hihi", r->args.data); /////////////////////
    ngx_snprintf(sub_location.data, sub_location.len, "%V%V", &sub_prefix, &r->args);
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "%s hehe", sub_location.data);/////////////////

    ngx_http_request_t *sr;                                            
    ngx_int_t rc = ngx_http_subrequest(r, &sub_location, NULL, &sr, psr, NGX_HTTP_SUBREQUEST_IN_MEMORY);
    if (rc != NGX_OK)
    {
        return NGX_ERROR;
    }

    return NGX_DONE;
}




























