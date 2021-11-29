CC = gcc --std=gnu99 -g -W

all: keygen enc_client
keygen: 
	$(CC) keygen.c -o keygen
enc_client: 
	$(CC) enc_client.c -o enc_client


# Helper makefile function
clean:
	rm keygen enc_client
zip:
	zip -cvf lamontap-asn5.tar *.c *.h Makefile