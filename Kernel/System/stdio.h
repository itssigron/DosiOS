#ifndef STDIO_H
#define STDIO_H

#include "utils.h"

#define PRINTF_STATE_NORMAL 0
#define PRINTF_STATE_LENGTH 1
#define PRINTF_STATE_LENGTH_SHORT 2
#define PRINTF_STATE_LENGTH_LONG 3
#define PRINTF_STATE_SPEC 4

#define PRINTF_LENGTH_DEFAULT 0
#define PRINTF_LENGTH_SHORT_SHORT 1
#define PRINTF_LENGTH_SHORT 2
#define PRINTF_LENGTH_LONG 3
#define PRINTF_LENGTH_LONG_LONG 4

void putchar(char c);
void puts(char* str);
char getchar();
char* getline();

/*
Reference - nanobyte's youtube channel: building an os - 5 - how to implement printf
prints your desired format
*/
void printf(const char* fmt, ...);

#endif