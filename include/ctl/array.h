#ifndef CTL_ARRAY
#define CTL_ARRAY

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define CTL_DECLARE_ARRAY_METHODS(T, prefix) \
typedef struct { \
    size_t length, capacity; \
    T *items; \
} prefix; \
CTL_DECLARE_ARRAY_METHODS_EXT(prefix, T, prefix) \

#define CTL_DECLARE_ARRAY_METHODS_EXT(struct_t, T, prefix, ...) \
__VA_ARGS__ struct_t prefix ## New(void); \
__VA_ARGS__ struct_t prefix ## NewWithCap(size_t cap); \
__VA_ARGS__ struct_t prefix ## NewFilled(size_t n, T value); \
__VA_ARGS__ void prefix ## Free(struct_t *this); \
__VA_ARGS__ void prefix ## Reserve(struct_t *this, size_t n); \
__VA_ARGS__ void prefix ## Fill(struct_t *this, size_t n, T value); \
__VA_ARGS__ T *prefix ## Append(struct_t *this, T value); \
__VA_ARGS__ void prefix ## Pop(struct_t *this); \
__VA_ARGS__ T *prefix ## Front(const struct_t *this); \
__VA_ARGS__ T *prefix ## Back(const struct_t *this); \
__VA_ARGS__ struct_t prefix ## Move(struct_t *this); \
__VA_ARGS__ struct_t prefix ## Dup(const struct_t *this); \

#define CTL_DEFINE_ARRAY_METHODS(T, prefix) \
CTL_DEFINE_ARRAY_METHODS_EXT(prefix, T, prefix, 0) \

#define CTL_DEFINE_ARRAY_METHODS_EXT(struct_t, T, prefix, destructor, duplicator, ...) \
static void __ ## prefix ## gen_warnings(void) { \
    typedef void (*destructor_t)(T *this); \
    typedef T (*dup_t)(const T *this); \
    destructor_t _de = destructor; \
    dup_t _du = duplicator; \
} \
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
__VA_ARGS__ struct_t prefix ## New(void) { \
    struct_t this = { \
        .length = 0, \
        .capacity = 0, \
        .items = NULL, \
    }; \
    \
    return this; \
} \
\
__VA_ARGS__ struct_t prefix ## NewWithCap(size_t cap) { \
    struct_t this = prefix ## New(); \
    prefix ## Reserve(&this, cap); \
    return this; \
} \
\
__VA_ARGS__ struct_t prefix ## NewFilled(size_t n, T value) { \
    struct_t this = prefix ## New(); \
    prefix ## Fill(&this, n, value); \
    return this; \
} \
__VA_ARGS__ void prefix ## Free(struct_t *this) { \
    if (destructor) { \
        for (size_t i = 0; i < this->length; ++i) { \
            ((void (*)(T *))destructor)(this->items + i); \
        } \
    } \
    \
    free(this->items); \
} \
\
__VA_ARGS__ void prefix ## Reserve(struct_t *this, size_t n) { \
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
__VA_ARGS__ void prefix ## Fill(struct_t *this, size_t n, T value) { \
    prefix ## Reserve(this, n); \
    if (!this->items) { \
        return; \
    } \
    this->length = n; \
    for (size_t i = 0; i < n; ++i) { \
        this->items[i] = value; \
    } \
} \
\
__VA_ARGS__ T *prefix ## Append(struct_t *this, T value) { \
    __ ## prefix ## grow(this, this->length); \
    this->items[this->length++] = value; \
    return this->items + this->length - 1; \
} \
\
__VA_ARGS__ void prefix ## Pop(struct_t *this) { \
    --this->length; \
    \
    if (destructor) { \
        ((void (*)(T *))destructor)(this->items + this->length); \
    } \
} \
__VA_ARGS__ T *prefix ## Front(const struct_t *this) { \
    return this->items; \
} \
__VA_ARGS__ T *prefix ## Back(const struct_t *this) { \
    return this->items + this->length - 1; \
} \
__VA_ARGS__ struct_t prefix ## Move(struct_t *this) { \
    struct_t ret = *this; \
    *this = prefix ## New(); \
    return ret; \
} \
\
__VA_ARGS__ struct_t prefix ## Dup(const struct_t *this) { \
    struct_t ret = prefix ## NewWithCap(this->length); \
    ret.length = this->length; \
    if (duplicator) { \
        for (size_t i = 0; i < ret.length; ++i) { \
            ret.items[i] = ((T (*)(const T *))duplicator)(this->items + i); \
        } \
    } else { \
        memcpy(ret.items, this->items, (sizeof *ret.items) * ret.length); \
    } \
    \
    return ret; \
} \

#endif
