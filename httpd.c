#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <pthread.h>
#include "threadpool/threadpool.h"

#define MAX 1024
#define MAIN_PAGE "index.html"
#define PAGE_404 "wwwroot/404.html"

static void usage(const char *proc)
{
	printf("Usage: %s port\n", proc);
}
	
int startup(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("socket");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(port);

	if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0){
		perror("bind");
		exit(3);
	}

	if(listen(sock, 5) < 0){
		perror("listen");
		exit(4);
	}

	return sock;
}

void echo_404(int sock)
{
	char line[MAX];
	struct stat st;
	stat(PAGE_404, &st);

	sprintf(line, "HTTP/1.0 404 Not Found\r\n");
	send(sock, line, strlen(line), 0);
    sprintf(line, "Content-Type: text/html;application/x-csi;application/x-jpg\r\n");
    send(sock, line, strlen(line), 0);
	sprintf(line, "Content-Length: %d\r\n", st.st_size);
	send(sock, line, strlen(line), 0);
	sprintf(line, "\r\n");
	send(sock, line, strlen(line), 0);


	int fd = open(PAGE_404, O_RDONLY);
	sendfile(sock, fd, NULL, st.st_size);
	close(fd);
}

void echoError(int sock, int status_code)
{
	switch(status_code){
		case 400:
			break;
		case 404:
			echo_404(sock);
			break;
		case 403:
			break;
		case 500:
			break;
		case 503:
			break;
		default:
			break;
	}
}

int getLine(int sock, char line[], int len)
{
	char c = 'a';
	int i = 0;
	while(c != '\n' && i < len - 1){
		ssize_t s = recv(sock, &c, 1, 0);
		if(s > 0){
	    	if( c == '\r' ){// \r \r\n \n -> \n
	    		recv(sock, &c, 1, MSG_PEEK);
	    		if(c == '\n'){
	    			recv(sock, &c, 1, 0);
	    		}else{
	    			c = '\n';
	    		}
	    	}
	    	line[i++] = c;
		}else{
			break;
		}
	}
	line[i] = '\0';
	return i;
}

void clearHeader(int sock)
{
	char line[MAX];
	do{
		getLine(sock, line, sizeof(line));
	}while(strcmp(line, "\n"));
}

//GET, PATH EXIST, NO CGI, QUERY_STIRNG IS NULL
int echo_resource(int sock, char *path, int size)
{
	char line[MAX];
	clearHeader(sock);
	int fd = open(path, O_RDONLY);
	if(fd < 0){
		return 500;
	}

	sprintf(line, "HTTP/1.0 200 OK\r\n");
	send(sock, line, strlen(line), 0);
	char *p = path + strlen(path)-1;
	while(*p != '.'){
		p--;
	}
	if(strcmp(p,".css") == 0){
    	sprintf(line, "Content-Type: text/css\r\n");
	}
	else if(strcmp(p,".js") == 0){
    	sprintf(line, "Content-Type: application/x-javascript\r\n");
	}else{
    	sprintf(line, "Content-Type: text/html;application/x-csi;application/x-jpg\r\n");
	}
    send(sock, line, strlen(line), 0);

	sprintf(line, "Content-Length: %d\r\n", size);
	send(sock, line, strlen(line), 0);
	sprintf(line, "\r\n");
	send(sock, line, strlen(line), 0);

	sendfile(sock, fd, NULL, size);

	close(fd);
	return 200;
}

int exe_cgi(int sock, char *method, char *path, char *query_string)
{
	char line[MAX];
	int content_length = -1;

	char method_env[MAX/16];
	char query_string_env[MAX];
	char content_length_env[MAX];

	if(strcasecmp(method, "GET") == 0){
		clearHeader(sock);
	}else{
		do{
			getLine(sock, line, sizeof(line));
			//Content-Length: 23 
			if(strncmp(line, "Content-Length: ", 16) == 0){
				content_length = atoi(line+16);
			}
		}while(strcmp(line,"\n"));
		if(content_length == -1){
			return 400;
		}
	}

	int input[2];
	int output[2];

	pipe(input);
	pipe(output);

	pid_t id = fork();
	if(id < 0){
		return 500;
	}
	else if(id == 0){ //child
		close(input[1]);
		close(output[0]);

		dup2(input[0], 0);
		dup2(output[1], 1);

		sprintf(method_env, "METHOD=%s", method);
		putenv(method_env);

		if(strcasecmp(method, "GET") == 0){
			sprintf(query_string_env, "QUERY_STRING=%s", query_string);
			putenv(query_string_env);
		}else{
			sprintf(content_length_env, "CONTENT_LENGTH=%d", content_length);
			putenv(content_length_env);
		}

		execl(path, path, NULL);
		exit(1);
	}
	else{
		close(input[0]);
		close(output[1]);

		int i = 0;
		char c;
		if(strcasecmp(method, "POST") == 0){
			for(; i < content_length; i++){
				recv(sock, &c, 1, 0);
				write(input[1], &c, 1);
			}
		}

    	sprintf(line, "HTTP/1.0 200 OK\r\n");
    	send(sock, line, strlen(line), 0);
    	sprintf(line, "Content-Type: text/html;charset='gb2312'\r\n");
    	send(sock, line, strlen(line), 0);
    	sprintf(line, "\r\n");
    	send(sock, line, strlen(line), 0);

		while(read(output[0], &c, 1) > 0){
			send(sock, &c, 1, 0);
		}

		waitpid(id, NULL, 0);

		close(input[1]);
		close(output[0]);
	}
	return 200;
}

