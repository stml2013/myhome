1 ngx_strerror_init()，该函数的定义在文件src/os/unix/ngx_errno.c中。该函数主要初始化系统中
错误编号对应的含义，这样初始化中进行对应的好处是，当出现错误，不用再去调用strerror()函数来获
取错误原因，而直接可以根据错误编号找到对应的错误原因，可以提高运行时的执行效率;
2 ngx_get_options() 解析Nginx的启动参数;
3 ngx_time_init() 该函数的定义在文件src/core/ngx_time.c中。ngx_cached_err_log_time，
ngx_cached_http_time，ngx_cached_http_log_time，ngx_cached_http_log_iso8601均为
ngx_str_t类型的，用于记录错误日志时间，http缓存时间，http缓存log时间以及iso8061时间，初始化过程中
，先计算该时间表示的字符串的长度，这样可以省却在用到的时候再进行计算。ngx_cached_time是nginx时间类
型的数据结构，他是volatile类型的，即防止编译器优化，每次都要从内存中读取，而不是用缓存值。这个函数
调用了ngx_time_update()该函数的定义在文件src/core/ngx_time.c中。函数用于更新系统时间;
4 ngx_regex_init() src/core/ngx_regex.c,如果启用了PCRE功能，则进行正则表达式的初始化工作。Nginx中的pcre主要
是用来支持URL Rewrite的，URL Rewrite主要是为了满足代理模式下，对请求访问的URL地址进行rewrite操作，来实
现定向访问。这个函数现在不多介绍，以后学http模块在介绍;
5 ngx_log_init() src/core/ngx_log.c，初始化log日志，包括创建日志文件，log模块也是核心模块，以后在学习。
6 ngx_ssl_init() src/event/ngx_event_openssl.c,这个函数暂不介绍，event模块;
7 ngx_save_argv() 保存启动参数在ngx_argv数组里面，参数个数保存在ngx_argc里面;
8 ngx_process_options() 用old_cycle先保存一些配置信息：prefix,conf_prefix,conf_file, conf_param;
然后这个old_cycle将会作为ngx_init_cycle参数中的old_cycle;
9 ngx_os_init() src/os/unix/ngx_posix_init.c,初始化系统相关变量，如内存页面大小ngx_pagesize,ngx_cacheline_size,最大连接数ngx_max_sockets等;
10 ngx_crc32_table_init() 调用初始化CRC表,后续的CRC校验通过查表进行，效率高;
11 ngx_add_inherited_sockets() 继承sockets,并把相关信息存储在init_cycle这个后面会详细介绍；
12 初始化各个模块的index
13 ngx_init_cycle() 调用ngx_init_cycle，这个函数很重要，大部分初始化操作都在这个函数里面,后面会详细学习
14 假如有信号，就调用ngx_siganl_process处理信号
15 ngx_init_signal 初始化信号；主要完成信号处理程序的注册
16 若无继承sockets，且设置了守护进程标识，则调用ngx_daemon()创建守护进程
17 ngx_create_pidfile 调用ngx_create_pidfile()创建进程记录文件,记录进程ID
18 进入进程循环，如果ngx_process=1,就进入ngx_single_process_cycle
			        ngx_process!=1,就进入ngx_master_process_cycle
	ngx_signale_process_cycle和ngx_master_process_cycle这两个函数后面会重点学习。

