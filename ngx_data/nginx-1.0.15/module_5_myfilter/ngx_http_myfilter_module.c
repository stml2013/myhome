#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct
{
	ngx_flag_t arg_flag;
}ngx_http_myfilter_loc_conf_t;

typedef struct
{
    ngx_int_t flag_count;
}ngx_http_myfilter_ctx_t;

static ngx_str_t filter_prefix = ngx_string("[my filter prefix]");
//必须是static变量，因为每个过滤文件中都有这两个变量，static限定这两个变量只在当前文件中有效
static ngx_http_output_header_filter_pt ngx_http_next_header_filter; 
static ngx_http_output_body_filter_pt   ngx_http_next_body_filter;

static char* ngx_http_myfilter_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void* ngx_http_myfilter_create_loc_conf(ngx_conf_t *cf);
static char* ngx_http_myfilter_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_myfilter_post_config(ngx_conf_t *cf);
static ngx_int_t ngx_http_myfilter_header_filter(ngx_http_request_t *r);
static ngx_int_t ngx_http_myfilter_body_filter(ngx_http_request_t *r, ngx_chain_t *in);
           
static ngx_command_t ngx_http_myfilter_commands[] = 
{
    {
        ngx_string("add_prefix"),
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
        ngx_http_myfilter_set,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_myfilter_loc_conf_t, arg_flag),
        NULL
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_myfilter_module_ctx = 
{
	NULL,
	ngx_http_myfilter_post_config,
	NULL,
	NULL,
	NULL,
	NULL,
	ngx_http_myfilter_create_loc_conf,
	ngx_http_myfilter_merge_loc_conf
};

ngx_module_t ngx_http_myfilter_module = 
{
	NGX_MODULE_V1,
	&ngx_http_myfilter_module_ctx,
	ngx_http_myfilter_commands,
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


static ngx_int_t ngx_http_myfilter_post_config(ngx_conf_t *cf)
{
	ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter  = ngx_http_myfilter_header_filter;

    ngx_http_next_body_filter   = ngx_http_top_body_filter;
    ngx_http_top_body_filter    = ngx_http_myfilter_body_filter;

    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "post config is called!");
    
    return NGX_OK;
}

static char* ngx_http_myfilter_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_myfilter_loc_conf_t *mlcf = conf;
    char* rc = ngx_conf_set_flag_slot(cf, cmd, conf);
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "arg_flag = %d", mlcf->arg_flag);

	return rc;
}


static void* ngx_http_myfilter_create_loc_conf(ngx_conf_t *cf)
{
	ngx_http_myfilter_loc_conf_t *mlcf;
    mlcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_myfilter_loc_conf_t));
	if (mlcf == NULL)
    {
		return NULL;
    }
    
    mlcf->arg_flag = NGX_CONF_UNSET;

    return mlcf;
}

static char* ngx_http_myfilter_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
	ngx_http_myfilter_loc_conf_t* prev = parent;
	ngx_http_myfilter_loc_conf_t* conf = child;
	
	ngx_conf_merge_value(conf->arg_flag, prev->arg_flag, 0);

	return NGX_CONF_OK;
}

static ngx_int_t ngx_http_myfilter_header_filter(ngx_http_request_t *r)
{
	if (r->headers_out.status != NGX_HTTP_OK)
    {
		ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "headers_out.status = %d", r->headers_out.status);
		return ngx_http_next_header_filter(r);
	}

	ngx_http_myfilter_loc_conf_t *mlcf;
	mlcf = ngx_http_get_module_loc_conf(r, ngx_http_myfilter_module);
	if (mlcf->arg_flag == 0) //flag = 0,表示不加前缀
	{
		ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "arg_flag = 0");
		return ngx_http_next_header_filter(r);
	}

	ngx_http_myfilter_ctx_t *mctx;
	mctx = ngx_http_get_module_ctx(r, ngx_http_myfilter_module);
	if (mctx != NULL) //mctx不为空，表示这个头部已经处理过，不许要再处理了
	{
		return ngx_http_next_header_filter(r);
	}
	
	mctx = ngx_pcalloc(r->pool, sizeof(ngx_http_myfilter_ctx_t));
	if (mctx == NULL)
	{
		return NGX_ERROR;
	}

	ngx_http_set_ctx(r, mctx, ngx_http_myfilter_module);
	mctx->flag_count = 0; //0：初始化为不加前缀
	ngx_str_t type = ngx_string("text/plain");
	if (r->headers_out.content_type.len >= type.len
		&& ngx_strncasecmp(r->headers_out.content_type.data, type.data, type.len) == 0)
	{
		mctx->flag_count = 1; //1：头部类型为“text/plain”的要加前缀
		if (r->headers_out.content_length_n > 0)
		{
			r->headers_out.content_length_n += filter_prefix.len;
		}
	}

	return ngx_http_next_header_filter(r);
}

static ngx_int_t ngx_http_myfilter_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
	ngx_http_myfilter_ctx_t *mctx;
	mctx = ngx_http_get_module_ctx(r, ngx_http_myfilter_module);
	if (mctx == NULL || mctx->flag_count != 1)
	{
		return ngx_http_next_body_filter(r, in);
	}

	mctx->flag_count = 2;
	ngx_buf_t* buf = ngx_create_temp_buf(r->pool, filter_prefix.len);//????????有问题
	buf->start = filter_prefix.data;
	buf->pos   = filter_prefix.data;
    buf->last  = filter_prefix.data + filter_prefix.len;

	ngx_chain_t* out = ngx_alloc_chain_link(r->pool);
	out->buf         = buf;
    out->next        = in;

    return ngx_http_next_body_filter(r, out);
}













