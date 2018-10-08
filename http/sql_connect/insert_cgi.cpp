#include <stdlib.h>
#include <string.h>
#include "comm.h"

int main()
{
	char buff[1024];
	if(getenv("METHOD")){
		if(strcasecmp(getenv("METHOD"), "GET") == 0){
			strcpy(buff, getenv("QUERY_STRING"));
		}else{
			int i=0;
			int len = atoi(getenv("CONTENT_LENGTH"));
			for(; i < len; i++){
				read(0, buff+i, 1);
			}
			buff[i] = 0;
		}
		//name=wangwu&phone=123456&hobby=123456
		strtok(buff, "=&");
		std::string name = strtok(NULL, "=&");
		strtok(NULL, "=&");
		std::string phone = strtok(NULL, "=&");
		strtok(NULL, "=&");
		std::string hobby = strtok(NULL, "=&");

    	MYSQL* myfd = my_connect();
    	my_insert(myfd, name, phone, hobby);
    	my_close(myfd);
	}
}


