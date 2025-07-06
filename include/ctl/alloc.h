#ifndef CTL_ALLOC_H
#define CTL_ALLOC_H

#include <stddef.h>

#include "def.h"

typedef struct {
    void *head;
    void *tail;
    size_t capacity;
} TArena;

TArena tarenaNew(size_t cap);
TArena tarenaNewFromBuffer(void *head, size_t cap);

void tarenaReset(TArena *this);
void *tarenaAlloc(TArena *this, size_t n_bytes);

TArena tarenaMove(TArena *this);
void tarenaFree(TArena *this);

typedef void *(*TAlloc)(size_t n_bytes, void *userdata);
typedef void *(*TRealloc)(void *old, size_t n_bytes, void *userdata);
typedef void  (*TDealloc)(void *ptr, void *userdata);

typedef struct {
    TAlloc alloc;
    TRealloc resize;
    TDealloc dealloc;
    TCleanup cleanup;
    void *userdata;
} TDynamicAllocator;

TDynamicAllocator tdaLibc(void);
void tdaFree(TDynamicAllocator *this);

void *tdaAlloc(TDynamicAllocator *this, size_t bytes);
void *tdaResize(TDynamicAllocator *this, void *old, size_t n_bytes);
void  tdaDealloc(TDynamicAllocator *this, void *ptr);

#endif
