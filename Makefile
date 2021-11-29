CC = gcc --std=gnu99 -g -W

all: keygen
keygen: 
	$(CC) keygen.c -o keygen


# Helper makefile function
clean:
	rm keygen
zip:
	zip -cvf lamontap-asn5.tar *.c *.h Makefile