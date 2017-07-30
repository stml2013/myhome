CXX = gcc
CXXFLAGS +=-g -Wall -Wextra
NGX_ROOT = /home/wyp/桌面/testkNx/nginx-1.0.15
 
TARGET_FILE = ngx_list_t_test
SRC_FILE = $(TARGET_FILE).c
OBJ_FILE = $(TARGET_FILE).o
EXE_FILE = $(TARGET_FILE)
TARGETS  = $(EXE_FILE)

CLEAN_TARGETS = -rm -f $(TARGETS) *.o

#h文件
CORE_INCS = -I.\
-I $(NGX_ROOT)/src/core \
-I $(NGX_ROOT)/src/event \
-I $(NGX_ROOT)/src/event/modules \
-I $(NGX_ROOT)/src/os/unix \
-I $(NGX_ROOT)/objs #注意'\'后不能有空格，否则会有错误

#obj文件
NGX_PALLOC = $(NGX_ROOT)/objs/src/core/ngx_palloc.o
NGX_STRING = $(NGX_ROOT)/objs/src/core/ngx_string.o
NGX_ALLOC  = $(NGX_ROOT)/objs/src/os/unix/ngx_alloc.o
NGX_LIST   = $(NGX_ROOT)/objs/src/core/ngx_list.o
 
all:$(TARGETS)
$(EXE_FILE): $(OBJ_FILE)
	$(CXX) $(CXXFLAGS) $(CORE_INCS) $(NGX_PALLOC) $(NGX_STRING) $(NGX_ALLOC) $(NGX_LIST) $^ -o $@
$(OBJ_FILE): $(SRC_FILE)
	$(CXX) -c $(CXXFLAGS) $(CORE_INCS) $(NGX_PALLOC) $(NGX_STRING) $(NGX_ALLOC) $(NGX_LIST) $<

.PHONY:clean
clean:
	$(CLEAN_TARGETS)

#ngx_config.h ngx_conf_file.h nginx.h ngx_core.h ngx_string.h ngx_palloc.h
 
