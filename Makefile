CC = gcc --std=gnu99 -W -g

all: keygen enc_client enc_server dec_client dec_server
keygen: 
	$(CC) keygen.c -o keygen
enc_client: 
	$(CC) enc_client.c -o enc_client
enc_server: 
	$(CC) enc_server.c -o enc_server
dec_server: 
	$(CC) dec_server.c -o dec_server
dec_client: 
	$(CC) dec_client.c -o dec_client


# Helper makefile function
clean:
	rm keygen enc_client enc_server dec_client dec_server && rm -r *.dSYM
zip:
	zip lamontap-homework5.zip *.c Makefile p5testscript plaintext*