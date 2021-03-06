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
    wget https://jaist.dl.sourceforge.net/project/libpng/zlib/1.2.11/zlib-1.2.11.tar.gz 
    tar -zxf zlib-1.2.11.tar.gz
    mv zlib-1.2.11 $zlib_path 
    rm zlib-1.2.11.tar.gz
fi

if [ ! -d "$openssl_path" ]; then
    wget https://www.openssl.org/source/openssl-1.0.1t.tar.gz
    tar -zxf openssl-1.0.1t.tar.gz
    mv openssl-1.0.1t $openssl_path 
    rm openssl-1.0.1t.tar.gz
fi

if [ ! -d "$pcre_path" ]; then
    wget http://downloads.sourceforge.net/project/pcre/pcre/8.35/pcre-8.35.tar.gz
    tar -zxf pcre-8.35.tar.gz
    mv pcre-8.35 $pcre_path
    rm pcre-8.35.tar.gz
fi

cd ./nginx_1015

./configure --prefix=$prefix \
    --with-openssl=$openssl_path \
    --with-zlib=$zlib_path \
    --with-pcre=$pcre_path \
    --with-http_stub_status_module \
    --with-http_ssl_module

make 
sudo make install
