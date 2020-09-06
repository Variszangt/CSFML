#ifndef GLOBAL_H
#define GLOBAL_H

#pragma warning(disable: 6387)
#pragma warning(disable: 4996)
#pragma warning(disable: 6308)
#pragma warning(disable: 28183)

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define DEFAULT_BUFFER_SIZE 100

#define PI 3.14159265358979323846

double deg(double rad);
double rad(double deg);

// Prints message to stderr and exits with 1.
void fatal(const char* message);

#endif