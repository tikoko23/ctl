#include "hashmap.h"
#include "array.h"
#include "str.h"

CTL_DECLARE_ARRAY_METHODS_EXT(_THashmapBucket, _THashmapItem, bucket, static);
CTL_DEFINE_ARRAY_METHODS_EXT(_THashmapBucket, _THashmapItem, bucket, NULL, NULL, static);

static _THashmapBucket *getBucket(const THashmap *this, TStringView key) {
    return this->buckets + (tsvHash(key) % this->n_buckets);
}

static _THashmapItem *getItem(const _THashmapBucket *bucket, TStringView key) {
    _THashmapItem *item = NULL;
    for (size_t i = 0; i < bucket->length; ++i) {
        _THashmapItem *entry = bucket->items + i;

        if (tsvEq(tsvNewFromStr(&entry->key), key)) {
            item = entry;
            break;
        }
    }

    return item;
}

static void discardData(const THashmap *this, void *data) {
    if (this->item_destructor) {
        this->item_destructor(data);
    }
}

THashmap tHashmapNew(size_t n_buckets) {
    THashmap this = {
        .n_buckets = n_buckets,
        .item_destructor = NULL,
    };

    this.buckets = calloc(n_buckets, sizeof *this.buckets);
    if (!this.buckets) {
        return (THashmap) {};
    }

    return this;
}

THashmap tHashmapNewCb(size_t n_buckets, TCleanup destructor) {
    THashmap this = tHashmapNew(n_buckets);
    this.item_destructor = destructor;

    return this;
}

void tHashmapSet(THashmap *this, TStringView key, void *data) {
    _THashmapBucket *bucket = getBucket(this, key);
    _THashmapItem *existing = getItem(bucket, key);

    if (existing) {
        discardData(this, existing->data);
        existing->data = data;

        if (!data) {
            tstrFree(&existing->key);
            _THashmapItem *last = bucketBack(bucket);
            *existing = *last;
            bucketPop(bucket);
        }

        return;
    }

    bucketAppend(bucket, (_THashmapItem) {
        .data = data,
        .key = tstrNewFromView(key),
    });
}

void *tHashmapGet(const THashmap *this, TStringView key) {
    _THashmapItem *item = getItem(getBucket(this, key), key);
    if (item) {
        return item->data;
    }

    return item;
}

void tHashmapFree(THashmap *this) {
    for (size_t i = 0; i < this->n_buckets; ++i) {
        _THashmapBucket *bucket = this->buckets + i;

        for (size_t j = 0; j < bucket->length; ++j) {
            _THashmapItem *item = bucket->items + j;

            tstrFree(&item->key);
            discardData(this, item->data);
        }

        bucketFree(bucket);
    }

    free(this->buckets);
}
