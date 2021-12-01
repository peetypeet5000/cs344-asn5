#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *);
void setupAddressStruct(struct sockaddr_in *, int);
int initilize_socket(int);
int accept_connection(int);
void child_process(int);
void process_greeting(int);
void recieve_data(int, char**, char**);
bool is_double_terminated(char*);
char* do_encryption(char*, char*);
int convert_character_value(char);
char revert_character_value(int);
void send_line(int, char*);

int main(int argc, char *argv[]) {
    int status, current_connections = 0;

    // Check usage & args
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s <port>\n", argv[0]);
        exit(0);
    }

    // Initilzie the socket for listening
    int listen_socket = initilize_socket(atoi(argv[1]));

    // Start listening for connetions. Allow up to 5 connections to queue up
    listen(listen_socket, 5);

    // Loop and accept connections, blocking if one is not available until one connects
    while (1) {
        // Accept new connections
        if(current_connections < 5) {
            int connection = accept_connection(listen_socket);

            // Fork a child
            pid_t pid = fork();
            if(pid < 0) {
                error("SERVER: Error forking child process");
            }

            // If this is the parent, increment counter
            if(pid != 0) {
                current_connections++;
            } else {
                // Have child call work function
                child_process(connection);
            }
        }

        // Check if any children have finished processing
        pid_t wait_pid = waitpid(-1, &status, WNOHANG);

        // If they have, decrement counter
        if(wait_pid < 0) {
            error("SERVER: Error waiting on child process");
        } else if (wait_pid != 0) {
            current_connections--;
        }
    }

    // Close the listening socket
    close(listen_socket);

    // Exit parent
    exit(0);
}



/* 
Error function for printing errors
from sys calls. Pass descriptive msg
*/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}



/*
Setup Address Struct. Use command-line arg
port # and localhost hostname
*/
void setupAddressStruct(struct sockaddr_in *address, int portNumber)
{

    // Clear out the address struct
    memset((char *)address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);
    // Allow a client at any address to connect to this server
    address->sin_addr.s_addr = INADDR_ANY;
}



int initilize_socket(int port) {
    struct sockaddr_in serverAddress;

    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("ERROR opening socket");
    }

    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, port);

    // Associate the socket to the port
    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("ERROR on binding");
    }

    return listenSocket;
}



int accept_connection(int listen_socket) {
    struct sockaddr_in client_address;
    socklen_t size_of_client_info = sizeof(client_address);

    // Accept the connection request which creates a connection socket
    int connection_socket = accept(listen_socket, (struct sockaddr *)&client_address, &size_of_client_info);
    if (connection_socket < 0) {
        error("SERVER: ERROR on accept");
    }

    return connection_socket;
}



void child_process(int connection_socket) {
    char* plaintext = NULL;
    char* key = NULL;

    // Receieve and send greeting to client connected
    process_greeting(connection_socket);

    // If all greetings are OK, continue with actual processing
    recieve_data(connection_socket, &plaintext, &key);

    // Encrypt message
    char* result = do_encryption(plaintext, key);

    // Send result back to client
    send_line(connection_socket, result);

    // Free memory
    free(plaintext);
    free(key);
    free(result);

    close(connection_socket);

    // Exit child process
    exit(0);
}



/*
Receives and sends the greeting to the client. Checks both are
of type encryption
*/
void process_greeting(int connection_socket) {
	char buffer[1] = {0};
    char* greeting = "E";

	// Read message from client
	int chars_read = recv(connection_socket, buffer, 1, 0);
	if (chars_read < 0) {
		error("SERVER: ERROR reading greeting from socket");
        exit(2);
	}

	// If the message from the server is incorrect, exit
	if(strncmp("E", buffer, 1) != 0) {
		fprintf(stderr, "SERVER: Client is of wrong type. Killing child process.\n");
		exit(2);
	}

	int chars_written = send(connection_socket, greeting, 1, 0);
	if (chars_written < 0) {
		error("SERVER: ERROR sending greeting to socket");
        exit(2);
        
	}
	if (chars_written < 1) {
		fprintf(stderr, "SERVER: Greeting not sent to server!\n");
        exit(2);
	}
}



