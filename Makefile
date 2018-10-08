cc=gcc
src=httpd.c
bin=httpd
LDFLAGS=-lpthread

.PHONY:all
all: $(bin) cgi

$(bin):$(src)
	$(cc) -o $@ $^ $(LDFLAGS)

cgi:
	cd wwwroot/cgi;make clean;make;cd -
	cd sql_connect/;make clean;make;cd -

.PHONY:clean
clean:
	rm -rf $(bin) output
	cd wwwroot/cgi;make clean;cd -
	cd sql_connect/;make clean;cd -
	

.PHONY:output
output:
	mkdir -p output/wwwroot/cgi
	mkdir -p output/lib
	cp httpd output
	cp wwwroot/*.html output/wwwroot
	cp wwwroot/js output/wwwroot/ -rf
	cp wwwroot/css output/wwwroot/ -rf
	cp wwwroot/images output/wwwroot/ -rf
	cp wwwroot/fonts output/wwwroot/fonts -rf
	cp wwwroot/cgi/math_cgi output/wwwroot/cgi
	cp sql_connect/insert_cgi output/wwwroot/cgi
	cp sql_connect/select_cgi output/wwwroot/cgi
	cp lib/lib/* output/lib
	cp start.sh output
















