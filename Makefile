CC = cc
CFLAGS = -Wall -Werror -pedantic

install: libAH1.so
	cp ./hash.h /usr/include/AH1.h
	cp ./libAH1.so /usr/lib

libAH1.so: hash.c
	$(CC) -o libAH1.so -shared -fPIC hash.c

clean:
	rm -f libAH1.so
