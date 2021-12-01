CC = gcc --std=gnu99 -W -g

all: keygen enc_client enc_server
keygen: 
	$(CC) keygen.c -o keygen
enc_client: 
	$(CC) enc_client.c -o enc_client
enc_server: 
	$(CC) enc_server.c -o enc_server


# Helper makefile function
clean:
	rm keygen enc_client enc_server && rm -r *.dSYM
zip:
	zip -cvf lamontap-asn5.tar *.c *.h Makefile