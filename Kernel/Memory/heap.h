#ifndef HEAP_H
#define HEAP_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "utils.h"

// kmalloc
// kcalloc
// krealloc
// calloc
// realloc
//char mem[0x10000]; // Define a global memory buffer of size 4MB
//#define MEM_ADDR_START mem
// #define MEM_SIZE sizeof(mem)
#define MEM_ADDR_START 0x1000
#define MEM_SIZE (1024 * 4096)

// Define a structure for memory block headers
typedef struct Header
{
	struct Header* next; // Pointer to the next header
	struct Header* prev;
	size_t size;     // Size of the memory block
	bool taken;   // Flag indicating if the block is allocated or free
} Header;

#define HEADER_SIZE sizeof(Header)

void init_heap();
Header* allocHeader(void* addr, size_t size);
bool splitHeader(Header* header, size_t size);
Header* takeInitialHeader(size_t size);
Header* findUnusedHeader(size_t size);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void* calloc(size_t num, size_t size);

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void* kcalloc(size_t num, size_t size);

void free(void* addr);

#endif // HEAP_H