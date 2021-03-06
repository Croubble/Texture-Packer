#include <stdlib.h>     /* malloc, free, rand */
#include <iostream>

const unsigned int alignBoundary = 8;

struct Memory
{
	int currentOffset;
	int size;
	void* memoryStart;
};

void* memory_alloc(Memory* memory, int numBytes)
{
	int bytesToAddToMemoryToConserveAlignment = (alignBoundary -  (numBytes % alignBoundary)) % alignBoundary;
	int finalBytes = numBytes + bytesToAddToMemoryToConserveAlignment;

	//check for overflow memory.
	if (finalBytes + memory->currentOffset >= memory->size)
	{
		return nullptr;
	}
	void* result = (void*) (((char*) memory->memoryStart) + memory->currentOffset);
	memory->currentOffset += finalBytes;
	return result;
}
void memory_clear(Memory* memory)
{
	memory->currentOffset = 0;
}
Memory* memory_create(int numBytes)
{
	int size = sizeof(Memory);
	Memory* memory = (Memory*) malloc(size);
	memory->currentOffset = 0;
	memory->size = numBytes;
	memory->memoryStart = malloc(numBytes);
	return memory;
}

void memory_pop_stack(Memory* memory, void* pointer)
{
	char* ptr_core = (char*) memory->memoryStart;
	char* ptr_bigger = (char*) pointer;
	int distance = (int) (ptr_bigger - ptr_core);
	int result = (int)distance;
	memory->currentOffset = distance;
}