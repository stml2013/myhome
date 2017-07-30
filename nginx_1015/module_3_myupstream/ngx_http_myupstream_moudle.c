#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct
{
    ngx_http_status_t    status;
    ngx_str_t            backendServer;
}ngx_http_myupstream_ctx_t;

typedef struct
{
    ngx_http_upstream_conf_t upstream;
}ngx_http_myupstream_loc_conf_t;

static char* ngx_http_myupstream(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void* ngx_http_myupstream_create_loc_conf(ngx_conf_t *cf);
static char* ngx_http_myupstream_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t myupstream_create_request(ngx_http_request *r);
static ngx_int_t myupstream_process_header(ngx_http_request *r);
static ngx_int_t myupstream_process_status_line(ngx_http_request *r);

static ngx_str_t ngx_http_proxy_hide_headers[] =
{
    ngx_str_t("Date"),
    ngx_str_t("Server"),
    ngx_str_t("X-Pad"),
    ngx_str_t("X-Accel-Expires"),
    ngx_str_t("X-Accel-Redirect"),
    ngx_str_t("X-Accel-Limit-Rate"),
    ngx_str_t("X-Accel-Buffering"),
    ngx_str_t("X-Accle-Charset"),
    ngx_null_string
};

static ngx_command_t ngx_http_myupstream_commands =
{
    {
        ngx_string("myupstream"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SERVER_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
        ngx_http_myupstream,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};

static ngx_http_moudle_t ngx_http_myupstream_module_ctx =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ngx_http_myupstream_create_loc_conf,
    ngx_http_myupstream_merge_loc_conf
};

ngx_moudle_t ngx_http_myupstream_module =
{
    NGX_MODULE_V1,
    &ngx_http_myupstream_modlue_ctx,
    ngx_http_myupstream_commands,
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

static void* ngx_http_myupstream_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_myupstream_loc_conf_t *mlcf;
    mlcf = ngx_pnalloc(cf->poor, sizeof(ngx_http_myupstream_loc_conf_t));
    if (mlcf == NULL)
    {
        return NULL;
    }

    mlcf->upstream.connect_timeout = 60000;
    mlcf->upstream.send_timeout = 60000;
    mlcf->upstream.read_timeout = 60000;
    mlcf->upstream.store_access = 0600;

    mlcf->upstream.buffering = 0;
    mlcf->upstream.bufs.num = 8;
    mlcf->upstream.bufs.size = ngx_pagesize;
    mlcf->upstream.buffer_size = ngx_pagesize;
    mlcf->upstream.busy_buffers_size = 2 * ngx_pagesize;
    mlcf->upstream.temp_file_wtire_size = 2 * ngx_pagesize;
    mlcf->upstream.max_temp_file_size = 1024 * 1024 * 1024;

    mlcf->upstream.hide_headers = NGX_CONF_UNSET_PTR;
    mlcf->upstream.pass_headers = NGX_CONF_UNSET_PTR;
}

static char* ngx_http_myupstream_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_myupstream_loc_conf_t *prev = parent;
    ngx_http_myupstream_loc_conf_t *conf = child;

    ngx_hash_init_t hash;
    hash.max_size = 100;
    hash.bucket_size = 1024;
    hash.name = "proxy_headers_hash";
    if (ngx_http_upstream_hide_headers_hash(cf, &conf->upstream,
                        &prev->upstream, ngx_http_proxy_headers, &hash))
        != NGX_OK)
    {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}

//创建一个请求
static ngx_int_t myupstream_create_request(ngx_http_request *r)
{
    static ngx_str_t backendQueryLine =
        ngx_string("GET /search?q=%V HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n");
    ngx_int_t queryLineLen = backendQueryLine.len + r->args.len - 2;
    ngx_buf_t* b = ngx_create_temp_buf(r->pool, queryLineLen);
    if (b == NULL)
    {
        return NGX_ERROR;
    }
    b->last = b->pos + queryLineLen;
    b->last_buf = 1;
    b->memory = 1;
    ngx_snprintf(b->pos, queryLineLen, (char*)backendQueryLine.data, &r->args);

    r->upstream->request_bufs = ngx_alloc_chain_link(r->pool);
    if (r->upstream->request_bufs == NULL)
    {
        return NGX_ERROR;
    }

    r->upstream->request_bufs->buf = b;
    r->upstream->request_bufs->next = NULL;
    r->upstream->request_sent = 0;
    r->upstream->header_set = 0;
    r->header_hash = 1;

    return NGX_OK;
}

static ngx_int_t myupstream_process_status_line(ngx_http_request *r)
{
    size_t              len;
    ngx_int_t           rc;
    ngx_http_upstream_t *u;

    ngx_http_myupstream_ctx_t* ctx = ngx_http_get_module(r, ngx_http_myupstream_module);
    if (ctx == NULL)
    {
        return NGX_ERROR;
    }

    u = r->upstream;
    //http框架提供的ngx_http_parse_status_line方法可以解析http
    //响应行，它的输入就是收到的字符流和上下文中的ngx_http_status_t结构
    rc = ngx_http_parse_status_line(r, &u->buffer, &ctx->status);

    if (rc == NGX_AGAIN)
    {
        return rx;
    }

    if (rc == NGX_ERROR)
    {
        ngx_log_error(NGX_LOG_ERROR, r->connection->log, 0, "upstream sent no valid HTTP/1.0 header");
        r->http_version = NGX_HTTP_VERSION_9;
        u->state->status = NGX_HTTP_OK;
        return NGX_OK;
    }

    if (u->status)
    {
        u->state->status = ctx->status.code;
    }

    u->headers_in.status_n = ctx->status.code;
    len = ctx->status.end - ctxx->status.start;
    u->headers_in.status_line.len = len;
    u->headers_in.status_line.data = ngx_pnalloc(r->pool, len);
    if (u->headers_in.status_line.data == NULL)
    {
        return NGX_ERROR;
    }

    ngx_memcpy(u->headers_in.status_line.data, ctx->status.start, len);
    u->process_header = myupstream_process_header;

    return myupstream_process_header(r);
}



