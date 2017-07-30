#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct
{
	ngx_flag_t arg_flag;
}ngx_http_myfilter_loc_conf_t;

typedef struct
{
    ngx_int_t   	add_prefix;
} ngx_http_myfilter_ctx_t;

//���ڰ�����������ǰ׺
static ngx_str_t filter_prefix = ngx_string("[my filter prefix]");
static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;
static ngx_http_output_body_filter_pt    ngx_http_next_body_filter;

static ngx_int_t ngx_http_myfilter_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_myfilter_header_filter(ngx_http_request_t *r);
static ngx_int_t ngx_http_myfilter_body_filter(ngx_http_request_t *r, ngx_chain_t *in);
static char* ngx_http_myfilter_set_flag(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void* ngx_http_myfilter_create_loc_conf(ngx_conf_t *cf);
static char* ngx_http_myfilter_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_command_t ngx_http_myfilter_commands[] = 
{
	{
		ngx_string("add_prefix"),
		NGX_HTTP_LOC_CONF|NGX_CONF_FLAG|NGX_CONF_TAKE1,
		ngx_http_myfilter_set_flag,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_myfilter_loc_conf_t, arg_flag),
		NULL
	},
	ngx_null_command
};

static ngx_http_module_t  ngx_http_myfilter_module_ctx =
{
    NULL,                                  /* preconfiguration����  */
    ngx_http_myfilter_init,                /* postconfiguration���� */

    NULL,                                  /*create_main_conf ���� */
    NULL,                                  /* init_main_conf���� */

    NULL,                                  /* create_srv_conf���� */
    NULL,                                  /* merge_srv_conf���� */

    ngx_http_myfilter_create_loc_conf,     /* create_loc_conf���� */
    ngx_http_myfilter_merge_loc_conf       /*merge_loc_conf����*/
};


ngx_module_t  ngx_http_myfilter_module =
{
    NGX_MODULE_V1,
    &ngx_http_myfilter_module_ctx,         /* module context */
    ngx_http_myfilter_commands,            /* module directives*/
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_int_t ngx_http_myfilter_init(ngx_conf_t *cf)
{
    //���뵽ͷ��������������ײ�
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_myfilter_header_filter;

    //���뵽���崦����������ײ�
    ngx_http_next_body_filter = ngx_http_top_body_filter;
    ngx_http_top_body_filter = ngx_http_myfilter_body_filter;
    
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "init over");


    return NGX_OK;
}

static ngx_int_t
ngx_http_myfilter_header_filter(ngx_http_request_t *r)
{
	ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "header filterAAAAA");
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "OUT_STATUS = %d xxxxxxx ", r->headers_out.status);//����

    ngx_http_myfilter_ctx_t   *ctx;

    //������Ƿ��سɹ�����ʱ�ǲ���Ҫ����Ƿ��ǰ׺�ģ�ֱ�ӽ�����һ������ģ��
    //������Ӧ���200������
    if (r->headers_out.status != NGX_HTTP_OK)
    {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "header filter 0000AAAA");
        return ngx_http_next_header_filter(r);
    }

    //��ȡhttp������
    ctx = ngx_http_get_module_ctx(r, ngx_http_myfilter_module);
    if (ctx)
    {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "header filter 11111");
        //��������������Ѿ����ڣ���˵��
        // ngx_http_myfilter_header_filter�Ѿ������ù�1�Σ�
        //ֱ�ӽ�����һ������ģ�鴦��
        return ngx_http_next_header_filter(r);
    }

    //����http�����Ľṹ��ngx_http_myfilter_ctx_t
    ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_myfilter_ctx_t));
    if (ctx == NULL)
    {
        return NGX_ERROR;
    }

	ngx_http_myfilter_loc_conf_t* mlcf = ngx_http_get_module_loc_conf(r, ngx_http_myfilter_module);
	if (mlcf == NULL || mlcf->arg_flag == 0)
	{
		return ngx_http_next_header_filter(r);
	}

    //add_prefixΪ0��ʾ����ǰ׺
    ctx->add_prefix = 0;

    //����������������õ���ǰ������
    ngx_http_set_ctx(r, ctx, ngx_http_myfilter_module);

    //myfilter����ģ��ֻ����Content-Type��"text/plain"���͵�http��Ӧ
    if (r->headers_out.content_type.len >= sizeof("text/plain") - 1
        && ngx_strncasecmp(r->headers_out.content_type.data, (u_char *) "text/plain", sizeof("text/plain") - 1) == 0)
    {
        //1��ʾ��Ҫ��http����ǰ����ǰ׺
        ctx->add_prefix = 1;

        //�������ģ���Ѿ���Content-Lengthд����http����ĳ��ȣ�����
        //���Ǽ�����ǰ׺�ַ�����������Ҫ������ַ����ĳ���Ҳ���뵽
        //Content-Length��
        if (r->headers_out.content_length_n > 0)
            r->headers_out.content_length_n += filter_prefix.len;
    }

    //������һ������ģ���������
    return ngx_http_next_header_filter(r);
}


static ngx_int_t
ngx_http_myfilter_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "body filter");
    ngx_http_myfilter_ctx_t   *ctx;
    ctx = ngx_http_get_module_ctx(r, ngx_http_myfilter_module);
//�����ȡ���������ģ����������Ľṹ���е�add_prefixΪ0����2ʱ��
//���������ǰ׺����ʱֱ�ӽ�����һ��http����ģ�鴦��
    if (ctx == NULL || ctx->add_prefix != 1)
    {
        return ngx_http_next_body_filter(r, in);
    }

//��add_prefix����Ϊ2��������ʹngx_http_myfilter_body_filter
//�ٴλص�ʱ��Ҳ�����ظ����ǰ׺
    ctx->add_prefix = 2;

//��������ڴ���з����ڴ棬���ڴ洢�ַ���ǰ׺
    ngx_buf_t* b = ngx_create_temp_buf(r->pool, filter_prefix.len);
//��ngx_buf_t�е�ָ����ȷ��ָ��filter_prefix�ַ���
    b->start = b->pos = filter_prefix.data;
    b->last = b->pos + filter_prefix.len;

//��������ڴ��������ngx_chain_t�������շ����ngx_buf_t���õ�
//��buf��Ա�У���������ӵ�ԭ�ȴ����͵�http����ǰ��
    ngx_chain_t *cl = ngx_alloc_chain_link(r->pool);
    cl->buf = b;
    cl->next = in;

//������һ��ģ���http���崦������ע����ʱ������������ɵ�cl����
    return ngx_http_next_body_filter(r, cl);
}


static char* ngx_http_myfilter_set_flag(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_myfilter_loc_conf_t *mlcf = conf;
	char* rc = ngx_conf_set_flag_slot(cf, cmd, conf);
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "arg_flag=%d DDDDDDD", mlcf->arg_flag);

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

