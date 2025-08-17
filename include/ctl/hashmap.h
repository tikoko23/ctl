#ifndef CTL_HASHMAP_H
#define CTL_HASHMAP_H

#include "ctl/def.h"
#include "ctl/str.h"

typedef struct {
    void *data;
    TString key;
} _THashmapItem;

typedef struct {
    size_t length, capacity;
    _THashmapItem *items;
} _THashmapBucket;

/**
 * \ref "THashmap" is a hashmap implementation which hashes \ref "TStringView"
 * keys using FNV-1a.
 */
typedef struct {
    size_t n_buckets;           /**< Length of the bucket array */
    _THashmapBucket *buckets;   /**< The bucket array */
    TCleanup item_destructor;   /**< Callback function to be called when a key is overwritten or erased */
} THashmap;

/**
 * Creates a hashmap with `n_buckets` buckets.
 */
THashmap tHashmapNew(size_t n_buckets);

/**
 * Creates a hashmap with `n_buckets` buckets and a destructor.
 * \param destructor A callback to be called when a key's value is overwritten or the key is erased
 */
THashmap tHashmapNewCb(size_t n_buckets, TCleanup destructor);

/**
 * Sets a key's value in the hashmap.
 * If the key doesn't exist, it creates it with the value `data`.
 * If the key exists, the value is replaced with `data`.
 * During replacement, `this->destructor` will be called if it is not `NULL`.
 * If `data` is `NULL`, the key is removed from the hashmap.
 */
void tHashmapSet(THashmap *this, TStringView key, void *data);

/**
 * Returns the value associated with `key`, or `NULL` if it does not exist.
 */
void *tHashmapGet(const THashmap *this, TStringView key);

/**
 * Destructs all values and deallocates all memory associated with `this`.
 * If `this->destructor` is not `NULL`, it is invoked for every value.
 */
void tHashmapFree(THashmap *this);

#endif
