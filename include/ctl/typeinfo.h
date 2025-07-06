#ifndef CTL_TYPEINFO_H
#define CTL_TYPEINFO_H

#include <stddef.h>
#include <stdint.h>

#include "str.h"

typedef struct {
    const char *name;
    uint64_t hash;
    size_t size;
} TTypeInfo;

#define ttypeInfo(T) ((TTypeInfo) { .name = #T, .hash = tsvHash(tsvNewFromL(#T)), .size = (sizeof (T)) })

#endif
