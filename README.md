# http_server
简易的http服务器

0.参考自https://github.com/zouxiaohang/webserver 但采用线程池实现，且全部采用C实现

1.服务器模型采用epoll + 线程池，支持多用户并发访问

2.目前HTTP只支持GET和HEAD方法

3.编译方式: cd src && make (重新编译先make clean)

4.运行方式: ./http_server ./http.config

5../http.config为配置文件，支持domain和docroot配置项，domain为网站域名，docroot为资源的根目录
