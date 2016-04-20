/*
 *   Author		Chenfuming
 *   Email		flemingchan@outlook.com
 *   File Name	cfm_http.c
 *	 Desc		webserver主文件
 *   Date 		2016.04.15
 */
#include "http_main.h"

char http_docroot[256] = {0};
char http_domain[256] = {0};

/*
 *函数作用：从非阻塞的描述符读取数据
 *函数参数：描述符，buff
 *函数返回值: -1 失败， 0 成功
 */
static int read_buff_from_fd(int fd, char *buff)
{
	if (0 >= fd || NULL == buff)
	{
		return -1;
	}
	
	int ret = 0;
	int one_read_len = 0; //每次读到的长度
	int total_read_len = 0; //读到的总长度
	
	while (1)
	{	
		if ((one_read_len = read(fd, buff+total_read_len, sizeof(buff)-1)) > 0)
		{
			total_read_len += one_read_len;
		}
        else if (-1 == one_read_len && errno == EINTR)
        {
			continue;
        }
		else if (0 == one_read_len || (-1 == one_read_len && errno == EAGAIN))
		{
            /*接收缓存区已无数据可读*/
			break;
		}
		else
		{
			perror("read fd error");
			free(buff);
			ret = -1;
			break;
		}
		
	}

	return ret;
}

/*
 *函数作用：发送响应报文至客户端
 *函数参数：连接socket，解析好的http头部，状态码，响应报文头
 *函数返回值: NULL
 */
void send_to_client(int conn_sock, http_header_t *phttphdr, 
                const int http_codes, const char *response)
{
    assert(NULL != phttphdr && NULL != response);

    char real_url[256] = {0};	
	int fd = 0;
    int file_size = 0;
    int one_write_len = 0; //每次写的长度
	int total_write_len = 0; //写的总长度
    
    if ( http_codes == HTTP_BADREQUEST 	|| 
	    http_codes == HTTP_NOIMPLEMENTED 	||
	    http_codes == HTTP_NOTFOUND 		||
	    (http_codes == HTTP_OK && 0 == strncmp(phttphdr->method, "HEAD", 4)))
	{
		while ((one_write_len = write(conn_sock, response+total_write_len, strlen(response))) != 0)
		{
			if (errno == EAGAIN)
			{
                break;
			}
            else if (errno == EINTR)
			{
				continue;
			}
			else
			{
                perror("write conn_sock:");
				goto out;
			}
			total_write_len += one_write_len;
		}
	}
    /*处理GET请求*/
	if (http_codes == HTTP_OK && 0 == strncmp(phttphdr->method, "GET", 3))
	{
		while ((one_write_len = write(conn_sock, response+total_write_len, strlen(response))) != 0)
		{
			if (errno == EAGAIN)
			{
                break;
			}
            else if (errno == EINTR)
			{
				continue;
			}
			else
			{
                perror("write conn_sock:");
				goto out;
			}
			
			total_write_len += one_write_len;
		}
        
		get_real_url(phttphdr->url, real_url);
		fd = open(real_url, O_RDONLY);
		file_size = get_file_length(real_url);
        //printf("file_size = %d\n", file_size);
		total_write_len = 0;
   /*发送文件*/
	again:
	
		if ((sendfile(conn_sock, fd, (off_t*)&total_write_len, file_size)) < 0 &&
               (errno != EAGAIN))
		{      
			perror("sendfile");
            goto out;
		}
        
		if (total_write_len < file_size)
		{
			goto again;
		}
        
        //printf("*******************　send file OK! ****************\n");
	}
    
 out:
    
    close(fd);
    return;
}

/*
 *函数作用：处理客户端http请求
 *函数参数：连接socket，已接收的buff
 *函数返回值: NULL
 */
 void handle_http_request(int conn_sock, char *buff)
{
	assert(-1 != conn_sock && NULL != buff);

	char response[ONEKILO] = {0};
	http_header_t *phttphdr = (http_header_t *)malloc_safely(sizeof(http_header_t));
    
	/*解析http请求*/
	if (-1 == parse_http_request(buff, phttphdr))
	{
		perror("tyhp_parse_http_request: parse str_http_request failed");
		goto out;
	}

	//printf("mthod:%s, url:%s, version:%s\n", phttphdr->method, phttphdr->url, phttphdr->version);

	/*拼接响应报文头*/
	int http_codes = get_http_response_header(phttphdr, response);
    
	printf("******************* response: ********************\n%s", response);
    printf("**************************************************\n");
    
	/*发送响应报文*/
    send_to_client(conn_sock, phttphdr, http_codes, response);

out:
	/*释放资源*/    
	free(phttphdr);
	//close(conn_sock);
    
	return;
}

