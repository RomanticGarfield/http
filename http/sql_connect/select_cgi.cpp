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
	}
    MYSQL* myfd = my_connect();
	my_select(myfd);
    my_close(myfd);
}
















