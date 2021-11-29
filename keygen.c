#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int str_to_int(char*);
void generate_key(int);

/*
Generates a random sequence of characters in range [A-Z, ]
of length given in command line argument
*/
int main(int argc, char *argv[])
{
    // Check if correct # of command line arguments provided
    if(argc != 2) {
        fprintf(stderr, "Usage: keygen <length(int)>");
        return EXIT_FAILURE;
    }

    // Convert arguments and generate key
    int length = str_to_int(argv[1]);
    generate_key(length);

    return EXIT_SUCCESS;

}



/*
Convert string to integer. Also
checks if provided string is correct
*/
int str_to_int(char* argument) {
    char* err_str;
    long value = strtol(argument, &err_str, 10);

    if(err_str[0] != '\0') {
        fprintf(stderr, "Error converting command line argument");

        return 0;
    }

    return (int)value;
}



/*
Generates key of specified length and sends to stdout
*/
void generate_key(int length) {
    // Allocate space for key + newline
    char output[length + 2];

    // Seed rand function
    srand(time(NULL));

    for(int i = 0; i < length; i++) {
        // Get random # from 0 - 26
        int new_char = rand() % 27;

        if(new_char == 26) {
            output[i] = ' ';
        } else {
            output[i] = new_char + 65;
        }
        
    }

    // Append newline and null to end
    output[length] = '\n';
    output[length + 1] = '\0';

    fprintf(stdout, "%s", output);
}

