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
 * arugments.
 * 2. Prompt the user for input and send that input as a message to the server.
 * 3. Print the message received from the server and exit the program.
 */
int main(int argc, char *argv[]) {	
	// Check usage & args
	if (argc != 4) {
		fprintf(stderr, "USAGE: %s <plaintext> <key> <port>\n", argv[0]);
		exit(0);
	}

	// Create a socket
	int socketFD = initilize_socket(atoi(argv[3]));

	// Send greeting to server
	send_greeting(socketFD);

	// Receieve greeting from server
	receive_greeting(socketFD);

	// If all connecting happened correnctly, continue with main program
	// Get both plaintext and key
	char* plaintext = get_file_line(argv[1]);
	char* key = get_file_line(argv[2]);

	// Check that key is long enough
	if(strlen(plaintext) < strlen(key)) {
		fprintf(stderr, "CLIENT: Key shorter than plaintext. Aborting...");
		exit(2);
	}

	// Send plaintext and key to server
	send_line(socketFD, plaintext);
	send_line(socketFD, key);

	// Receive encrypted message
	receive_result(socketFD);

	// Close the socket
	close(socketFD);

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
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
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
		error("CLIENT: ERROR opening socket");
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
Sends an 'E' to a socket to indicate this is the
encryption client.
*/
void send_greeting(int socketFD) {
	char* greeting = "E";
	int chars_written = send(socketFD, greeting, strlen(greeting), 0);

	if (chars_written < 0) {
		error("CLIENT: ERROR writing to socket");
	}
	if (chars_written < strlen(greeting)) {
		fprintf(stderr, "CLIENT: Greeting not sent to client!\n");
	}
}



/*
Sends an 'E' to a socket to indicate this is the
encryption client.
*/
void receive_greeting(int socketFD) {
	char buffer[2] = {0};

	// Read message from server
	int chars_read = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
	if (chars_read < 0) {
		error("CLIENT: ERROR reading from socket");
	}

	// If the message from the server is incorrect, exit
	if(strncmp("E", buffer, 2) != 0) {
		fprintf(stderr, "CLIENT: Attempted to connect to wrong server type. Aborting.\n");
		exit(2);	
	}
}



/*
Gets a line of text from a file, allocates
buffer, and returns it. Must free later.
*/
char* get_file_line(char* file_name) {
	char* buffer = NULL;
	int size = 0;

	// Open  file
	FILE* file = fopen(file_name, "r");
	if(file == NULL) {
		error("CLIENT: Error opening file");
	}

	// Get a line from the file
	getline(buffer, &size, file);

	// Strip off newline, replace with null
	buffer[size - 1] = '\0';

	return buffer;
}



/*
Sends a line of text to the server. Sends
in chunks of maximum size 1000
*/
void send_line(int socketFD, char* line) {
	int chars_to_send;

	// Send in 1000 character chunks
	for(int i = 0; i < strlen(line); i += 1000) {
		// Determine how many characters to send
		if(strlen(line + i) > 1000) {
			chars_to_send = 1000;
		} else {
			chars_to_send = strlen(line = i);
		}

		int chars_written = send(socketFD, line + i, chars_to_send, 0);

		if (chars_written < 0) {
			error("CLIENT: ERROR writing to socket");
		}
	}

}


/*
Recieve result in 1000 character chuncks. Keep receieving
until a null terminator is spotted.
*/
void receive_result(int socketFD) {
	char buffer[1000];

	// Loop until terminating character is found
	do {
		// Clear buffer 
		memset(buffer, '\0', 1000);

		// Read from socket
		int chars_read = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
		if (chars_read < 0) {
			error("CLIENT: ERROR reading result from socket");
		}

		// Print message to stdout
		printf("%s", buffer);
	} while(strchr(buffer, '\0') == NULL);

	// If the message from the server is incorrect, exit
	printf("%s", buffer);
}