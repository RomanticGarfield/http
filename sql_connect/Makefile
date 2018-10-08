ROOT_PATH=$(shell pwd)
INCLUDE=-I$(ROOT_PATH)/../lib/include
LIB=-L$(ROOT_PATH)/../lib/lib -lmysqlclient
cc=g++

.PHONY:all
all:insert_cgi select_cgi

insert_cgi:insert_cgi.cpp comm.cpp 
	$(cc) -o $@ $^ $(INCLUDE) $(LIB)
select_cgi:select_cgi.cpp comm.cpp
	$(cc) -o $@ $^ $(INCLUDE) $(LIB)

.PHONY:clean
clean:
	rm -f insert_cgi select_cgi
