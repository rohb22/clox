#include <stdlib.h>

#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    // if newsize of 0 means free the array/values
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    // reallocate the pointer to a space with proper capacity needed
    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}