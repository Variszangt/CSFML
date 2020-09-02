#ifndef GLOBAL_H
#define GLOBAL_H

#pragma warning(disable: 6387)

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define DEFAULT_BUFFER_SIZE 100

// Prints message to stderr and exits with 1.
void fatal(const char* message);

#endif