#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "Drivers/terminal.h"

void* memset(void* addr, uint8_t c, size_t n);
void  memcpy(void* s1, const void* s2, size_t n);
void* memmove(void*, const void*, size_t);

char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);

size_t strl(const char* str);

bool checkCommand(const char* str1, const char* command);

int strcmp(const char* s1, const char* s2);

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);

char* getInfoAfterCommand(const char* str1, const char* command);
int startsWith(const char* str, const char* prefix);

#endif
