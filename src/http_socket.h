#ifndef _HTTP_SOCKET_H_
#define _HTTP_SOCKET_H_
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/tcp.h>

#define MAX_BACKLOG 100   //最大监听队列数
#define MAX_EVENTS	1024	//epoll最大监听事件数

/*
 *函数作用：设置套接字SO_REUSEADDR选项
 *函数参数：要设置的套接字
 *函数返回值: 无
 */
 void set_socket_reuseaddr(int sockfd);

/*
 *函数作用：设置文件描述符为非阻塞模式
 *函数参数：要设置的描述符
 *函数返回值: 无
 */
 void set_fd_nonblocking(int fd);
 /*
 *函数作用：开启套接字TCP_NODELAY选项，关闭nagle算法
 *函数参数：要设置的套接字
 *函数返回值: 无
 */
 void set_off_tcp_nagle(int sockfd);
 
/*
 *函数作用：创建TCP套接字并绑定监听
 *函数参数：无
 *函数返回值: 已在listen的套接字
 */
int create_and_listen();

/*
 *函数作用：创建epoll
 *函数参数：epoll监听事件数
 *函数返回值: epoll描述符
 */
int http_epoll_create(int size);

/*
 *函数作用：添加一个epoll事件
 *函数参数：epoll描述符，要添加的描述符，需要监听的类型
 *函数返回值: 已在listen的套接字
 */
void epoll_add_event(int epollfd, int fd, int state);

/*
 *函数作用：添加一个epoll事件
 *函数参数：epoll描述符，要添加的描述符，需要监听的类型
 *函数返回值: 已在listen的套接字
 */
void epoll_modify_event(int epollfd, int fd, int state);

/*
 *函数作用：添加一个epoll事件
 *函数参数：epoll描述符，要添加的描述符，需要监听的类型
 *函数返回值: 已在listen的套接字
 */
void epoll_del_event(int epollfd, int fd, int state);

/*
*函数作用：处理新的连接
*函数参数：epoll描述符，监听描述符，客户端地址和长度
*函数返回值: 无
*/
void handle_accept(int epoll_fd, int listenfd, struct sockaddr *cliaddr, socklen_t *addrlen);

void *malloc_safely(size_t size);
    
#endif

