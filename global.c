#include "global.h"

double deg(double rad)
{
    return rad * 180 / PI;
}

double rad(double deg)
{
    return deg / (180 / PI);
}

void fatal(const char* message)
{
    static char* prefix = "[ERROR] ";

    size_t len = (strlen(prefix) + strlen(message) + 1);
    char* final_message = (char*)malloc(sizeof(char) * len);

    strcpy_s(final_message, len, prefix);
    strcat_s(final_message, len, message);
    fprintf(stderr, final_message);

    exit(1);
}