/*
 *   Author		Chenfuming
 *   Email		flemingchan@outlook.com
 *   File Name	http_socket.c
 *	 Desc		soket��epoll API�ķ�װ
 *   Date 		2016.04.15
 */
#include "http_socket.h"

/*
 *�������ã������׽���SO_REUSEADDRѡ��
 *����������Ҫ���õ��׽���
 *��������ֵ: ��
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
 *�������ã������ļ�������Ϊ������ģʽ
 *����������Ҫ���õ�������
 *��������ֵ: ��
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
 *�������ã������׽���TCP_NODELAYѡ��ر�nagle�㷨
 *����������Ҫ���õ��׽���
 *��������ֵ: ��
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
 *�������ã�����TCP�׽��ֲ��󶨼���
 *������������
 *��������ֵ: ����listen���׽���
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

	//���ü����׽���Ϊ������ģʽ
	set_fd_nonblocking(listen_sock);
	//�Լ����׽�������SO_REUSEADDRѡ��
	set_socket_reuseaddr(listen_sock);
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(80);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//��������sockaddr_in������׽��ְ�
	if (bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind:");
		exit(-1);
	}
	
	//��ʼ����
	if (listen(listen_sock, MAX_BACKLOG) == -1)
	{
		perror("listen:");
		exit(-1);
	}

	return listen_sock;
}

/*
 *�������ã�����epoll
 *����������epoll�����¼���
 *��������ֵ: epoll������
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
*�������ã����һ��epoll�¼�
*����������epoll��������Ҫ��ӵ�����������Ҫ����������
*��������ֵ: ����listen���׽���
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
*�������ã����һ��epoll�¼�
*����������epoll��������Ҫ��ӵ�����������Ҫ����������
*��������ֵ: ����listen���׽���
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
*�������ã����һ��epoll�¼�
*����������epoll��������Ҫ��ӵ�����������Ҫ����������
*��������ֵ: ����listen���׽���
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
*�������ã������µ�����
*����������epoll���������������������ͻ��˵�ַ�ͳ���
*��������ֵ: ��
*/
void handle_accept(int epoll_fd, int listenfd, struct sockaddr *cliaddr, socklen_t *addrlen)
{
	int conn_sock;

	/*
	 * �������ͬʱ����ʱ����������TCP��������˲����۶����������
	 * �����Ǳ�Ե����ģʽ��epollֻ��֪ͨһ��,������Ҫѭ��accept
	 * ֱ������-1����errnoΪEAGAIN(���������ѿ�)
	 */
	while ((conn_sock = accept(listenfd, cliaddr, (size_t *)addrlen)) > 0) 
	{
		printf("Client %s connected!\n", inet_ntoa(((struct sockaddr_in *)&cliaddr)->sin_addr));
		/*���������������÷�����ģʽ��������epoll����*/
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

