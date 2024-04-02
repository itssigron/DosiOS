#include "utils.h"

void* memset(void* addr, uint8_t c, size_t n) {

  for (uint32_t i = 0; i < n; i++) {
    ((char*)addr)[i] = c;
  }

  return addr;
}

void* memsetw(void* addr, uint16_t val, size_t n) {
  
  for (uint32_t i = 0; i < n / 2; i++) {
    ((uint16_t*)addr)[i] = val;
  }

  return addr;
}

void memcpy(void* s1, const void* s2, size_t n) {
  for (; n > 0; n--) { *(uint8_t*)s1++ = *(uint8_t*)s2++; }
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}

/* Check if a character is in the English alphabet */
bool isalpha(char c) {  
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) ? true : false;
}

/* Check if a character is a uppercase character */
bool isupper(char c) {
  return (c >= 'A' && c <= 'Z') ? true : false;
}

/* Check if a character is a lowercase character */
bool islower(char c) {
  return (c >= 'a' && c <= 'z') ? true : false;
}

/*
  That function will check if have specific 
  string in the first of string like "ls"
  or "cd"

  input: str1<string> - the string that we check
  command<string> - the command we check if have
  output: have that command<bool>
*/
bool checkCommand(const char* str1, const char* command)
{
  return startsWith(str1, command); //fix this later
}

/*
 Get the information after a specified command in a string.
 
 input:
   str1 <string> - the string that contains the command and information
   command <string> - the command to search for in str1
 output:
   A pointer to the start of the information after the command in str1.
   Returns NULL if the command is not found or if memory allocation fails.
*/

// Custom function to check if one string starts with another
int startsWith(const char* str, const char* prefix) {
    while (*prefix) {
        if (*prefix++ != *str++) {
            return 0; // Not a match
        }
    }
    return 1; // Match
}

char* getInfoAfterCommand(const char* str1, const char* command)
{
    // Check for NULL input strings
    if (str1 == NULL || command == NULL) {
        return NULL;
    }

    // Save the size of the command and the length of str1
    size_t size_of_command = strl(command);
    size_t size_of_str1 = strl(str1);

    // Find the position of the command in str1
    const char* commandPosition = str1;

    if (*commandPosition == '\0') {
        // Command not found
        return NULL;
    }

    // Move the pointer to the start of the information after the command
    commandPosition += size_of_command;

    // Calculate the size of the information after the command
    size_t size_of_info_after_command = size_of_str1 - (commandPosition - str1);

    // Allocate memory for the information after the command
    char* info_after_command = kmalloc(size_of_info_after_command + 1);

    if (info_after_command == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    // Copy the information after the command to the allocated buffer
    size_t i;
    for (i = 0; i < size_of_info_after_command; i++) {
        info_after_command[i] = commandPosition[i];
    }

    // Null-terminate the string
    info_after_command[i] = '\0';

    return info_after_command;
}

/* Get the character length of a string */
size_t strl(const char* str) {
  
  if (!str) { return 0; }

  size_t len = 0;
  while (str[len]) { len++; }

  return len;
}

/* Get the digit length of an integer */
size_t intl(int n) {
  
  size_t length = 0;
  while (n > 0) { length++; n /= 10; }

  return length;
}
/*
 This function converts the given size_t value to its string representation
 using the specified base. The result is stored in the provided character buffer.
*/
char* itoa_size_t(size_t value, char* buffer, size_t size, size_t base) 
{
    if (size == 0) {
        // Invalid buffer size
        return NULL;
    }

    size_t i = 0;

    // Special case for 0
    if (value == 0) {
        buffer[i++] = '0';
    } else {
        // Convert the value to a string in reverse order
        while (value > 0 && i < size - 1) {
            size_t digit = value % base;
            buffer[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
            value /= base;
        }
    }

    // Reverse the string
    for (size_t j = 0; j < i / 2; j++) {
        char temp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }

    // Null-terminate the string
    buffer[i] = '\0';

    return buffer;
}
/*
itoa turns an integer into a char array
Input: Integer to convert, base to represent as (e.g 10 for decimal, 16 for hexadecimal..)
*/
char* itoa(int value, size_t base) {

  static char buf[32] = { 0 };

  if (!value) { return "0"; }     // If value is 0, return a 0 string
  if (base > 16) { return NULL; } // Can't convert bases above hexadecimal

  size_t i = 30;

  for (; value && i; i--, value /= base) {
    buf[i] = "0123456789abcdef"[value % base];
  }
  
  return &buf[i+1];
}

/* Copy contents of one string into another */
char* strcpy(char* dest, const char* src) { 
  
  memcpy(dest, src, strl(dest));
  return dest;
}

char* strncpy(char* dest, const char* src, size_t n) 
{
  memcpy(dest, src, n);
  return dest;
}

/* Copy src string to dest string */
char* strcat(char* dest, const char* src) {

  unsigned long i = 0;
  size_t dest_length = strl(dest);
  size_t src_length = strl(src);

  for(; i < dest_length; i++) {  }
  for (; i - dest_length < src_length; i++) { dest[i] = src[i - dest_length]; }  

  return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
  
  unsigned long i = 0;
  size_t dest_length = strl(dest);

  for (; i < dest_length; i++) { }
  for (; i - dest_length < n; i++) { dest[i] = src[i - dest_length]; }
  dest[i] = NULL;
  return dest;
}

/* Make all lower characters in the string uppercase */
char* upper(char* src, size_t n) {
  
  for (unsigned long i = 0; i < n; i++) { 
    if (islower(src[i])) { src[i] += ('A' - 'a'); }
  }

  return src;
}

int strcmp(const char* s1, const char* s2) {
  while(*s1 && (*s1 == *s2)) { s1++; s2++; }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void strrev(char* str) {

  size_t size = strl(str);
  char c = 0;
  
  for (unsigned long i = 0; i < size / 2; i++) {
    c = str[i];
    str[i] = str[size - i - 1];
    str[size - i - 1] = c;
  }
}