void *handlerRequest(void *arg)
{
	printf("get a new client, thread create success...\n");
	int sock = (int)arg;
	char line[MAX];
	char method[MAX/16];
	char url[MAX];
	char path[MAX];
	int i = 0;
	int j = 0;
	int status_code = 200;
	int cgi = 0;
	char *query_string = NULL;

	getLine(sock, line, MAX);

	while(i < sizeof(method)-1 && j < sizeof(line) && !isspace(line[j]))
	{
		method[i] = line[j];
		i++, j++;
	}
	method[i] = '\0';
	//GET POST Get Post get post
	if(strcasecmp(method, "GET") == 0){
	}
	else if(strcasecmp(method, "POST") == 0){
		cgi = 1;
	}
	else{
		clearHeader(sock);
		status_code = 400;
		goto end;
	}

	//method url http_version
	i = 0;
	while(j < sizeof(line) && isspace(line[j])){
		j++;
	}

	while(i < sizeof(url)-1 && j < sizeof(line) && !isspace(line[j]) ){
		url[i] = line[j];
		i++, j++;
	}
	url[i] = '\0';

	//url = /a/b/c/x?q=100&q=200
	if(strcasecmp(method, "GET") == 0){
		query_string = url;
		while(*query_string){
			if(*query_string == '?'){
				*query_string = '\0';
				query_string++;
				cgi = 1;
				break;
			}
			query_string++;
		}
	}

	//method, url[path, query_string(GET)]

	sprintf(path, "wwwroot%s", url);

	if(path[strlen(path)-1] == '/'){
		strcat(path, MAIN_PAGE);
	}
	printf("method: %s, url: %s, query_string: %s\n", method, path, query_string);

	struct stat st;
	if(stat(path, &st) < 0){
		clearHeader(sock);
		status_code = 404;
		goto end;
	}
	else{
		if(S_ISDIR(st.st_mode)){
			strcat(path, "/");
			strcat(path, MAIN_PAGE);
		}
		else if((st.st_mode & S_IXUSR) ||\
				(st.st_mode & S_IXGRP) ||\
				(st.st_mode & S_IXOTH)){
			cgi = 1;
		}else{
			//do nothing
		}

		//method(GET, POST), path(EXIST), cgi(0|1), query_string(GET)
		if(cgi)
		{
			status_code = exe_cgi(sock, method, path, query_string);
		}
		else
		{
			status_code = echo_resource(sock, path, st.st_size);
		}

	}

end:
	if(status_code != 200){
		echoError(sock, status_code);
	}
	close(sock);
}

//./httpd 8080
int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		usage(argv[0]);
		return 1;

	}
	signal(SIGPIPE, SIG_IGN);
	int listen_sock = startup(atoi(argv[1]));
	
	int epoll_fd = epoll_create(10);
	if (epoll_fd < 0) {
		perror("epoll_create");
		return 1;
	}
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = listen_sock;
	
	ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &event);
	if (ret < 0) {
		perror("epoll_ctl");
		return 1;
	}
	threadpool_t pool;
	threadpool_init(&pool,4);
	for( ; ; ){
		struct epoll_event events[10];
		int size = epoll_wait(epoll_fd, events, sizeof(events) / sizeof(events[0]), -1);
		if (size < 0) {
			perror("epoll_wait");
			continue;
		}
		for(int i = 0; i < size; ++i){
			//套接字描述符改变_处理连接请求
            if(events[i].data.fd == listen_sock){
 
				struct sockaddr_in client;
				socklen_t len = sizeof(client);
				int sock = accept(listen_sock, (struct sockaddr*)&client, &len);
				if(sock < 0){
					perror("accept");
					continue;
				}
				struct epoll_event ev;
				ev.data.fd = connect_fd;
				ev.events = EPOLLIN;
				int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &ev);
			}
			//有读事件发生
            else if(events[i].events & EPOLLIN){

				struct sockaddr_in client;
				socklen_t len = sizeof(client);
				int sock = accept(listen_sock, (struct sockaddr*)&client, &len);
				if(sock < 0){
					perror("accept");
					continue;
				}
				threadpool_add(&pool, handlerRequest, (void *)sock);//添加至线程池处理
			}
	}
	threadpool_destroy(&pool);
	close(listen_sock);
	return 0;
}

