#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct
{
   ngx_str_t arg_str;
   ngx_int_t arg_num;
}ngx_http_test_loc_conf_t;

static int ngx_hello_visited_times = 0;

static char* ngx_http_test_string(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char* ngx_http_test_counter(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void* ngx_http_test_create_loc_conf(ngx_conf_t *cf);
static ngx_int_t ngx_http_test_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_test_init(ngx_conf_t *cf);

static ngx_command_t ngx_http_test_commands[] = {
   {
       ngx_string("my_string"),
       NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
       ngx_http_test_string,
       NGX_HTTP_LOC_CONF_OFFSET,
       offsetof(ngx_http_test_loc_conf_t, arg_str),
       NULL 
   },
   {
       ngx_string("my_counter"),
       NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
       ngx_http_test_counter,
       NGX_HTTP_LOC_CONF_OFFSET,
       offsetof(ngx_http_test_loc_conf_t, arg_num),
       NULL 
   },
   ngx_null_command
};

static ngx_http_module_t ngx_http_test_module_ctx = {
        NULL,                             /* preconfiguration */
        ngx_http_test_init,            /* postconfiguration */

        NULL,                             /* create main configuration */
        NULL,                             /* init main configuration */

        NULL,                             /* create server configuration */
        NULL,                             /* merge server configuration */

        ngx_http_test_create_loc_conf, /* create location configuration */
        NULL                              /* merge location configuration */
};

/*static 加上这个static就报下面这个结构体没有引用*/
ngx_module_t ngx_http_test_module = {
        NGX_MODULE_V1,
        &ngx_http_test_module_ctx,    /* module context */
        ngx_http_test_commands,       /* module directives */
        NGX_HTTP_MODULE,                 /* module type */
        NULL,                            /* init master */
        NULL,                            /* init module */
        NULL,                            /* init process */
        NULL,                            /* init thread */
        NULL,                            /* exit thread */
        NULL,                            /* exit process */
        NULL,                            /* exit master */
        NGX_MODULE_V1_PADDING
};


static ngx_int_t ngx_http_test_handler(ngx_http_request_t *r)
{
        ngx_int_t    rc;
        ngx_buf_t   *b;
        ngx_chain_t  out;
        ngx_http_test_loc_conf_t* my_conf;
        u_char ngx_hello_string[1024] = {0};
        ngx_uint_t content_length = 0;

        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "ngx_http_hello_handler is called!");

        my_conf = ngx_http_get_module_loc_conf(r, ngx_http_test_module);
        if (my_conf->arg_str.len == 0 )
        {
           ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "hello_string is empty!");
           return NGX_DECLINED;
        }


        if (my_conf->arg_num == NGX_CONF_UNSET || my_conf->arg_num == 0)
        {
           ngx_sprintf(ngx_hello_string, "%s", my_conf->arg_str.data);
        }
        else
        {
           ngx_sprintf(ngx_hello_string, "%s Visited Times:%d", my_conf->arg_str.data,
                       ++ngx_hello_visited_times);
        }
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "hello_string:%s", ngx_hello_string);
        content_length = ngx_strlen(ngx_hello_string);

        if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) 
        {
           return NGX_HTTP_NOT_ALLOWED;
        }

        rc = ngx_http_discard_request_body(r);
        if (rc != NGX_OK)
        {
           return rc;
        }

        ngx_str_set(&r->headers_out.content_type, "text/html");
        if (r->method == NGX_HTTP_HEAD) 
        {
           r->headers_out.status = NGX_HTTP_OK;
           r->headers_out.content_length_n = content_length;
           return ngx_http_send_header(r);
        }

        b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
        if (b == NULL) 
        {
           return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }
        b->pos = ngx_hello_string;
        b->last = ngx_hello_string + content_length;
        b->memory = 1;    /* this buffer is in memory */
        b->last_buf = 1;  /* this is the last buffer in the buffer chain */

        out.buf = b;
        out.next = NULL;
        r->headers_out.status = NGX_HTTP_OK;
        r->headers_out.content_length_n = content_length;

        rc = ngx_http_send_header(r);

        if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) 
        {
           return rc;
        }

        return ngx_http_output_filter(r, &out);
}

static void* ngx_http_test_create_loc_conf(ngx_conf_t *cf)
{
        ngx_http_test_loc_conf_t* local_conf = NULL;
        local_conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_test_loc_conf_t));
        if (local_conf == NULL)
        {
                return NULL;
        }

        ngx_str_null(&local_conf->arg_str);
        local_conf->arg_num = NGX_CONF_UNSET;

        return local_conf;
}

static char* ngx_http_test_string(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
        ngx_http_test_loc_conf_t* local_conf;
        local_conf = conf;
        char* rv = ngx_conf_set_str_slot(cf, cmd, conf);

        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "my_string:%s", local_conf->arg_str.data);

        return rv;
}


static char* ngx_http_test_counter(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
        ngx_http_test_loc_conf_t* local_conf;
        local_conf = conf;
        char* rv = ngx_conf_set_flag_slot(cf, cmd, conf);

        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "my_counter:%d", local_conf->arg_num);
        return rv;
}

static ngx_int_t ngx_http_test_init(ngx_conf_t *cf)
{
        ngx_http_handler_pt        *h;
        ngx_http_core_main_conf_t  *cmcf;

        cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

        h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
        if (h == NULL) 
        {
           return NGX_ERROR;
        }

        *h = ngx_http_test_handler;
        return NGX_OK;
}


