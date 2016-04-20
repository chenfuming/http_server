#ifndef _HTTP_MAIN_H_
#define _HTTP_MAIN_H_

#include "http_parse.h"
#include "thread_pool.h"
#include "http_socket.h"
#include <signal.h>

void *handle_conn_socket(void *arg);
void handle_http_request(int conn_sock, char *buff);
int parse_config_file(const char *path);

#endif

