#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_conf_enum_t test_enums[] = {
    { ngx_string("good"), 1},
    { ngx_string("better"), 2},
    { ngx_string("best"), 3},
    { ngx_null_string, 0}
}; 

typedef struct{
    ngx_str_t       arg_str;
    ngx_int_t       arg_counter;
    ngx_flag_t      arg_flag;
    ngx_uint_t      arg_enum_seq;
    ngx_bufs_t      arg_bufs;
}ngx_http_myconfig_loc_conf_t;

/*
location /myconfig {
    mystring    smile_to_life;
    mycounter   24;
    myflag      on;
    mygrage     better;
    mybufs      2 4k;
}
*/
static char* ngx_http_mystring(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char* ngx_http_mycounter(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char* ngx_http_myflag(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char* ngx_http_mygrade(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char* ngx_http_mybufs(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_myconfig_handler(ngx_http_request_t *r);
static void* ngx_http_myconfig_create_loc_conf(ngx_conf_t* cf);

static ngx_command_t ngx_http_myconfig_commands[] = {
    {
      ngx_string("mystring"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_mystring,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_myconfig_loc_conf_t, arg_str),//offsetof(type, member):&(((type*)0)->member)
      NULL
    },
    {
       ngx_string("mycounter"),
       NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
       ngx_http_mycounter,
       NGX_HTTP_LOC_CONF_OFFSET,
       offsetof(ngx_http_myconfig_loc_conf_t, arg_counter),
       NULL
    },
    {
       ngx_string("myflag"),
       NGX_HTTP_LOC_CONF | NGX_CONF_FLAG | NGX_CONF_TAKE1,//ngx_conf_t 的类型要用NGX_CONF_FLAG
       ngx_http_myflag,
       NGX_HTTP_LOC_CONF_OFFSET,
       offsetof(ngx_http_myconfig_loc_conf_t, arg_flag),
       NULL
    },
    {
       ngx_string("mygrade"),
       NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
       ngx_http_mygrade,
       NGX_HTTP_LOC_CONF_OFFSET,
       offsetof(ngx_http_myconfig_loc_conf_t, arg_enum_seq),
       test_enums //post = test_enums
    },
    {
       ngx_string("mybufs"),
       NGX_HTTP_LOC_CONF | NGX_CONF_TAKE2,//ngx_bufs_t 有两个数据成员
       ngx_http_mybufs,
       NGX_HTTP_LOC_CONF_OFFSET,
       offsetof(ngx_http_myconfig_loc_conf_t, arg_bufs),
       NULL
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_myconfig_module_ctx = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ngx_http_myconfig_create_loc_conf,
    NULL
};

ngx_module_t ngx_http_myconfig_module = {
    NGX_MODULE_V1,
    &ngx_http_myconfig_module_ctx,
    ngx_http_myconfig_commands,
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

static char* ngx_http_mystring(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "mystring begin");
    ngx_http_myconfig_loc_conf_t *mlcf;
    mlcf = conf;
    char* rt = ngx_conf_set_str_slot(cf, cmd, conf);
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "mystring = %s", mlcf->arg_str.data);

    ngx_http_core_loc_conf_t *clcf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_myconfig_handler;
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "handler");
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "mystring end");

    return rt;
}

static char* ngx_http_mycounter(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "c begin");
    ngx_http_myconfig_loc_conf_t *mlcf;
    mlcf = conf;
    char* rt = ngx_conf_set_num_slot(cf, cmd, conf);
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "num=%d c end", mlcf->arg_counter);

    return rt;
}

static char* ngx_http_myflag(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_myconfig_loc_conf_t *mlcf;
    mlcf = conf;
    char* rt = ngx_conf_set_flag_slot(cf, cmd, conf);
    
    int flag = mlcf->arg_flag;
    if (flag == 0)
    {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "myflag = on");
    }
    else
    {
         ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "myflag = off");
    }
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "myflag=%d", mlcf->arg_flag);
    return rt;
}

