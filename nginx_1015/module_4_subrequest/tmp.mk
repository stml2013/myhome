gcc -c -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror -g   -I src/core -I src/event -I src/event/modules -I src/os/unix -I /home/wyp/下载/pcre-8.33 -I /home/wyp/下载/openssl-0.9.8o/.openssl/include -I /home/wyp/下载/zlib-1.2.8 -I objs -I src/http -I src/http/modules -I src/mail \
		-o objs/addon/module_4_subrequest/ngx_http_mysubrequest_module.o \
		/home/wyp/桌面/testkNx/nginx-1.0.15/module_4_subrequest/ngx_http_mysubrequest_module.c