1 ngx_strerror_init()
/*初始化系统错误编码，并将其对应的描述信息存放在ngx_sys_errlist中*/
ngx_uint_t
ngx_strerror_init(void)
{
    char       *msg;
    u_char     *p;
    size_t      len;
    ngx_err_t   err;

    len = NGX_SYS_NERR * sizeof(ngx_str_t);//数组内存大小

    ngx_sys_errlist = malloc(len);//申请存储错误的数组
    if (ngx_sys_errlist == NULL) {
        goto failed;
    }
    //编码范围是0-131,总共132个编码错误
    for (err = 0; err < NGX_SYS_NERR; err++) {
        //strerror通过标准错误的标号，获得错误的描述字符串 ，
        //将单纯的错误标号转为字符串描述，方便用户查找错误
        msg = strerror(err);
        len = ngx_strlen(msg);

        p = malloc(len);
        if (p == NULL) {
            goto failed;
        }

        ngx_memcpy(p, msg, len);//拷贝字符串
        ngx_sys_errlist[err].len = len;
        ngx_sys_errlist[err].data = p;//放入数组
    }

    return NGX_OK;

failed:

    err = errno;
    ngx_log_stderr(0, "malloc(%uz) failed (%d: %s)", len, err, strerror(err));

    return NGX_ERROR;
}

2 ngx_get_options()
/*
函数主要是解析Nginx的启动参数如下：
Options:
  -?,-h         : this help
  -v            : show version and exit
  -V            : show version and configure options then exit
  -t            : test configuration and exit
  -q            : suppress non-error messages during configuration testing
  -s signal     : send signal to a master process: stop, quit, reopen, reload
  -p prefix     : set prefix path (default: /nginx/)
  -c filename   : set configuration file (default: conf/nginx.conf)
  -g directives : set global directives out of configuration file
*/
static ngx_int_t
ngx_get_options(int argc, char *const *argv)
{
    u_char     *p;
    ngx_int_t   i;

    for (i = 1; i < argc; i++) {

        p = (u_char *) argv[i];

        if (*p++ != '-') {
            ngx_log_stderr(0, "invalid option: \"%s\"", argv[i]);
            return NGX_ERROR;
        }

        while (*p) {

            switch (*p++) {

            case '?':
            case 'h':
                ngx_show_version = 1;
                ngx_show_help = 1;
                break;

            case 'v':
                ngx_show_version = 1;
                break;

            case 'V':
                ngx_show_version = 1;
                ngx_show_configure = 1;
                break;

            case 't':
                ngx_test_config = 1;
                break;

            case 'q':
                ngx_quiet_mode = 1;
                break;

            case 'p':
                if (*p) {
                    ngx_prefix = p;
                    goto next;
                }

                if (argv[++i]) {
                    ngx_prefix = (u_char *) argv[i];
                    goto next;
                }

                ngx_log_stderr(0, "option \"-p\" requires directory name");
                return NGX_ERROR;

            case 'c':
                if (*p) {
                    ngx_conf_file = p;
                    goto next;
                }

                if (argv[++i]) {
                    ngx_conf_file = (u_char *) argv[i];
                    goto next;
                }

                ngx_log_stderr(0, "option \"-c\" requires file name");
                return NGX_ERROR;

            case 'g':
                if (*p) {
                    ngx_conf_params = p;
                    goto next;
                }

                if (argv[++i]) {
                    ngx_conf_params = (u_char *) argv[i];
                    goto next;
                }

                ngx_log_stderr(0, "option \"-g\" requires parameter");
                return NGX_ERROR;

            case 's':
                if (*p) {
                    ngx_signal = (char *) p;

                } else if (argv[++i]) {
                    ngx_signal = argv[i];

                } else {
                    ngx_log_stderr(0, "option \"-s\" requires parameter");
                    return NGX_ERROR;
                }

                if (ngx_strcmp(ngx_signal, "stop") == 0
                    || ngx_strcmp(ngx_signal, "quit") == 0
                    || ngx_strcmp(ngx_signal, "reopen") == 0
                    || ngx_strcmp(ngx_signal, "reload") == 0)
                {
                    ngx_process = NGX_PROCESS_SIGNALLER;
                    goto next;
                }

                ngx_log_stderr(0, "invalid option: \"-s %s\"", ngx_signal);
                return NGX_ERROR;

            default:
                ngx_log_stderr(0, "invalid option: \"%c\"", *(p - 1));
                return NGX_ERROR;
            }
        }

    next:

        continue;
    }

    return NGX_OK;
}


