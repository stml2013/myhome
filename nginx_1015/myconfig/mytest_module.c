class ngx_str_t;
class ngx_buf_t;
class ngx_chain_t;
class ngx_conf_t;
class ngx_command_t;
class ngx_module_t;
class ngx_http_module_t;

ngx_str_t ngx_string(const char*);
void *pcalloc(void*, void*);

#include <ngx_conf.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
	ngx_str_t ed;
}ngx_http_echo_loc_conf_t;

static char* ngx_http_echo(ngx_conf_t *cf, ngx_command_t *cmp, void *conf);
static void* ngx_http_echo_create_loc_conf(ngx_conf_t *cf);
static char* ngx_http_echo_merge_loc_conf(ngx_conf_t *cf, void *parent, void * child);
static ngx_int_t ngx_http_echo_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_echo_commands[] = {
	{
		ngx_string("echo"),
		NGX_HTTP_LOC_CONF|NGX_CONF_TAK1,
		ngx_http_echo,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_echo_loc_conf_t, ed),
		NULL
	},
	ngx_null_command
};

static ngx_http_module_t ngx_http_echo_module_ctx = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	ngx_http_echo_create_loc_conf,
	ngx_http_echo_merge_loc_conf
};

static ngx_module_t ngx_http_echo_moudle = {
	NGX_MODULE_V1,
	&ngx_http_echo_module_ctx,
	ngx_http_echo_commands,
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

/*
set是一个函数指针，用于指定一个参数转化函数，这个函数一般是将配置文件中相关指令的参数转化成
需要的格式并存入配置结构体。Nginx预定义了一些转换函数，可以方便我们调用，
这些函数定义在core/ngx_conf_file.h中，一般以“_slot”结尾，
例如ngx_conf_set_flag_slot将“on或off”转换为“1或0”，
再如ngx_conf_set_str_slot将裸字符串转化为ngx_str_t。
*/
static char* ngx_http_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_echo_handler;
	ngx_conf_set_str_slot(cf, cmd, conf);
	return NGX_CONF_OK;
}

static void* ngx_http_echo_create_loc_conf(ngx_conf_t *cf)
{
	ngx_http_echo_loc_conf_t *conf;
	conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_echo_loc_conf_t));
	if (conf == NULL) {
		return  NGX_CONF_ERROR;
	}
	conf->ed.len = 0;
	conf->ed.data = NULL;
	return conf;
}

static char* ngx_http_echo_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
	ngx_http_echo_loc_conf_t *prev = parent;
	ngx_http_echo_loc_conf_t *conf = child;
	ngx_conf_merge_str_value(conf->ed, prev->ed, "");
	return NGX_CONF_OK;
}

static ngx_int_t ngx_http_echo_handler(ngx_http_request_t *r)
{
	if (!(r->method & (NGX_HTTP_HEAD|NGX_HTTP_GET|NGX_HTTP_POST))) {
		return NGX_HTTP_NOT_ALLOWED;
	}

	ngx_http_echo_loc_conf_t *elcf;
	elcf = ngx_http_get_module_loc_conf(r, ngx_http_echo_moudle);
	r->headers_out.content_type = ngx_string("text/html");
	r->headers_out.status = NGX_HTTP_OK;
	r->headers_out.content_length_n = clef->ed.len;

	ngx_int_t rc;
	if (r->method == NGX_HTTP_HEAD) {
		rc = ngx_http_send_header(r);
		if (rc != NGX_OK) {
			return rc;
		}
	}

	ngx_buf_t *b;
	b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
	if (b == NULL) {
		ngx_log_error(NGX_LOG_ERROR, r->connection_log, 0, "Failed to allocate response buffer.");
		return NGX_HTTP_INTERNAL_SEVER_ERROR;
	}
	b->pos = elcf->ed.data;
	b->last = elcf->ed.data + elcf->ed.len;
	b->last_buf = 1;
	b->memroy = 1;

	ngx_chain_t out;
	out.buf = b;
	out.next = NULL;

	rc = ngx_http_send_header(r);
	if (rc != NGX_OK) {
		return rc;
	}

	return ngx_http_output_filter(r, &out);
}
