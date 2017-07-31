NGX_ROOT = /home/wyp/桌面/testkNx/nginx-1.0.15

ngx_http_myconfig_module.o: ngx_http_myconfig_module.c 
	gcc -c -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g   -I $(NGX_ROOT)/src/core -I $(NGX_ROOT)/src/event -I $(NGX_ROOT)/src/event/modules -I $(NGX_ROOT)/src/os/unix -I /home/wyp/下载/pcre-8.33 -I /home/wyp/下载/openssl-0.9.8o/.openssl/include -I /home/wyp/下载/zlib-1.2.8 -I $(NGX_ROOT)/objs -I $(NGX_ROOT)/src/http -I $(NGX_ROOT)/src/http/modules -I $(NGX_ROOT)/src/mail \
		-o $(NGX_ROOT)/objs/addon/myconfig/ngx_http_myconfig_module.o \
		/home/wyp/桌面/testkNx/nginx-1.0.15/myconfig/ngx_http_myconfig_module.c

