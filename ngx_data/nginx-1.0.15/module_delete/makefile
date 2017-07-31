###########################################################
#### makefile 
###########################################################
CC = gcc
CCFLAGS = -pipe  -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Wunused-function \
          -Wunused-variable -Wunused-value -Werror -g

NGX_ROOT = /home/wyp/桌面/testkNx/nginx-1.0.15
TARGET_FILE = ngx_http_myconfig_module
CPP_DIR = myconfig

CPP_FILE = $(TARGET_FILE).c
OBJ_FILE = $(TARGET_FILE).o
CPP_FILE_PATH = $(NGX_ROOT)/$(CPP_DIR)/$(CPP_FILE)
OBJ_SAVE_PATH = $(NGX_ROOT)/objs/addon/$(CPP_DIR)/$(OBJ_FILE)

TARGETS = $(OBJ_FILE)
CLEAN_TARGETS = -rm -f $(OBJ_SAVE_PATH)

PCRE_PATH = /home/wyp/下载/pcre-8.33
SSL_PATH = /home/wyp/下载/openssl-0.9.8o/.openssl/include
ZLIB_PATH = /home/wyp/下载/zlib-1.2.8

HEADER_INCLUDES = -I. \
-I $(NGX_ROOT)/src/core\
-I $(NGX_ROOT)/src/event\
-I $(NGX_ROOT)/src/event/modules\
-I $(NGX_ROOT)/src/os/unix\
-I $(PCRE_PATH)\
-I $(SSL_PATH)\
-I $(ZLIB_PATH)\
-I $(NGX_ROOT)/objs\
-I $(NGX_ROOT)/src/http\
-I $(NGX_ROOT)/src/http/modules\
-I $(NGX_ROOT)/src/mail

all: $(TARGETS)
$(OBJ_FILE): $(CPP_FILE) 
	$(CC) -c $(CCFLAGS) $(HEADER_INCLUDES) -o $(OBJ_SAVE_PATH) $(CPP_FILE)

.PHONLY:clean
clean:
	$(CLEAN_TARGETS)
