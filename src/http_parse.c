/*
 *   Author		Chenfuming
 *   Email		flemingchan@outlook.com
 *   File Name	http_parse.c
 *	 Desc		http请求的处理
 *   Date 		2016.04.18
 */
#include "http_parse.h"

extern char http_docroot[64];
extern char http_domain[128];

/*
 *函数作用：解析http_request
 *函数参数：http_request为待解析的值，phttphdr保存了解析下来的值
 *函数返回值: 0表示解析成功，-1表示解析失败
 */
int parse_http_request(char *http_request, http_header_t *phttphdr)
{
	assert(NULL != http_request && NULL != phttphdr);

	char str_line[256] = {0};
	char *p = NULL;
	unsigned int i = 0, j = 0;
	
	//解析http请求包的起始行
	if((p = strstr(http_request, CRLF)) == NULL)
	{
		perror("parse_http_request: http_request has not a \\r\\n");
		return -1;
	}

	//获取到第一行类似于"GET / HTTP/1.1"
	strncpy(str_line, http_request, p-http_request);
	str_line[p-http_request] = '\0';
	
	//获取method "GET"
	 while (!ISspace(str_line[i]) && (j < sizeof(phttphdr->method) - 1))
	 {
	  	phttphdr->method[j] = str_line[i];
	  	i++; 
		j++;
	 }
	 phttphdr->method[j] = '\0';

	//获取url "/"
	j = 0;
	while (ISspace(str_line[i]) && str_line[i] != '\0') //跳过空格
	{
		i++;
	}
	while (!ISspace(str_line[i]) && (j < sizeof(phttphdr->url) - 1))
	{
		phttphdr->url[j] = str_line[i];
	  	i++; 
		j++;
	}
	phttphdr->url[j] = '\0';

	//获取http版本 "HTTP/1.1"
	while (ISspace(str_line[i]))//跳过空格
	{
		i++;
	}
	strncpy(phttphdr->version, str_line+i, sizeof(phttphdr->version));
		
	return 0;
}

/*
 *函数作用：获取系统时间
 *函数参数：str_time用来存放系统时间 例如：Fri, 22 May 2009 06:07:21 GMT
 *函数返回值: 无
 */
void get_time_gmt(char *str_time)
{
	assert(NULL != str_time);
	
	time_t now;
	struct tm *time_now;
	char buf[16] = {0};

	time(&now);
	time_now = localtime(&now);

	switch (time_now->tm_wday)
	{
		case 0:
			strcpy(str_time, "Sun, ");
			break;
		case 1:
			strcpy(str_time, "Mon, ");
			break;
		case 2:
			strcpy(str_time, "Tue, ");
			break;
		case 3:
			strcpy(str_time,  "Wed, ");
			break;
		case 4:
			strcpy(str_time, "Thu, ");
			break;
		case 5:
			strcpy(str_time, "Fri, ");
			break;
		case 6:
			strcpy(str_time, "Sat, ");
			break;
	}
	
	snprintf(buf, sizeof(buf), "%d ", time_now->tm_mday);
	strcat(str_time, buf);

	switch (time_now->tm_mon)
	{
		case 0:
			strcat(str_time, "Jan ");
			break;
		case 1:
			strcat(str_time, "Feb ");
			break;
		case 2:
			strcat(str_time, "Mar ");
			break;
		case 3:
			strcat(str_time, "Apr ");
			break;
		case 4:
			strcat(str_time, "May ");
			break;
		case 5:
			strcat(str_time, "Jun ");
			break;
		case 6:
			strcat(str_time, "Jul ");
			break;
		case 7:
			strcat(str_time, "Aug ");
			break;
		case 8:
			strcat(str_time, "Sep ");
			break;
		case 9:
			strcat(str_time, "Oct ");
			break;
		case 10:
			strcat(str_time, "Nov ");
			break;
		case 11:
			strcat(str_time, "Dec ");
			break;
	}
	snprintf(buf, sizeof(buf), "%d", time_now->tm_year + 1900);
	strcat(str_time, buf);
	snprintf(buf, sizeof(buf), " %d:%d:%d ", time_now->tm_hour, time_now->tm_min, time_now->tm_sec);
	strcat(str_time, buf);

	strcat(str_time, "GMT");

	return;
}

/*
 *函数作用：测试文件是否存在
 *函数参数：path为绝对路径+文件名
 *函数返回值: -1表示文件不存在，其他值表示文件存在
 */
inline int is_file_existed(const char *path)
{
	int ret = open(path, O_RDONLY | O_EXCL);
	close(ret);
	return ret;
}

/*
 *函数作用：根据http请求包中的url和配置文件中的docroot配置选项构造真正的url
 *函数参数：request_url
 *函数返回值: 真正的url(绝对路径)
 */
