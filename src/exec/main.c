#include <stdint.h>
#include <stdio.h>

#include "array.h"
#include "format.h"
#include "hashmap.h"
#include "str.h"

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

static void eraseValue(void *ptr) {
    printf("erasing: %d\n", *(int *)ptr);
}

int main(void) {
    I32Array arr = i32NewFilled(37, 23);
    i32Append(&arr, 37);

    for (size_t i = 0; i < arr.length; ++i) {
        // printf("%zu: %d\n", i, arr.items[i]);
    }

    I32Array dup = i32Dup(&arr);

    for (size_t i = 0; i < dup.length; ++i) {
        // printf("%zu: %d\n", i, dup.items[i]);
    }

    int32_t arr2[64] = {
        5, 3, 2, 0, 7, 1
    };

    THashmap map = tHashmapNewCb(64, eraseValue);
    tHashmapSet(&map, tsvNewFromL("i1"), arr2);
    tHashmapSet(&map, tsvNewFromL("i2"), arr2 + 8);
    tHashmapSet(&map, tsvNewFromL("i1"), arr2 + 2);

    printf("i1: %d\n", *(int *)tHashmapGet(&map, tsvNewFromL("i1")));
    printf("i2: %d\n", *(int *)tHashmapGet(&map, tsvNewFromL("i2")));

    tHashmapSet(&map, tsvNewFromL("i1"), NULL);

    printf("i1: %p\n", tHashmapGet(&map, tsvNewFromL("i1")));

    tHashmapFree(&map);
    i32Free(&arr);
    i32Free(&dup);

    tFmtInitialise();

    tPrintFmtL("n: 0x$[c|%X] $[c|%p] $[i32] $[bool] $[cstr]\n", 37u, NULL, -1, false, "c-style-string");

    tFmtDeinitialise();
}
