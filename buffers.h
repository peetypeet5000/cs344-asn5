#ifndef BUFFERS_H
#define BUFFERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

extern char buffer_1[50][1000];
extern int count_1;
extern int insert_index_1;
extern int read_index_1;

extern char buffer_2[50][1000];
extern int count_2;
extern int insert_index_2;
extern int read_index_2;

extern char buffer_3[50][1000];
extern int count_3;
extern int insert_index_3;
extern int read_index_3;

extern bool done_processing;

void put_buff_1(char*);
char* get_buff_1();
void put_buff_2(char*);
char* get_buff_2();
void put_buff_3(char[1000]);
char* get_buff_3();

#endif