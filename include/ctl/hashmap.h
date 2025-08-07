#ifndef CTL_HASHMAP_H
#define CTL_HASHMAP_H

#include "def.h"
#include "str.h"

typedef struct {
    void *data;
    TString key;
} _THashmapItem;

typedef struct {
    size_t length, capacity;
    _THashmapItem *items;
} _THashmapBucket;

typedef struct {
    size_t n_buckets;
    _THashmapBucket *buckets;
    TCleanup item_destructor;
} THashmap;

THashmap tHashmapNew(size_t n_buckets);
THashmap tHashmapNewCb(size_t n_buckets, TCleanup destructor);

void tHashmapSet(THashmap *this, TStringView key, void *data);
void *tHashmapGet(const THashmap *this, TStringView key);

void tHashmapFree(THashmap *this);

#endif
