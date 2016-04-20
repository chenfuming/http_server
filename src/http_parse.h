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
/***********************************  HTTP״̬��  *******************************************/
#define HTTP_CONTINUE 		    100	    //�յ����������ʼ���֣��ͻ���Ӧ�ü�������
#define HTTP_OK				    200	    //�������Ѿ��ɹ���������
#define HTTP_ACCEPTED		    202	    //�����ѽ��ܣ���������δ����
#define HTTP_MOVED			    301	    //�����URL�����ߣ���ӦӦ�ð���Location URL
#define	HTTP_FOUND			    302	    //�����URL��ʱ���ߣ���ӦӦ�ð���Location URL
#define HTTP_SEEOTHER		    303	    //���߿ͻ���Ӧ������һ��URL��ȡ��Դ����ӦӦ�ð���Location URL
#define HTTP_NOTMODIFIED	    304	    //��Դδ�����仯
#define HTTP_BADREQUEST		400	    //�ͻ��˷�����һ���쳣����
#define HTTP_FORBIDDEN		    403	    //�������ܾ�����
#define HTTP_NOTFOUND		    404 	//URLδ�ҵ�
#define HTTP_ERROR			    500	    //����������
#define HTTP_NOIMPLEMENTED	501     //��������֧�ֵ�ǰ��������Ҫ��ĳ������
#define HTTP_BADGATEWAY		502	    //��Ϊ���������ʹ�õķ�����������������Ӧ�������ε���Ч��Ӧ
#define HTTP_SRVUNAVAILABLE	503     //������Ŀǰ�޷��ṩ������񣬹�һ��ʱ�����Իָ�
/******************************************************************************************/
/************************************ HTTP״̬*******************************/
#define HTTP_OK_STRING 			    "OK"
#define HTTP_BADREQUEST_STRING 	    "Bad Request"
#define HTTP_FORBIDDEN_STRING	        "Forbidden"
#define HTTP_NOTFOUND_STRING  		"Not Found"
#define HTTP_NOIMPLEMENTED_STRING	"No Implemented"
/***********************************  HTTP��Ӧ�ײ�  *******************************************/
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

//�����http request����������ֵ
typedef struct _http_header_t
{
	char method[16];
	char url[256];
	char version[16];
}http_header_t;

int parse_http_request(char *http_request, http_header_t *phttphdr);
void get_real_url(char *request_url, char *real_url);

#endif

