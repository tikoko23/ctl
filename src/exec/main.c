#include <stdint.h>
#include <stdio.h>

#include "array.h"

typedef struct {
    size_t length, capacity;
    int32_t *items;
} I32Array;

// Demonstrative copy function 
static int32_t copyI32(const int32_t *this) {
    return *this + 1;
}

CTL_DECLARE_ARRAY_METHODS_EXT(I32Array, int32_t, i32);
CTL_DEFINE_ARRAY_METHODS_EXT(I32Array, int32_t, i32, NULL, copyI32);

int main(void) {
    I32Array arr = i32NewFilled(37, 23);
    i32Append(&arr, 37);

    for (size_t i = 0; i < arr.length; ++i) {
        printf("%zu: %d\n", i, arr.items[i]);
    }

    I32Array dup = i32Dup(&arr);

    for (size_t i = 0; i < dup.length; ++i) {
        printf("%zu: %d\n", i, dup.items[i]);
    }
}
