#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	printf("hello CGI!\n");
	int len = -1;
	char buf[1024];
	if(getenv("METHOD")){
		if(strcasecmp(getenv("METHOD"), "GET") == 0){
			strcpy(buf, getenv("QUERY_STRING"));
		}else{
			len = atoi(getenv("CONTENT_LENGTH"));
			int i = 0;
			for(;i < len; i++){
				read(0, buf+i, 1);
				printf("%c ", buf[i]);
			}

			buf[i] = 0;
		}
	}

	//x=100&y=200
	int x,y;
	sscanf(buf, "x=%d&y=%d", &x, &y);

	printf("<html>\n");
	printf("<h1>%d + %d = %d</h1>\n", x, y, x + y);
	printf("</html>\n");
}


