/*
Gets both the plaintext and key from the client,
returns in 2D array
*/
void recieve_data(int connection_socket, char** plaintext, char** key) {
    char* result = NULL;
    int count = 0;
    char buffer[1000];

	// Loop until TWO terminating characters are found
    // We need TWO because it will send two strings!
	do {
		// Clear buffer 
        memset(buffer, '\0', 1000);

		// Read from socket
		int chars_read = recv(connection_socket, buffer, 999, 0);
		if (chars_read < 0) {
			error("CLIENT: ERROR reading result from socket");
		}

        // Manually append null for use in strcat
        buffer[999] = '\0';

		// Append buffer to result string
		result = realloc(result, ++count * 1000 + 1);
        strcat(result, buffer);
	} while(is_double_terminated(result) == false);

    // Split string into two substrings
    char* first_dollar = strchr(result, '$');
    *key = malloc(strlen(first_dollar + 1) + 1);
    strncpy(*key, first_dollar + 1, strlen(first_dollar + 1));

    // Copy in first string too
    *(first_dollar + 1) = '\0';
    *plaintext = malloc(strlen(result) + 1);
    strncpy(*plaintext, result, strlen(result));

    free(result);
}


/*
Helper function to check if there are TWO 
$ in a string
*/
bool is_double_terminated(char* result) {
    char* dollar_ptr = NULL;

    // Check for first dollar
    dollar_ptr = strchr(result, '$');

    // Check again for second dollar
    if(dollar_ptr != NULL) {
        dollar_ptr = strchr(dollar_ptr + 1, '$');

        if(dollar_ptr != NULL) {
            return true;
        }
    }

    // If not found, return false
    return false;
}




/*
Takes the plaintext and key and performs the modular addition
encryption
*/
char* do_encryption(char* plaintext, char* key) {
    char* result = malloc(strlen(plaintext) + 1);

    // Encrypt each character in the plaintext file
    for(int i = 0; i < (int)strlen(plaintext) - 1; i++) {
        // Get converted character value
        int current_char_value = convert_character_value(plaintext[i]);
        int current_key_value = convert_character_value(key[i]);

        // Do modular addition and store in result
        int encryption_result = (current_char_value + current_key_value) % 27;

        char val = revert_character_value(encryption_result);
        if(val != 32 && (val < 65 || val > 90)) {
			printf("weird char! char: %c, key: %c, plain %c", val, key[i], plaintext[i]);
		}

        // Convert back to ASCII and store
        result[i] = revert_character_value(encryption_result);
    }

    // Add $ as last character and null termination
    result[strlen(plaintext) - 2] = '$';
    result[strlen(plaintext) - 1] = '\0';

    return result;
}



/*
Convers an ascii character to its value used
in encryption (0-26, where 26 is a space)
*/
int convert_character_value(char character) {
    int char_value;

    if(character == 32) {
        char_value = 26;
    } else {
        char_value = character - 65;
    }

    return char_value;
}



/*
Convers an ascii character to its value used
in encryption (0-26, where 26 is a space)
*/
char revert_character_value(int character) {
    char char_value;

    if(character == 26) {
        char_value = ' ';
    } else {
        char_value = character + 65;
    }

    return char_value;
}



/*
Sends a line of text to the client.
*/
void send_line(int socketFD, char* line) {
	int chars_written = 0;

	// Send all the characters, looping untill all sent
    do {
		 chars_written += send(socketFD, line + chars_written, strlen(line) - chars_written, 0);

		if (chars_written < 0) {
			error("CLIENT: ERROR writing to socket");
		}
	} while(chars_written < (int)strlen(line));
}