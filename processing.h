#ifndef PROCESSING_H
#define PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "main.h"
#include "buffers.h"

void* do_input(void*);
void* do_line_seperator(void*);
void* do_plus_sign(void*);
void* do_output(void*);

#endif