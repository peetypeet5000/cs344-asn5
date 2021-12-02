#include <netdb.h> // gethostbyname()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/socket.h> // send(),recv()
#include <sys/types.h>  // ssize_t
#include <unistd.h>

void setupAddressStruct(struct sockaddr_in*, int);
int initilize_socket(int);
void error(const char*);
void send_greeting(int);
void receive_greeting(int);
char* get_file_line(char*);
void send_line(int, char*);
void receive_result(int);



/**
 * Client code
 * 1. Create a socket and connect to the server specified in the command
 * arugments. Sends and receives a greeting to verify correct server
 * 2. Reads plaintext and Key from file
 * 3. Sends both to server
 * 4. Recieves encrypted message from server and prints to screen
 */
int main(int argc, char *argv[]) {	
	// Check usage & args
	if (argc != 4) {
		fprintf(stderr, "USAGE: %s <ciphertext> <key> <port>\n", argv[0]);
		exit(0);
	}

	// Get both plaintext and key
	char* ciphertext = get_file_line(argv[1]);
	char* key = get_file_line(argv[2]);

	// Check the key is long enough
	if(strlen(ciphertext) > strlen(key)) {
		fprintf(stderr, "CLIENT: Key shorter than plaintext. Aborting...");
		exit(2);
	}

	// Create a socket
	int socketFD = initilize_socket(atoi(argv[3]));

	// Send greeting to server
	send_greeting(socketFD);

	// Receieve greeting from server
	receive_greeting(socketFD);

	// If all connecting happened correnctly, continue with main program
	// Send plaintext and key to server
	send_line(socketFD, ciphertext);
	send_line(socketFD, key);

	// Receive encrypted message
	receive_result(socketFD);

	// Close the socket
	close(socketFD);

	free(ciphertext);
	free(key);

	exit(EXIT_SUCCESS);
}



/* 
Error function for printing errors
from sys calls. Pass descriptive msg
*/
void error(const char *msg) {
	perror(msg);
	exit(2);
}

/*
Setup Address Struct. Use command-line arg
port # and localhost hostname
*/
void setupAddressStruct(struct sockaddr_in *address, int portNumber) {
	// Store the port number, set to internet protocol
	address->sin_family = AF_INET;
	address->sin_port = htons(portNumber);

	// Get the DNS entry for localhost
	struct hostent *hostInfo = gethostbyname("localhost");
	if (hostInfo == NULL) {
		fprintf(stderr, "dec_client error: error, no such host\n");
		exit(0);
	}

	// Copy the first IP address from the DNS entry to sin_addr.s_addr
	memcpy(
		(char *)&address->sin_addr.s_addr,
		hostInfo->h_addr_list[0],
		hostInfo->h_length);
}



int initilize_socket(int port) {
	int socketFD;
	struct sockaddr_in serverAddress = {0};

	// Make socket file descriptor
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) {
		error("dec_client error: error opening socket");
	}

	// Set up the server address struct, set command-line arg port
	setupAddressStruct(&serverAddress, port);

	// Connect to server
	int connect_return = connect(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (connect_return < 0) {
		error("CLIENT: ERROR connecting");
	}

	return socketFD;
}


/*
Sends an 'D' to a socket to indicate this is the
decryption client.
*/
void send_greeting(int socketFD) {
	char* greeting = "D";
	int chars_written = send(socketFD, greeting, 1, 0);

	if (chars_written < 0) {
		error("CLIENT: ERROR writing to socket");
	}
	if (chars_written < 1) {
		fprintf(stderr, "CLIENT: Greeting not sent to client!\n");
	}
}



/*
Receives an 'D' to a socket to indicate this is the
decryption server.
*/
void receive_greeting(int socketFD) {
	char buffer[1] = {0};

	// Read message from server
	int chars_read = recv(socketFD, buffer, 1, 0);
	if (chars_read < 0) {
		error("CLIENT: ERROR reading from socket");
	}

	// If the message from the server is incorrect, exit
	if(strncmp("D", buffer, 1) != 0) {
		fprintf(stderr, "CLIENT: Attempted to connect to wrong server type. Aborting.\n");
		exit(2);	
	}
}



/*
Gets a line of text from a file, allocates
buffer, and returns it. Must free later.
Also checks if illegal characters are inserted
*/
char* get_file_line(char* file_name) {
	char* buffer = NULL;
	size_t size = 0;

	// Open  file
	FILE* file = fopen(file_name, "r");
	if(file == NULL) {
		error("dec_client error: Error opening file");
	}

	// Get a line from the file
	getline(&buffer, &size, file);

	// Check if any unallowed characters exist
	for(int i = 0; i < (int)strlen(buffer) - 1; i++) {
		if(buffer[i] != 32 && (buffer[i] < 65 || buffer[i] > 90)) {
			printf("dec_client error: input contains bad characters");
			exit(2);
		}
	}

	// Strip off newline, replace with $ terminating character
	buffer[strlen(buffer) - 1] = '$';

	return buffer;
}



/*
Sends a line of text to the server. Sends
in chunks of maximum size 1000
*/
void send_line(int socketFD, char* line) {
	int chars_written = 0;

	// Send all the characters, looping until all sent
    do {
		chars_written += send(socketFD, line + chars_written, strlen(line) - chars_written, 0);
		if (chars_written < 0) {
			error("dec_client error: error writing to socket");
		}
	} while(chars_written < (int)strlen(line));
}


/*
Recieve result in 1000 character chuncks. Keep receieving
until a null terminator is spotted.
*/
void receive_result(int socketFD) {
	char* result = NULL;
	int count = 0;
	char buffer[1000];

	// Loop until terminating character is found
	do {
		// Clear buffer 
        memset(buffer, '\0', 1000);

		// Read from socket
		int chars_read = recv(socketFD, buffer, 999, 0);
		if (chars_read < 0) {
			error("CLIENT: ERROR reading result from socket");
		}

		// Manually append null for use in srtcat
        buffer[999] = '\0';

		// Append buffer to result string
		result = realloc(result, ++count * 1000+ 1);
        strcat(result, buffer);
	} while(strchr(result, '$') == NULL);

	// Print message to stdout
	result[strlen(result) - 1] = '\n';	
	printf("%s", result);
}