/*
 *函数作用：处理客户端链接的线程例程
 *函数参数：连接socket
 *函数返回值: NULL
 */
void *handle_conn_socket(void *arg)
{
	int conn_sock = *((int *)arg);
	char *buff = (char *)malloc_safely(ONEMEGA);
    
	printf("NO.%u thread runs now !!!\n", (unsigned int)pthread_self());
    	
    //关闭connfd的Nagle算法
	set_off_tcp_nagle(conn_sock);
       
	if (read_buff_from_fd(conn_sock, buff) != -1)
	{
		printf("******************* request: ********************\n%s", buff);
        printf("**************************************************\n");        
		handle_http_request(conn_sock, buff);
	}
     
    if (NULL != buff)
	{
		free(buff);
		buff = NULL;
	}
	close(conn_sock);
    
	return NULL;	
}

/*
 *函数作用：解析配置文件
 *函数参数：配置文件的路径
 *函数返回值: -1 失败， 0 成功
 */
int parse_config_file(const char *path)
{
	assert(NULL != path);
	
	FILE *fp = NULL;
	char *p = NULL;
	char str_line[256] = {0};
	int i = 0;
    
	if ((fp = fopen(path, "r")) == NULL)
	{
		printf("The config file does not exist\n");
		return -1;
	}
	//文件内容格式: docroot:/home/worke/..
	//				domain:www.fleminchan.com
	while (fgets(str_line, sizeof(str_line), fp) != NULL)
	{
		if ((p = strstr(str_line, DOCROOT)) != NULL)
		{
            p = p + strlen(DOCROOT) + 1;
            for (i = 0; p[i] != '\0' && p[i] != '\r' && p[i] != '\n'; i++)
            {
                http_docroot[i] = p[i];
            }
            
            http_docroot[i] = '\0';
			continue;
		}
		if ((p = strstr(str_line, DOMAIN)) != NULL)
		{
			p = p + strlen(DOMAIN) + 1;
            for (i = 0; p[i] != '\0' && p[i] != '\r' && p[i] != '\n'; i++)
            {
                http_domain[i] = p[i];
            }
            
            http_domain[i] = '\0';
			continue;
		}	

	}

	//printf("http_doroot:%s,http_domain:%s\n", http_docroot, http_domain);
	return 0;
}

static void signal_handler(int32_t sig)
{
	if (SIGPIPE == sig)
	{
		printf("SIGPIPE received.\n");
	}
}

int main(int argc, const char *argv[])
{
	int listen_socket;
	int epoll_fd;
	int conn_fd;
	int nfds;
	int i = 0;
	struct epoll_event events[MAX_EVENTS];
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(clientaddr);
	
	if (argc != 2)
	{
		printf("Usage: %s <config_path>\n", argv[0]);
		exit(-1);
	}
	//解析配置文件
	if (-1 == parse_config_file(argv[1]))
	{
		printf("parse_config_file error\n");
		exit(-1);
	}
    
    /*处理SIGPIPE信号，预防进程broken pipe直接退出*/
    signal(SIGPIPE, signal_handler);
    
	/*创建SOCKET并开启监听,默认设置为非阻塞模式*/
	listen_socket = create_and_listen();
	
	/*创建epoll*/
	epoll_fd = http_epoll_create(MAX_EVENTS);
	
	/*添加到epoll读event，边缘触发模式*/
	epoll_add_event(epoll_fd, listen_socket, EPOLLIN|EPOLLET);

	/*初始化线程池*/
	thread_pool_init(THREAD_POLL_SIZE);
	
	while (1)
	{
		nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds == -1)
		{
			perror("epoll_wait:");
			exit(-1);
		}

		for (i = 0; i < nfds; i++)
		{
			if (events[i].data.fd == listen_socket)
			{
				/*处理监听套接字触发的事件(新的连接)*/
				handle_accept(epoll_fd, listen_socket, (struct sockaddr*)&clientaddr, &addrlen);
			}
			else if (events[i].events & EPOLLIN)
			{
				conn_fd = events[i].data.fd;
				/*主循环不再监听该链接，转由线程池内处理链接业务*/
				epoll_del_event(epoll_fd, events[i].data.fd, EPOLLIN|EPOLLET);
				pool_add_worker(handle_conn_socket, (void *)&conn_fd);
			}
		}
	}

	close(listen_socket);
	thread_pool_destroy();
	
	return 0;
}

