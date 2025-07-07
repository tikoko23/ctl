#ifndef CTL_ARRAY_H
#define CTL_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * Convenience macro to be used when minimal customisation is needed.
 */
#define CTL_DECLARE_ARRAY_METHODS(T, prefix) \
typedef struct { \
    size_t length, capacity; \
    T *items; \
} prefix; \
CTL_DECLARE_ARRAY_METHODS_EXT(prefix, T, prefix) \

/**
 * Extended version of \ref "CTL_DECLARE_ARRAY_METHODS" for more customisation.
 *
 * \param struct_t Type which will serve as the array container
 * \param T        Type which the array will store
 * \param prefix   A prefix to be prepended to all method functions (must be unique unless declared as static)
 * \param ...      Extra and optional declarations specifiers (static, etc.). Same declarations must be passed to \ref "CTL_DEFINE_ARRAY_METHODS_EXT".
 *
 * Refer to the documentation for \ref "CTL_DEFINE_ARRAY_METHODS_EXT" for more information regarding this macro.
 */
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

/**
 * Convenience macro to be used when minimal customisation is needed.
 */
#define CTL_DEFINE_ARRAY_METHODS(T, prefix) \
CTL_DEFINE_ARRAY_METHODS_EXT(prefix, T, prefix, NULL, NULL) \

/**
 * \param struct_t   Type which will serve as the array container
 * \param T          Type which the array will store
 * \param prefix     A prefix to be prepended to all method functions (must be unique unless declared as static)
 * \param destructor A callable which will be invoked for every invalidated object, before invalidation
 * \param duplicator A callable which will be invoked for every object during `$Dup()`
 * \param ...        Extra and optional declarations specifiers (`static`, etc.). Same declarations must be passed to \ref "CTL_DECLARE_ARRAY_METHODS_EXT".
 *
 * Specifiers such as `static` may be used in `...` to annotate all functions defined (except private functions which are always statically linked).
 *
 * Signature for `destructor` (function or function pointer):
 * \code{c}
 * void destructor(T *value);
 * \endcode
 *
 * Signature for `duplicator` (function or function pointer):
 * \code{c}
 * T destructor(const T *value);
 * \endcode
 *
 * `destructor` and / or `duplicator` may be `NULL`. In such case they will not be called.
 * If `duplicator` is `NULL`, a shallow `memcpy` will take place instead.
 *
 * This macro defines the following functions:
 * \code{c}
 * struct_t $New(void);                             // Creates an empty array
 * struct_t $NewWithCap(size_t cap);                // Creates an empty array and allocates enough memory for `cap` elements
 * struct_t $NewFilled(size_t n, T value);          // Creates an array of length `n` with all elements set to `value`
 * void $Free(struct_t *this);                      // Calls `destructor` for each element, deallocates any owned memory and leaves `this` in a valid state
 * void $Reserve(struct_t *this, size_t n);         // Reserves enough memory ahead of time to hold at least `n` elements
 * void $Fill(struct_t *this, size_t n, T value);   // Resizes the array to be of length `n` and sets every element to `value`
 * T *$Append(struct_t *this, T value);             // Appends `value` to the array and returns a reference to it
 * void $Pop(struct_t *this);                       // Calls `destructor` for the last element and removes it from the array
 * T *$Front(const struct_t *this);                 // Gets a reference to the first element
 * T *$Back(const struct_t *this);                  // Gets a reference to the last element
 * struct_t $Move(struct_t *this);                  // Transfers ownership of `this`'s memory and leaves `this` in a valid state
 * struct_t $Dup(struct_t *this);                   // Creates a new array and populates each element with `duplicator`
 * \endcode
 */
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
    \
    *this = prefix ## New(); \
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
