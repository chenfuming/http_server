#ifndef _HTTP_PARSE_H_
#define _HTTP_PARSE_H_
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#define ONEKILO		1024            // 1K
#define ONEMEGA		1024*ONEKILO    // 1M    
#define ONEGIGA		1024*ONEMEGA    // 1G     

#define CRLF      "\r\n"
#define CRLFCRLF "\r\n\r\n"
#define DOCROOT   "docroot"
#define DOMAIN    "domain"	 
#define LOGLEVEL  "loglevel"
/***********************************  HTTP状态码  *******************************************/
#define HTTP_CONTINUE 		    100	    //收到了请求的起始部分，客户端应该继续请求
#define HTTP_OK				    200	    //服务器已经成功处理请求
#define HTTP_ACCEPTED		    202	    //请求已接受，服务器尚未处理
#define HTTP_MOVED			    301	    //请求的URL已移走，响应应该包含Location URL
#define	HTTP_FOUND			    302	    //请求的URL临时移走，响应应该包含Location URL
#define HTTP_SEEOTHER		    303	    //告诉客户端应该用另一个URL获取资源，响应应该包含Location URL
#define HTTP_NOTMODIFIED	    304	    //资源未发生变化
#define HTTP_BADREQUEST		400	    //客户端发送了一条异常请求
#define HTTP_FORBIDDEN		    403	    //服务器拒绝请求
#define HTTP_NOTFOUND		    404 	//URL未找到
#define HTTP_ERROR			    500	    //服务器出错
#define HTTP_NOIMPLEMENTED	501     //服务器不支持当前请求所需要的某个功能
#define HTTP_BADGATEWAY		502	    //作为代理或网关使用的服务器遇到了来自响应链中上游的无效响应
#define HTTP_SRVUNAVAILABLE	503     //服务器目前无法提供请求服务，过一段时间后可以恢复
/******************************************************************************************/
/************************************ HTTP状态*******************************/
#define HTTP_OK_STRING 			    "OK"
#define HTTP_BADREQUEST_STRING 	    "Bad Request"
#define HTTP_FORBIDDEN_STRING	        "Forbidden"
#define HTTP_NOTFOUND_STRING  		"Not Found"
#define HTTP_NOIMPLEMENTED_STRING	"No Implemented"
/***********************************  HTTP响应首部  *******************************************/
#define HTTP_ACCEPTRANGE_HEAD			"Accpet-Range"
#define	HTTP_AGE_HEAD 					"Age"
#define	HTTP_ALLOW_HEAD				"Allow"
#define	HTTP_CONTENTBASE_HEAD			"Content-Base"
#define	HTTP_CONTENTLENGTH_HEAD		"Content-Length"
#define	HTTP_CONTENTLOCATION_HEAD	"Content-Location"
#define	HTTP_CONTENTRANGE_HEAD		"Content-Range"
#define	HTTP_CONTENTTYPE_HEAD			"Content-Type"
#define	HTTP_DATE_HEAD					"Date"
#define	HTTP_EXPIRES_HEAD				"Expires"
#define	HTTP_LAST_MODIFIED_HEAD		"Last-Modified"
#define	HTTP_LOCATION_HEAD 			"Location"
#define	HTTP_PUBLIC_HEAD				"Public"
#define HTTP_RANGE_HEAD 				"Range"
#define	HTTP_SERVER_HEAD				"Server"
/******************************************************************************************/

#define ISspace(x) isspace((int)(x))

//保存从http request解析下来的值
typedef struct _http_header_t
{
	char method[16];
	char url[256];
	char version[16];
}http_header_t;

int parse_http_request(char *http_request, http_header_t *phttphdr);
void get_real_url(char *request_url, char *real_url);

#endif