void get_real_url(char *request_url, char *real_url)
{
	assert(NULL != request_url && NULL != real_url);
		
	char temp_url[256] = {0};
	char *p = NULL;
	
	if ((p = strstr(request_url, http_domain)) != NULL)//url中包含域名，要将其删去
	{
		strncpy(temp_url, request_url+strlen(http_domain), sizeof(temp_url));
	}
	else
	{
		strncpy(temp_url, request_url, sizeof(temp_url));
	}
    
	if (http_docroot[strlen(http_docroot)-1] == '/')//配置项docroot末尾有'/'
	{
		if (temp_url[0] == '/')
		{
			strncpy(real_url, http_docroot, 255);
			strcat(real_url, temp_url+1);
		}
		else
		{
			strncpy(real_url, http_docroot, 255);
			strcat(real_url, temp_url);
		}
	}
	else//配置项docroot末尾没有'/'
	{
		if (temp_url[0] == '/')
		{
			strncpy(real_url, http_docroot, 255);
			strcat(real_url, temp_url);
		}
		else
		{
			strncpy(real_url, http_docroot, 255);
			strcat(real_url, "/");
			strcat(real_url, temp_url);
		}
	}
    
	return;
}

/*
 *函数作用：获得文件长度
 *函数参数：path为绝对路径+文件名
 *函数返回值: 文件长度
 */
 int get_file_length(const char *path)
 {
 	struct stat buf;
 	int ret = stat(path, &buf);
 	if (ret == -1)
 	{
 		perror("tyhp_get_file_length");
 		exit(-1);
 	}
 	return (int)buf.st_size;
 }
/*
 *函数作用：根据解析下来的http_header_t来处理客户的请求
 *函数参数：phttphdr:从客户的请求解析下来的http_header_t
 *			response:保存了处理的结果，即http响应包
 *函数返回值: HTTP状态码
 */
int get_http_response_header(http_header_t *phttphdr, char *response)
{
	assert(NULL != response && NULL != phttphdr);
	
	char server[32] = "Server: cfmhttp\r\n";
	char public[32] = "Public: GET, HEAD\r\n";	
	char content_length[32] = "Content-Length: ";
	char content_location[32] = "Content-Location: ";
	char content_base[64] = {0};
    char time_gmt[64] = {0};
	char date[128] = {0};
	char status_line[256] = {0};
	char real_url[256] = {0};
	char temp_buff[ONEKILO] = {0};	
	
	get_real_url(phttphdr->url, real_url);
	get_time_gmt(time_gmt);
	
	snprintf(date, sizeof(date), "Date: %s\r\n", time_gmt);
	snprintf(content_base, sizeof(content_base), "Content-Base: %s\r\n", http_domain);

	if (0 == strncmp(phttphdr->method, "GET", 3) || 0 == strncmp(phttphdr->method, "HEAD", 4))
	{
		if (is_file_existed(real_url) == -1) //请求的url不存在，返回404
		{
			snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n", 
				HTTP_NOTFOUND, HTTP_NOTFOUND_STRING);
			snprintf(temp_buff, sizeof(temp_buff), "%s%s%s\r\n", status_line, server, date);
            strncpy(response, temp_buff, ONEKILO-1);

			return HTTP_NOTFOUND;
		}
		else
		{
			int len = get_file_length(real_url);
			snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n", 
				HTTP_OK, HTTP_OK_STRING);
			snprintf(temp_buff, sizeof(temp_buff), "%s%s%d\r\n%s%s%s\r\n", 
				status_line, content_length, len, server, content_base, date);
            strncpy(response, temp_buff, ONEKILO-1);
		}
	}
	else if (0 == strncmp(phttphdr->method, "PUT", 3) ||
			  0 == strncmp(phttphdr->method, "DELETE", 6) ||
			  0 == strncmp(phttphdr->method, "POST", 4)) //后期可拓展支持这几种method
	{
		snprintf(status_line, sizeof(status_line), "HTTP/1.1 %d %s\r\n", 
				HTTP_NOIMPLEMENTED, HTTP_NOIMPLEMENTED_STRING);
		snprintf(temp_buff, sizeof(temp_buff), "%s%s%s%s\r\n", status_line, server, public, date); 
		strncpy(response, temp_buff, ONEKILO-1);
        
		return HTTP_NOIMPLEMENTED;
	}
	else //非法请求
	{
		snprintf(temp_buff, sizeof(temp_buff), "HTTP/1.1 %d %s\r\n\r\n", 
			HTTP_BADREQUEST, HTTP_BADREQUEST_STRING);
		strncpy(response, temp_buff, ONEKILO-1);
        
		return HTTP_BADREQUEST;
	}

	return HTTP_OK;
}

