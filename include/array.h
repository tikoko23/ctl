#ifndef CTL_ARRAY
#define CTL_ARRAY

#include <stddef.h>
#include <stdlib.h>

#define CTL_DECLARE_ARRAY_METHODS(T, prefix) \
typedef struct { \
    size_t length, capacity; \
    T *items; \
} prefix; \
CTL_DECLARE_ARRAY_METHODS_EXT(prefix, T, prefix) \

#define CTL_DECLARE_ARRAY_METHODS_EXT(struct_t, T, prefix) \
struct_t prefix ## New(); \
struct_t prefix ## NewWithCap(size_t cap); \
struct_t prefix ## NewFilled(size_t n, T value); \
void prefix ## Free(struct_t *this); \
void prefix ## Reserve(struct_t *this, size_t n); \
void prefix ## Fill(struct_t *this, size_t n, T value); \
T *prefix ## Append(struct_t *this, T value); \
void prefix ## Pop(struct_t *this); \
T *prefix ## Front(const struct_t *this); \
T *prefix ## Back(const struct_t *this); \

#define CTL_DEFINE_ARRAY_METHODS(T, prefix) \
CTL_DEFINE_ARRAY_METHODS_EXT(prefix, T, prefix, 0) \

#define CTL_DEFINE_ARRAY_METHODS_EXT(struct_t, T, prefix, destructor) \
static void __ ## prefix ## grow(struct_t *this, size_t n) { \
    if (this->capacity >= n) { \
        return; \
    } \
    \
    this->capacity *= 2; \
    if (this->capacity < n) { \
        this->capacity = n; \
    } \
    \
    prefix ## Reserve(this, this->capacity); \
} \
\
struct_t prefix ## New() { \
    struct_t this = { \
        .length = 0, \
        .capacity = 0, \
        .items = NULL, \
    }; \
    \
    return this; \
} \
\
struct_t prefix ## NewWithCap(size_t cap) { \
    struct_t this = prefix ## New(); \
    prefix ## Reserve(&this, cap); \
    return this; \
} \
\
struct_t prefix ## NewFilled(size_t n, T value) { \
    struct_t this = prefix ## NewWithCap(n); \
    for (size_t i = 0; i < n; ++i) { \
        this.items[i] = value; \
    } \
    \
    this.length = n; \
    return this; \
} \
void prefix ## Free(struct_t *this) { \
    if (destructor) { \
        for (size_t i = 0; i < this->length; ++i) { \
            ((void (*)(T *))destructor)(this->items + i); \
        } \
    } \
    \
    free(this->items); \
} \
\
void prefix ## Reserve(struct_t *this, size_t n) { \
    if (this->capacity >= n) { \
        return; \
    } \
    \
    this->items = realloc(this->items, n * (sizeof *this->items)); \
    if (!this->items) { \
        this->length = 0; \
        this->capacity = 0; \
    } \
} \
\
void prefix ## Fill(struct_t *this, size_t n, T value) { \
    prefix ## Reserve(this, n); \
    for (size_t i = 0; i < n; ++i) { \
        this->items[i] = value; \
    } \
} \
\
T *prefix ## Append(struct_t *this, T value) { \
    __ ## prefix ## grow(this, this->length); \
    this->items[this->length++] = value; \
    return this->items + this->length - 1; \
} \
\
void prefix ## Pop(struct_t *this) { \
    --this->length; \
    \
    if (destructor) { \
        ((void (*)(T *))destructor)(this->items + this->length); \
    } \
} \
T *prefix ## Front(const struct_t *this) { \
    return this->items; \
} \
T *prefix ## Back(const struct_t *this) { \
    return this->items + this->length - 1; \
} \

#endif
