!/usr/bin/bash

prefix=$PWD
if [ "xx$1" != "xx" ]; then
    prefix=$1
fi

tmp_dir=$prefix/tmp
zlib_path=$tmp_dir/zlib_
openssl_path=$tmp_dir/openssl_
pcre_path=$tmp_dir/pcre_

if [ ! -d "$tmp_dir" ]; then
    mkdir $tmp_dir
fi

if [ ! -d "$zlib_path" ]; then
    wget http://zlib.net/zlib-1.2.8.tar.gz
    tar -zxf zlib-1.2.8.tar.gz
    mv zlib-1.2.8 $zlib_path 
fi

if [ ! -d "$openssl_path" ]; then
    wget https://www.openssl.org/source/openssl-1.0.1t.tar.gz
    tar -zxf openssl-1.0.1t.tar.gz
    mv openssl-1.0.1t $openssl_path 
fi

if [ ! -d "$pcre_path" ]; then
    wget http://downloads.sourceforge.net/project/pcre/pcre/8.35/pcre-8.35.tar.gz
    tar -zxf pcre-8.35.tar.gz
    mv pcre-8.35 $pcre_path
fi

cd /Users/didi/git_ngx/nginx_1015

./configure --sbin-path=$prefix \
    --with-openssl=$openssl_path \
    --with-pcre=$pcre_path \
    --with-zlib=$zlib_path \
    --with-http_stub_status_module \
    --with-http_ssl_module

make 
make install
