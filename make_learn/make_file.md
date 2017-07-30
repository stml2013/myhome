################################################################
#编译nginx的makefaile
CXX = gcc
CXXFLAGS +=-g -Wall -Wextra
NGX_ROOT = /home/wyp/桌面/testNx/nginx-1.0.15
 
TARGET_FILE = ngx_list_t_test
SRC_FILE = $(TARGET_FILE).c
OBJ_FILE = $(TARGET_FILE).o
EXE_FILE = $(TARGET_FILE)
TARGETS  = $(EXE_FILE)

CLEAN_TARGETS= -rm -f $(TARGETS) *.o

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
##################################################################	
	


#main : main.o print.o computer.o
#       gcc -o main main.o print.o computer.o
#main.o : main.c print.h computer.h
#       gcc -c main.c
#print.o : print.c print.h
#       gcc -c print.c
#computer.o : computer.c computer.h
#       gcc -c computer.c
#
#makefile:11: *** 遗漏分隔符 。 停止.
#解决方法是在错误行之前加一个table建，即直接在gcc命令按tab健
main : main.o print.o computer.o	
	gcc -o $@ $^	
main.o : main.c print.h computer.h	
	gcc -c $<	
print.o : print.c print.h	
	gcc -c $<	
computer.o : computer.o computer.h
	gcc -c $<
.PHONY : clean
clean:
	-rm main main.o print.o computer.o	

#自动推到机制
main : main.o print.o computer.o
	gcc -o main main.o print.o computer.o
main.o : print.h computer.h
print.o : print.h
computer.o : computer.h

#宏定义
objects = main.o print.o computer.o
main : $(objects)
	gcc -o main $(objects)
main.o : main.c print.h computer.h
	gcc -c main.c
print.o : print.c print.h
	gcc -c print.c
computer.o : computer.c computer.h
	gcc -c computer.c
.PHONY: clean
clean:
	-rm main $(objects)

#简化
objects = main.o print.o computer.o
main : $(objects)
	gcc -o main $(objects)
main.o : print.h computer.h
print.o : print.h
computer.o : computer.h
.PHONY: clean
clean:
	-rm main $(objects)

#自动生成依赖性
cc -M main.c #main.c的依赖项,会打印出库依赖
gcc -MM main.c #不会打印出库依赖

4.3依赖的类型
1常规依赖项：依赖文件被更新后，需要更新规则的目标
2order-only:依赖文件被更新后，不需要更新规则的目标，这类文件常常用“|”符号隔开，并放在符号的左边
看一个列子：
LIBS = libtest.a
foo: foo.c|$(LIBS)
	$(CC) $(CFLAGS) $< -O $@ $(LIBS)
make在执行这个规则时,如果目标文件“foo”已经存在。当“foo.c”被修改以后,目标“foo”
将会被重建,但是当“libtest.a”被修改以后。将不执行规则的命令来重建目标“foo”

在 Makefile 有这样一个变量定义:“objects = *.o”。它表示变量“objects”的值是字符串“*.o”(并不是期
望的空格分开的.o 文件列表)。当需要变量“objects”代表所有.o 文件列表示,需要使
用函数“wildcard”(objects = $(wildcar *.o))。

4.43函数wildcard
一般我们可以使用“$(wildcard *.c)”来获取工作目录下的所有的.c 文件列表。复
杂一些用法;可以使用“$(patsubst %.c,%.o,$(wildcard *.c))”,首先使用“wildcard”
函数获取工作目录下的.c 文件列表;之后将列表中所有文件名的后缀.c 替换为.o。这样
我们就可以得到在当前目录可生成的.o 文件列表。因此在一个目录下可以使用如下内
容的 Makefile 来将工作目录下的所有的.c 文件进行编译并最后连接成为一个可执行文
件:
#sample Makefile
objects := $(patsubst %.c,%.o,$(wildcard *.c))
foo : $(objects)
	cc -o foo $(objects)

4.5目录搜寻
4.5.1VPATH
定义变量“VPATH”时,使用空格或者冒号(:)将多个需要搜索的目录分开。 make
搜索目录的顺序是按照变量“VPATH”定义中的目录顺序进行的(当前目录永远是第
一搜索目录)。例如对变量的定义如下:
VPATH = src:../headers
4.5.2 选择性搜索(关键字vpath)
另一个设置文件搜索路径的方法是使用 make 的“vpath”关键字(全小写的)。它
不是一个变量,而是一个 make 的关键字,它所实现的功能和上一小节提到的“VPATH”
变量很类似,但是它更为灵活。它可以为不同类型的文件(由文件名区分)指定不同的
搜索目录。它的使用方法有三种:
1、vpath PATTERN DIRECTORIES
为所有符合模式“PATTERN”的文件指定搜索目录“DIRECTORIES”。多个目
录使用空格或者冒号(:)分开。类似上一小节的“VPATH”变量。
    vpath %.h ../headers
2、vpath PATTERN
清除之前为符合模式“PATTERN”的文件设置的搜索路径。
3、vpath
清除所有已被设置的文件搜索路径。
vapth 使用方法中的“PATTERN”需要包含模式字符“%”。“%”意思是匹配一个
或者多个字符,例如,“%.h”表示所有以“.h”结尾的文件。
4.5.3
为了更清楚地描述此算法,我们使用一个例子来说明。存在一个目录“prom”,
“prom”的子目录“src”下存在“sum.c”和“memcp.c”两个源文件。在“prom”
目录下的 Makefile 部分内容如下:
LIBS = libtest.a
VPATH = src
libtest.a : sum.o memcp.o
$(AR) $(ARFLAGS) $@ $^
首先,如果在两个目录(“prom”和“src”)都不存在目标“libtest.a”,执行 make
时将会在当前目录下创建目标文件“libtest.a”。另外;如果“src”目录下已经存在
“libtest.a”,以下两种不同的执行结果:
1) 当它的两个依赖文件“sum.c”和“memcp.c”没有被更新的情况下我们执行
make,首先 make 程序会搜索到目录“src”下的已经存在的目标“libtest.a”。
由于目标“libtest.a”的依赖文件没有发生变化,所以不会重建目标。并且目标
所在的目录不会发生变化。
2) 当我们修改了文件“sum.c”或者“memcp.c”以后执行 make。“libtest.a”和
“sum.o”或者“memcp.o”文件将会被在当前目录下创建(目标完整路径名
被废弃),而不是在“src”目录下更新这些已经存在的文件。此时在两个目录下
(“prom”和“src”)同时存在文件“libtest.a”。但只有“prom/libtest.a”是最
新的库文件。
当在上边的 Makefile 文件中使用“GPATH”指定目录时,情况就不一样了。首先
看看怎么使用“GPATH”,改变后的 Makefile 内容如下:
LIBS = libtest.a
GPATH = src
VPATH = src
LDFLAGS += -L ./. –ltest
同样;当两个目录都不存在目标文件“libtest.a”时,目标将会在当前目录(“prom”
目录)下创建。如果“src”目录下已经存在目标文件“libtest.a”。当其依赖文件任何
一个被改变以后执行 make,目标“libtest.a”将会被在“src”目录下被更新(目标完
整路径名不会被废弃)。

