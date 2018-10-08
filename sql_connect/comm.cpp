#include "comm.h"


MYSQL* my_connect()
{
	MYSQL *myfd = mysql_init(NULL);
	if(NULL == mysql_real_connect(myfd, "127.0.0.1", "root",\
				"", "ResearchTable", 3306, NULL, 0)){
		std::cout << "connect error" << std::endl;
	}else{
	}

	return myfd;
}

int my_insert(MYSQL *myfd, const std::string &name,\
		const std::string &phone,const std::string &hobby)
{
	std::string sql = "INSERT INTO students (name,phone,hobby) VALUES (\"";
	sql += name;
	sql += "\",\"";
	sql += phone;
	sql += "\",\"";
	sql += hobby;
	sql += "\")";

	std::cout << sql << std::endl;

	return mysql_query(myfd, sql.c_str());
}

int my_select(MYSQL *myfd)
{
	std::string sql = "SELECT * FROM students";
	mysql_query(myfd, sql.c_str());
	MYSQL_RES *result = mysql_store_result(myfd);

	int lines = mysql_num_rows(result);
	int cols = mysql_num_fields(result);

	std::cout <<"<html>"<< std::endl;
	std::cout <<"<head>" << std::endl;
	
	std::cout << "<link rel=\"stylesheet\" href=\"https://cdn.bootcss.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">" << std::endl;

	std::cout <<"</head>" << std::endl;

	std::cout << "<body>" << std::endl;
	std::cout << "<table border=\"1\">" << std::endl;
	MYSQL_FIELD *field = mysql_fetch_fields(result);
	int i = 0;
	std::cout << "<tr>" << std::endl;
	for(; i < cols; i++){
		std::cout << "<th>" << field[i].name <<"</th>" << std::endl;
	}
	std::cout << "</tr>" << std::endl;

	for(i=0; i < lines; i++){
		MYSQL_ROW row = mysql_fetch_row(result);
		int j = 0;
		std::cout << "<tr>";
		for(; j < cols; j++){
			std::cout << "<td>" << row[j] << "</td>";
		}
		std::cout << "</tr>" << std::endl;
	}

	std::cout <<"</table>" << std::endl;
	std::cout << "</body>" << std::endl;
	std::cout <<"</html>"<< std::endl;
	free(result);
}


void my_close(MYSQL *myfd)
{
	mysql_close(myfd);
}





















