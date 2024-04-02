#include "heap.h"

// Initialize the heap by setting up the initial header
void init_heap()
{
	Header* initialHeader = (Header*)MEM_ADDR_START;

	initialHeader->next = NULL;
	initialHeader->prev = NULL;
	initialHeader->size = 0;
	initialHeader->taken = false;
}

// Allocate a new header at a specific address with a given size
Header* allocHeader(void* addr, size_t size)
{
	Header* allocatedHeader;
	size_t memOffset = (char*)addr - MEM_ADDR_START;

	// Check if there's enough space for the new header
	if (MEM_SIZE >= memOffset + HEADER_SIZE + size)
	{
		allocatedHeader = (Header*)addr;
		allocatedHeader->size = size;
		allocatedHeader->taken = true;
		allocatedHeader->next = NULL;
		allocatedHeader->prev = NULL;

		return (Header*)addr;
	}

	return NULL;
}

// Function to split a header into two if it's larger than the requested size
bool splitHeader(Header* header, size_t size)
{
	if (header->size > size + HEADER_SIZE)
	{
		// Split the header
		Header* newHeader = (Header*)((char*)header + HEADER_SIZE + size);
		newHeader->size = header->size - size - HEADER_SIZE;
		newHeader->taken = false;
		newHeader->next = header->next;
		newHeader->prev = header;

		header->size = size;
		if (header->next)
		{
			header->next->prev = newHeader;
		}
		header->next = newHeader;

		return true;
	}

	return false;
}

// Function to take the initial header if it's not already taken
Header* takeInitialHeader(size_t size)
{
	Header* initialHeader = (Header*)MEM_ADDR_START;

	// If the linked list's head is the only node in the list and its unused
	if (!initialHeader->next && !initialHeader->taken)
	{
		// Split the initial header if needed
		if (!splitHeader(initialHeader, size))
		{
			// Set size manually if it couldnt split the header
			initialHeader->size = size;
		}

		// mark block as taken
		initialHeader->taken = true;

		// Reset block's memory
		memset((char*)initialHeader + HEADER_SIZE, 0, size);

		return initialHeader;
	}

	return NULL;
}

// Function to find the first free header that can accommodate the requested size
Header* findUnusedHeader(size_t size)
{
	Header* cur = (Header*)MEM_ADDR_START;
	Header* lastHeader = NULL;

	while (cur)
	{
		if (!cur->taken && cur->size >= size)
		{
			// Split the header if needed
			if (!splitHeader(cur, size))
			{
				// Set size manually if it couldnt split the header
				cur->size = size;
			}

			cur->taken = true;

			// Reset block's memory
			memset((char*)cur + HEADER_SIZE, 0, size);
			return cur;
		}

		lastHeader = cur;
		cur = cur->next;
	}


	// If an unused header was not found, lets try to allocate at the end of all used headers (linked list's tail)
	Header* allocatedHeader = allocHeader((char*)lastHeader + HEADER_SIZE + lastHeader->size, size);

	if (allocatedHeader)
	{
		// Set new tail
		lastHeader->next = allocatedHeader;
		allocatedHeader->prev = lastHeader;

		// Reset block's memory
		memset((char*)allocatedHeader + HEADER_SIZE, 0, size);

		return allocatedHeader;
	}

	// return null if it failed to find an unused block that can store the requested size
	return NULL;
}

// Custom malloc implementation
void* malloc(size_t size)
{
	Header* initialHeader = (Header*)MEM_ADDR_START;
	Header* allocatedHeader = NULL;

	// Do we have enough room?
	if (MEM_SIZE >= size + HEADER_SIZE)
	{
		// Try to take the initial header (linked list's head)
		allocatedHeader = takeInitialHeader(size);

		if (!allocatedHeader)
		{
			// If it couldnt allocate at initial header, lets try to find an unused block
			// which is either the first header to be unused or after the last used header (linked list's tail) -> whichever comes first
			allocatedHeader = findUnusedHeader(size);
		}
	}

	// Return allocated header (may be null if failed)
	return allocatedHeader ? (char*)allocatedHeader + HEADER_SIZE : NULL;
}

// Custom realloc implementation
void* realloc(void* ptr, size_t size)
{
	if (!ptr)
	{
		// If ptr is NULL, realloc behaves like malloc
		return malloc(size);
	}

	if (size == 0)
	{
		// If size is 0, realloc behaves like free
		free(ptr);
		return NULL;
	}

	Header* header = (Header*)((char*)ptr - HEADER_SIZE);
	size_t currentSize = header->size;

	if (size <= currentSize)
	{
		// If the requested size is less than or equal to the current size,
		// no need to reallocate, just return the original pointer
		return ptr;
	}

	// Allocate a new block of memory with the requested size
	void* newPtr = malloc(size);

	if (newPtr)
	{
		// Copy the contents from the old block to the new block
		memcpy(newPtr, ptr, currentSize);

		// Free the old block
		free(ptr);
	}

	return newPtr;
}

// Custom calloc implementation
void* calloc(size_t num, size_t size)
{
	size_t totalSize = num * size;
	void* ptr = malloc(totalSize); // Allocate memory using malloc

	if (ptr)
	{
		// If allocation successful, initialize memory to zero
		memset(ptr, 0, totalSize);
	}

	return ptr;
}

// Custom malloc implementation
void* kmalloc(size_t size)
{
  return malloc(size);
}

// Custom realloc implementation
void* krealloc(void* ptr, size_t size)
{
  return realloc(ptr, size);
}

// Custom calloc implementation
void* kcalloc(size_t num, size_t size)
{
  return calloc(num, size);
}

// Custom free implementation
void free(void* addr)
{
	if (addr)
	{
		Header* header = (Header*)((char*)addr - HEADER_SIZE);

		// reset contents of freed memory
		memset(addr, 0xdd, header->size);
		header->taken = false;

		Header* cur = header;

		// Merge consecutive free blocks
		while (cur->next && !cur->next->taken)
		{
			// Merge current header with the next header
			//std::cout << "\033[1;31mMerged block of size " << cur->size << " to block with size " << cur->next->size << "\033[0m" << std::endl;
			cur->size += HEADER_SIZE + cur->next->size;
			cur->next = cur->next->next;

			// Update next header's previous pointer
			if (cur->next)
			{
				cur->next->prev = cur;
			}

			// reset contents of merged block
			memset((char*)cur + HEADER_SIZE, 0xDD, cur->size);
		}

		// Merge retrograde free blocks
		while (cur->prev && !cur->prev->taken)
		{
			// Merge current header with the next header
			//std::cout << "\033[1;31mMerged block of size " << cur->size << " to block with size " << cur->prev->size << "\033[0m" << std::endl;
			cur->prev->size += HEADER_SIZE + cur->size;
			cur->prev->next = cur->next;

			// Update next header's previous pointer
			if (cur->next)
			{
				cur->next->prev = cur->prev;
			}

			// reset contents of merged block
			cur = cur->prev;
			memset((char*)cur + HEADER_SIZE, 0xDD, cur->size);
		}
	}
}
