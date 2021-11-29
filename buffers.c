#include "buffers.h"

/******************************
 * INITILIZE GLOBAL VARIABLES *
 * ***************************/
char buffer_1[50][1000] = {0};
int count_1 = 0;
int insert_index_1 = 0;
int read_index_1 = 0;
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

char buffer_2[50][1000] = {0};
int count_2 = 0;
int insert_index_2 = 0;
int read_index_2 = 0;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

char buffer_3[50][1000] = {0};
int count_3 = 0;
int insert_index_3 = 0;
int read_index_3 = 0;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

bool done_processing = false;

/**** NOTE: Some of this code is borrowed from the example given in the tips section
 * Located at: https://replit.com/@cs344/65prodconspipelinec ***********************/



/*
 * Puts a line of text into buffer 1,
 * locks and unlocks mutex
 */
void put_buff_1(char* input_buffer){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_1);

    // Place item in buffer and increment counters
    strncpy(buffer_1[insert_index_1++], input_buffer, 1000);
    count_1++;

    // Signal to the consumer that the buffer is no longer empty & unlock mutex
    pthread_cond_signal(&full_1);
    pthread_mutex_unlock(&mutex_1);
}



/*
 * Returns a line of text from buffer 1,
 * locks and unlocks mutex. Must free returned
 * text
 */
char* get_buff_1() {
    // Declare stack space for string and allocate memory
    char* item;
    item = calloc(1000, sizeof(char));

    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_1);

    while (count_1 == 0) {
    // Buffer is empty. Wait for the producer to signal that the buffer has data
        pthread_cond_wait(&full_1, &mutex_1);
    }

    // Copy item locally, increment read index
    strncpy(item, buffer_1[read_index_1++], 1000);

    // Decrement count to allow waiting again
    count_1--;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1);

    // Return the buffer to process
    return item;
}



/*
 * Puts a line of text into buffer 2,
 * locks and unlocks mutex
 */
void put_buff_2(char* input_buffer){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_2);

    // Place item in buffer and increment counters
    strncpy(buffer_2[insert_index_2++], input_buffer, 1000);
    count_2++;

    // Signal to the consumer that the buffer is no longer empty & unlock mutex
    pthread_cond_signal(&full_2);
    pthread_mutex_unlock(&mutex_2);
}



/*
 * Returns a line of text from buffer 2,
 * locks and unlocks mutex. Must free returned
 * text
 */
char* get_buff_2() {
    // Declare stack space for string and allocate memory
    char* item;
    item = calloc(1000, sizeof(char));

    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_2);

    while (count_2 == 0) {
    // Buffer is empty. Wait for the producer to signal that the buffer has data
        pthread_cond_wait(&full_2, &mutex_2);
    }

    // Copy item locally, increment read index
    strncpy(item, buffer_2[read_index_2++], 1000);

    // Decrement count to allow waiting again
    count_2--;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_2);

    // Return the buffer to process
    return item;
}



/*
 * Puts a line of text into buffer 3,
 * locks and unlocks mutex
 */
void put_buff_3(char result_buffer[1000]){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_3);

    // Place item in buffer and increment counters
    strncpy(buffer_3[insert_index_3++], result_buffer, 1000);
    count_3++;

    // Signal to the consumer that the buffer is no longer empty & unlock mutex
    pthread_cond_signal(&full_3);
    pthread_mutex_unlock(&mutex_3);
}



/*
 * Returns a line of text from buffer 1,
 * locks and unlocks mutex. Must free returned
 * text
 */
char* get_buff_3() {
    // Declare stack space for string and allocate memory
    char* item;
    item = calloc(1000, sizeof(char));

    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_3);

    while (count_3 == 0) {
    // Buffer is empty. Wait for the producer to signal that the buffer has data
        pthread_cond_wait(&full_3, &mutex_3);
    }

    // Copy item locally, increment read index
    strncpy(item, buffer_3[read_index_3++], 1000);

    // Decrement count to allow waiting again
    count_3--;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);

    // Return the buffer to process
    return item;
}