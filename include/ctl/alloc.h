#ifndef CTL_ALLOC_H
#define CTL_ALLOC_H

#include <stddef.h>

#include "def.h"

/**
 * \ref "TArena" is a bump / linear allocator.
 * A fixed size buffer is preallocated and parts of it are given to allocation requests.
 * All allocated data is deallocated / invalidated at once.
 * Resizing of allocations is not possible.
 */
typedef struct {
    void *head;       /**< Start of the block */
    void *tail;       /**< Position for the next allocation */
    size_t capacity;  /**< Block size */
    size_t alignment; /**< Alignment requirement, defaults to `8` if set to `0` */
    TCleanup cleanup; /**< Optional custom deallocation function */
} TArena;

/**
 * Allocates a new arena using `malloc`.
 * \param cap  Capacity of the arena in bytes
 * \returns    An arena allocated with `malloc` of capacity `cap`.
 */
TArena tarenaNew(size_t cap);

/**
 * Uses (owns) a preallocated buffer as an arena.
 * \param head    Start of the buffer
 * \param cap     Capacity of the buffer
 * \param cleanup Cleanup function called with `this->head` in \ref "tarenaFree", pass `NULL` to default to libc `free`
 * \returns       An arena using `head` as the underlying buffer
 */
TArena tarenaNewFromBuffer(void *head, size_t cap, TCleanup cleanup);

/**
 * Resets `this->tail`.
 * Allows reuse of the arena without allocation overhead.
 * This operation invalidates all previous pointers as new allocations will overwrite the existing data.
 * \param this
 */
void tarenaReset(TArena *this);

/**
 * Returns `this->tail` and shifts `this->tail` by `n_bytes`.
 * \param this
 * \param n_bytes Size of the allocation in bytes
 * \returns       A pointer to the allocated region, or `NULL` if the allocation would exceed the arena's capacity
 */
void *tarenaAlloc(TArena *this, size_t n_bytes);

/**
 * Moves ownership of `this` to a new arena and puts `this` into an empty state.
 * `this` can safely be passed to \ref "tarenaFree" after calling this function.
 * \param this
 * \returns    The new arena that took ownership
 */
TArena tarenaMove(TArena *this);

/**
 * Deallocates all data that is owned by `this` using `this->cleanup`
 * \param this
 */
void tarenaFree(TArena *this);

typedef void *(*TAlloc)(size_t n_bytes, void *userdata);              /**< Allocates a new region of size `n_bytes` */
typedef void *(*TRealloc)(void *old, size_t n_bytes, void *userdata); /**< Resizes an allocated region to be `n_bytes` */
typedef void  (*TDealloc)(void *ptr, void *userdata);                 /**< Deallocates an allocated region */

/**
 * \ref "TDynamicAllocator" is a type whhich groups different kinds allocation functions.
 */
typedef struct {
    TAlloc alloc;     /**< `malloc` equivalent of \ref "TDynamicAllocator" */
    TRealloc resize;  /**< `realloc` equivelent of \ref "TDynamicAllocator" */
    TDealloc dealloc; /**< `free` equivelent of \ref "TDynamicAllocator" */
    TCleanup cleanup; /**< Optional custom cleanup function */
    void *userdata;   /**< Pointer passed to the cleanup function */
} TDynamicAllocator;

/**
 * Creates a \ref "TDynamicAllocator" object that uses libc allocators (`malloc`, `realloc`, `free`).
 * \returns The created allocator
 */
TDynamicAllocator tdaLibc(void);

/**
 * Cleans up the allocator (not to be confused with \ref "tdaDealloc").
 * \param this
 */
void tdaFree(TDynamicAllocator *this);

/**
 * Allocates `bytes` bytes with the underlying allocator.
 * \param this
 * \param bytes Size of the allocation, in bytes
 * \returns     A pointer to the allocated region
 */
void *tdaAlloc(TDynamicAllocator *this, size_t bytes);

/**
 * Resizes a previously allocated region.
 * This invalidates `old`, even if the region is not moved much like `realloc`.
 *
 * If `this->resize` is `NULL`, this function will naively resize the buffer manually.
 * In this case, \ref "tdaResize" is equivalent to the following:
 * \code{c}
 * void *new_block = tdaAlloc(this, n_bytes);
 * memcpy(new_block, old, n_bytes);
 *
 * tdaDealloc(this, old);
 *
 * return new_block;
 * \endcode
 * \param this
 * \param old     The region to resize
 * \param n_bytes New size
 * \returns       The new region if moved, `old` if not
 */
void *tdaResize(TDynamicAllocator *this, void *old, size_t n_bytes);

/**
 * Deallocates a previously allocated region (not to be confused with \ref "tdaFree").
 * \param this
 * \param ptr  The region to deallocate
 */
void tdaDealloc(TDynamicAllocator *this, void *ptr);

#endif
