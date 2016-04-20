/*
 *   Author		Chenfuming
 *   Email		flemingchan@outlook.com
 *   File Name	http_socket.c
 *	 Desc		soket和epoll API的封装
 *   Date 		2016.04.15
 */
#include "http_socket.h"

/*
 *函数作用：设置套接字SO_REUSEADDR选项
 *函数参数：要设置的套接字
 *函数返回值: 无
 */
 void set_socket_reuseaddr(int sockfd)
{
	int on = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret == -1)
	{
		perror("setsockopt: SO_REUSEADDR");
		exit(-1);
	}
}

/*
 *函数作用：设置文件描述符为非阻塞模式
 *函数参数：要设置的描述符
 *函数返回值: 无
 */
 void set_fd_nonblocking(int fd)
 {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
	{
		perror("fcntl: F_GETFL");
		exit(-1);
	}
	flags |= O_NONBLOCK;
	int ret = fcntl(fd, F_SETFL, flags);
	if (ret < 0)
	{
		perror("fcntl");
		exit(-1);
	}
 }
 /*
 *函数作用：开启套接字TCP_NODELAY选项，关闭nagle算法
 *函数参数：要设置的套接字
 *函数返回值: 无
 */
 void set_off_tcp_nagle(int sockfd)
 {
 	int on = 1;
 	int ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
 	if (ret == -1)
 	{
 		perror("setsockopt: TCP_NODELAY ON");
		exit(-1);
 	}
 }
/*
 *函数作用：创建TCP套接字并绑定监听
 *函数参数：无
 *函数返回值: 已在listen的套接字
 */
int create_and_listen()
{
	int listen_sock;
	struct sockaddr_in server_addr;
	
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == listen_sock)
	{
		perror("socket:");
		exit(-1);
	}

	//设置监听套接字为非阻塞模式
	set_fd_nonblocking(listen_sock);
	//对监听套接字设置SO_REUSEADDR选项
	set_socket_reuseaddr(listen_sock);
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(80);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//将服务器sockaddr_in与监听套接字绑定
	if (bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind:");
		exit(-1);
	}
	
	//开始监听
	if (listen(listen_sock, MAX_BACKLOG) == -1)
	{
		perror("listen:");
		exit(-1);
	}

	return listen_sock;
}

/*
 *函数作用：创建epoll
 *函数参数：epoll监听事件数
 *函数返回值: epoll描述符
 */
int http_epoll_create(int size)
{
	int epoll_fd;
	epoll_fd = epoll_create(size);

	if (-1 == epoll_fd)
	{
		perror("epoll_create");
		exit(-1);
	}

	return epoll_fd; 
}

/*
*函数作用：添加一个epoll事件
*函数参数：epoll描述符，要添加的描述符，需要监听的类型
*函数返回值: 已在listen的套接字
*/
void epoll_add_event(int epollfd, int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		perror("EPOLL_CTL_ADD");
		exit(-1);
	}	
}

/*
*函数作用：添加一个epoll事件
*函数参数：epoll描述符，要添加的描述符，需要监听的类型
*函数返回值: 已在listen的套接字
*/
void epoll_modify_event(int epollfd, int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;

	if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev) == -1)
	{
		perror("EPOLL_CTL_MOD");
		exit(-1);
	}	
}

/*
*函数作用：添加一个epoll事件
*函数参数：epoll描述符，要添加的描述符，需要监听的类型
*函数返回值: 已在listen的套接字
*/
void epoll_del_event(int epollfd, int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;

	if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev) == -1)
	{
		perror("EPOLL_CTL_DEL");
		exit(-1);
	}	
}

/*
*函数作用：处理新的连接
*函数参数：epoll描述符，监听描述符，客户端地址和长度
*函数返回值: 无
*/
void handle_accept(int epoll_fd, int listenfd, struct sockaddr *cliaddr, socklen_t *addrlen)
{
	int conn_sock;

	/*
	 * 多个连接同时到达时，服务器的TCP就绪队列瞬间积累多个就绪连接
	 * 由于是边缘触发模式，epoll只会通知一次,所以需要循环accept
	 * 直到返回-1并且errno为EAGAIN(就绪队列已空)
	 */
	while ((conn_sock = accept(listenfd, cliaddr, (size_t *)addrlen)) > 0) 
	{
		printf("Client %s connected!\n", inet_ntoa(((struct sockaddr_in *)&cliaddr)->sin_addr));
		/*把连接描述符设置非阻塞模式，并加入epoll监听*/
		set_fd_nonblocking(conn_sock);
    	epoll_add_event(epoll_fd, conn_sock, EPOLLIN|EPOLLET);
	}
	
	if (conn_sock == -1) 
	{
    	if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
    	{
    		perror("accept");
    	}
	}

	return;
}

void *malloc_safely(size_t size)
{
	void *ptr = malloc(size);
	
	if (NULL == ptr)
	{
		perror("malloc");
		exit(-1);
	}
	
	return ptr;
}