static char* ngx_http_mygrade(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "grage begin");
    ngx_http_myconfig_loc_conf_t *mlcf;
    mlcf = conf;
    char* rt = ngx_conf_set_enum_slot(cf, cmd, conf);
    
    ngx_str_t grade;
    if (mlcf->arg_enum_seq == 1)
    {
       ngx_str_set(&grade, "good");
    }
    else if(mlcf->arg_enum_seq == 2)
    {
       ngx_str_set(&grade, "better");
    }
    else
    {
       ngx_str_set(&grade, "best");
    }
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "mygrage = %s", grade.data);

    return rt;
}

static char* ngx_http_mybufs(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "buffer begin");
    ngx_http_myconfig_loc_conf_t *mlcf;
    mlcf = conf;
    char* rt = ngx_conf_set_bufs_slot(cf, cmd, conf);
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "buf_nums = %d, buf_size = %z", 
                                             mlcf->arg_bufs.num, mlcf->arg_bufs.size);

    return rt;
}

static void* ngx_http_myconfig_create_loc_conf(ngx_conf_t *cf)
{
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "create loc_conf_t begin");
    ngx_http_myconfig_loc_conf_t *mlcf;
    mlcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_myconfig_loc_conf_t));
    if (mlcf == NULL)
    {
       return NGX_CONF_ERROR;;
    }

    //ngx_str_set(&mlcf->arg_str, "");///////////nginx: [emerg] "mystring" directive is duplicate in,这个错误调试了一整天
    //看ngx_conf_set_str_slot的实现,当arg_str.data不为空时会返回“is duplicate”
    //
    ngx_str_null(&mlcf->arg_str);
    mlcf->arg_counter = NGX_CONF_UNSET;
    mlcf->arg_flag = NGX_CONF_UNSET;
    mlcf->arg_enum_seq = NGX_CONF_UNSET_UINT;
    //mlcf->arg_bufs不用初始化
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "create loc_conf_t end");
    return mlcf;
}

static ngx_int_t ngx_http_myconfig_handler(ngx_http_request_t *r)
{
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "ngx_http_myconfig_handler is called!");
    if (!(r->method & (NGX_HTTP_HEAD | NGX_HTTP_GET))) 
    {
       ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "method is failed!");
       return NGX_HTTP_NOT_ALLOWED;
    }

    ngx_int_t rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK)
    {
      ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "discard_qeuwest_body is failed!");
       return rc;
    }
    
    ngx_str_t type = ngx_string("text/html");
    r->headers_out.content_type = type;
    r->headers_out.status = NGX_HTTP_OK;
    if (r->method == NGX_HTTP_HEAD)
    {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "only header!");
        r->headers_out.content_length_n = type.len;
       return ngx_http_send_header(r);
    }
    
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "handling is beginning");
    ngx_http_myconfig_loc_conf_t *mlcf;
    mlcf = ngx_http_get_module_loc_conf(r, ngx_http_myconfig_module);
    if (mlcf == NULL)
    {
       ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, " mlcf is empty!");
       return NGX_ERROR;
    }
    ngx_str_t mystring = mlcf->arg_str;
    ngx_str_t myflag;
    if (mlcf->arg_flag == 1)
    {
       ngx_str_set(&myflag, "on");
    }
    else
    {
       ngx_str_set(&myflag, "off");
    }
    ngx_str_t grade;
    if (mlcf->arg_enum_seq == 1)
    {
       ngx_str_set(&grade, "good");
    }
    else if(mlcf->arg_enum_seq == 2)
    {
       ngx_str_set(&grade, "better");
    }
    else
    {
       ngx_str_set(&grade, "best");
    }
    ngx_str_t format = ngx_string("mystring=%s, mycounter=%d, myflag=%s, mygrage=%s, buf_num=%d, buf_size=%z");
    ngx_int_t content_length = format.len + mystring.len + myflag.len + grade.len;
    r->headers_out.content_length_n = content_length;
    
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "buffer is initing!");
    u_char* content_buf = (u_char*)ngx_pcalloc(r->pool, content_length);
    if (content_buf == NULL)
    {
       return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    ngx_sprintf(content_buf, (char*)format.data, mystring.data, mlcf->arg_counter, myflag.data, 
                grade.data, mlcf->arg_bufs.num, mlcf->arg_bufs.size);

    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "buffer is ended!");
    ngx_buf_t* buf = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (buf == NULL)
    {
       return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    buf->pos = content_buf;
    buf->last = buf->pos + content_length;
    buf->last_buf = 1;
    buf->memory = 1;
    
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


