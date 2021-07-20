#include "Allocator.h"



void* Allocator::Allocate(size_t size)
{
	void* p = ::operator new(size);
	//void * p = malloc(size); will also work fine

	return p;
}
