#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "def.h"

TArena tarenaNew(size_t cap) {
    void *ptr = malloc(cap);
    if (!ptr) {
        return (TArena) {};
    }

    return (TArena) {
        .tail = ptr,
        .head = ptr,
        .capacity = cap,
        .cleanup = NULL,
    };
}

TArena tarenaNewFromBuffer(void *head, size_t cap, TCleanup cleanup) {
    return (TArena) {
        .head = head,
        .tail = head,
        .capacity = cap,
        .cleanup = cleanup,
    };
}

void tarenaReset(TArena *this) {
    this->tail = this->head;
}

void *tarenaAlloc(TArena *this, size_t n_bytes) {
    size_t used = this->tail - this->head;
    if (used + n_bytes > this->capacity) {
        return NULL;
    }

    void *ret = this->tail;
    this->tail += n_bytes;

    return ret;
}

TArena tarenaMove(TArena *this) {
    TArena ret = *this;
    *this = (TArena) {};
    return ret;
}

void tarenaFree(TArena *this) {
    if (this->cleanup) {
        this->cleanup(this->head);
    } else {
        free(this->head);
    }

    *this = (TArena) {};
}

static void *mallocWrapper(size_t n_bytes, void *userdata) {
    return malloc(n_bytes);
}

static void *reallocWrapper(void *old, size_t n_bytes, void *userdata) {
    return realloc(old, n_bytes);
}

static void freeWrapper(void *ptr, void *userdata) {
    free(ptr);
}

TDynamicAllocator tdaLibc(void) {
    return (TDynamicAllocator) {
        .alloc = mallocWrapper,
        .resize = reallocWrapper,
        .dealloc = freeWrapper,
        .cleanup = NULL,
        .userdata = NULL,
    };
}

void tdaFree(TDynamicAllocator *this) {
    if (this->cleanup) {
        this->cleanup(this->userdata);
    }

    *this = (TDynamicAllocator) {};
}

void *tdaAlloc(TDynamicAllocator *this, size_t bytes) {
    return this->alloc(bytes, this->userdata);
}

void *tdaResize(TDynamicAllocator *this, void *old, size_t n_bytes) {
    if (this->resize) {
        return this->resize(old, n_bytes, this->userdata);
    }

    void *new_block = tdaAlloc(this, n_bytes);
    memcpy(new_block, old, n_bytes);

    tdaDealloc(this, old);

    return new_block;
}

void tdaDealloc(TDynamicAllocator *this, void *ptr) {
    this->dealloc(ptr, this->userdata);
}
