#pragma once

#include <iostream>
#include <string>
#include <mysql.h>

MYSQL* my_connect();
int my_insert(MYSQL *myfd, const std::string &name,\
		const std::string &phone,const std::string &hobby);
int my_select(MYSQL *myfd);
void my_close(MYSQL *myfd);
