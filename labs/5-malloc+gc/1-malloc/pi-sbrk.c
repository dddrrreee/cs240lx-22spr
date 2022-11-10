#include "kr-malloc.h"

// implement in libpi.
void *sbrk(long increment) {
	return kmalloc(increment);
}
