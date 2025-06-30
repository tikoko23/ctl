#include <stdint.h>
#include <stdio.h>

#include "array.h"

typedef struct {
    size_t length, capacity;
    int32_t *items;
} I32Array;

CSTL_DECLARE_ARRAY_METHODS_EXT(I32Array, int32_t, i32);
CSTL_DEFINE_ARRAY_METHODS_EXT(I32Array, int32_t, i32, 0);

int main(void) {
    I32Array arr = i32NewFilled(37, 23);
    i32Append(&arr, 37);

    for (size_t i = 0; i < arr.length; ++i) {
        printf("%zu: %d\n", i, arr.items[i]);
    }
}
