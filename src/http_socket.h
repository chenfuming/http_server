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

#define MAX_BACKLOG 100   //������������
#define MAX_EVENTS	1024	//epoll�������¼���

/*
 *�������ã������׽���SO_REUSEADDRѡ��
 *����������Ҫ���õ��׽���
 *��������ֵ: ��
 */
 void set_socket_reuseaddr(int sockfd);

/*
 *�������ã������ļ�������Ϊ������ģʽ
 *����������Ҫ���õ�������
 *��������ֵ: ��
 */
 void set_fd_nonblocking(int fd);
 /*
 *�������ã������׽���TCP_NODELAYѡ��ر�nagle�㷨
 *����������Ҫ���õ��׽���
 *��������ֵ: ��
 */
 void set_off_tcp_nagle(int sockfd);
 
/*
 *�������ã�����TCP�׽��ֲ��󶨼���
 *������������
 *��������ֵ: ����listen���׽���
 */
int create_and_listen();

/*
 *�������ã�����epoll
 *����������epoll�����¼���
 *��������ֵ: epoll������
 */
int http_epoll_create(int size);

/*
 *�������ã����һ��epoll�¼�
 *����������epoll��������Ҫ��ӵ�����������Ҫ����������
 *��������ֵ: ����listen���׽���
 */
void epoll_add_event(int epollfd, int fd, int state);

/*
 *�������ã����һ��epoll�¼�
 *����������epoll��������Ҫ��ӵ�����������Ҫ����������
 *��������ֵ: ����listen���׽���
 */
void epoll_modify_event(int epollfd, int fd, int state);

/*
 *�������ã����һ��epoll�¼�
 *����������epoll��������Ҫ��ӵ�����������Ҫ����������
 *��������ֵ: ����listen���׽���
 */
void epoll_del_event(int epollfd, int fd, int state);

/*
*�������ã������µ�����
*����������epoll���������������������ͻ��˵�ַ�ͳ���
*��������ֵ: ��
*/
void handle_accept(int epoll_fd, int listenfd, struct sockaddr *cliaddr, socklen_t *addrlen);

void *malloc_safely(size_t size);
    
#endif

