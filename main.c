#include "main.h"

/*
Starts a text processing program with 4 threads.
Program continues until STOP\n is written to stdin.
*/
int main(int argc, char *argv[])
{
    // Declare pthread variables
    pthread_t input_t, line_seperator_t, plus_sign_t, output_t;

    // Create threads
    pthread_create(&input_t, NULL, do_input, NULL);
    pthread_create(&line_seperator_t, NULL, do_line_seperator, NULL);
    pthread_create(&plus_sign_t, NULL, do_plus_sign, NULL);
    pthread_create(&output_t, NULL, do_output, NULL);

    // Close threads when finished
    pthread_join(input_t, NULL);
    pthread_join(line_seperator_t, NULL);
    pthread_join(plus_sign_t, NULL);
    pthread_join(output_t, NULL);

    return EXIT_SUCCESS;

}