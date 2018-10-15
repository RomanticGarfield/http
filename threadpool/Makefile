.PHONY: clean

OBJETS = threadpool.o \
		condition.o \
		test.o 
CC=gcc

all : test

test : $(OBJETS)
	$(CC) $^ -o $@ -lpthread -lrt

%.O : %.c
	gcc -c $< -o $@

clean:
	rm -rf $(OBJETS)

