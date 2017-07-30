#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

/*
location /test {
   my_string      smtl
   my_counter     1
   my_flag        on
}
*/

typedef struct {
   ngx_str_t arg_str;
   ngx_int_t arg_num;
   ngx_flag_t arg_flag;
}ngx_http_mytest2_loc_conf_t;

static void* ngx_http_mytest2_create_loc_conf(ngx_conf_t *cf);
static char* ngx_http_myflag(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_mytest2_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_mytest2_commands[] = {
   {
      ngx_string("my_string"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_mytest2_loc_conf_t, arg_str),
      NULL
   },
   {
      ngx_string("my_counter"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_mytest2_loc_conf_t, arg_num),
      NULL
   },
   {
      ngx_string("my_flag"),
      NGX_HTTP_LOC_CONF|NGX_CONF_FLAG|NGX_CONF_TAKE1,
      ngx_http_myflag,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_mytest2_loc_conf_t, arg_flag),
      NULL
   },
   ngx_null_command /*这句不能少*/
};

static ngx_http_module_t ngx_http_mytest2_module_ctx = {
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   ngx_http_mytest2_create_loc_conf,
   NULL
};

static ngx_module_t ngx_http_mytest2_module = {
   NGX_MODULE_V1,
   &ngx_http_mytest2_module_ctx,
   ngx_http_mytest2_commands,
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

static void* ngx_http_mytest2_create_loc_conf(ngx_conf_t * cf)
{
   ngx_http_mytest2_loc_conf_t *mlcf;
   mlcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_mytest2_loc_conf_t));
   if (mlcf == NULL) {
      return NGX_CONF_ERROR;
   }
   /*
   mlcf->arg_str.len = 0;
   mlcf->arg_str.data = NULL;
   */
   ngx_str_null(&mlcf->arg_str);
   mlcf->arg_num = NGX_CONF_UNSET;
   mlcf->arg_flag = NGX_CONF_UNSET;

   return mlcf;
}

static char* ngx_http_myflag(ngx_conf_t* cf, ngx_command_t* cmd, void *conf)
{
   ngx_http_core_loc_conf_t *clcf;
   clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
   clcf->handler = ngx_http_mytest2_handler;
   ngx_conf_set_flag_slot(cf, cmd, conf);
   return NGX_CONF_OK;
}

static ngx_int_t ngx_http_mytest2_handler(ngx_http_request_t* r)
{
   if (!(r->method & (NGX_HTTP_HEAD | NGX_HTTP_GET))) {
      return NGX_HTTP_NOT_ALLOWED;
   }
   
   ngx_int_t rc = ngx_http_discard_request_body(r);
   if (rc != NGX_OK) {
      return rc;
   }

   ngx_http_mytest2_loc_conf_t *mlcf;
   mlcf = ngx_http_get_module_loc_conf(r, ngx_http_mytest2_module);
   ngx_str_t arg_str = mlcf->arg_str;
   ngx_int_t arg_num = mlcf->arg_num;
   ngx_flag_t arg_flag = mlcf->arg_flag;
   ngx_str_t str_format = ngx_string("my_string=%V, my_counter = %i, my_flag = %i");
   ngx_int_t content_length = arg_str.len + str_format.len;
   
   ngx_str_t content_type = ngx_string("text/html");
   r->headers_out.content_type = content_type;
   r->headers_out.status = NGX_HTTP_OK;
   r->headers_out.content_length_n = content_length;
   if (r->method == NGX_HTTP_HEAD) {
      return ngx_http_send_header(r);
   }

   ngx_buf_t *b;
   b = ngx_pcalloc(r->pool, content_length);
   if (b == NULL) {
      return NGX_HTTP_INTERNAL_SERVER_ERROR;
   }
   ngx_snprintf(b->pos, content_length, (char*)str_format.data, &arg_str, arg_num, arg_flag);
   b->last = b->pos + content_length;
   b->memory = 1;
   b->last_buf = 1;
   rc = ngx_http_send_header(r);
   if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
      return rc;
   }
   ngx_chain_t out;
   out.buf = b;
   out.next = NULL;

   return ngx_http_output_filter(r, &out);
}





