#ifndef CTL_TYPEINFO_H
#define CTL_TYPEINFO_H

#include <stddef.h>
#include <stdint.h>

#include "str.h"

/**
 * \ref "TTypeInfo" is a type which allows primitive runtime reflection.
 * This approach is very limited since the only way to differentiate types
 * with the preprocessor is by using the type name.
 *
 * This causes problems for code such as:
 * \code{c}
 * typedef struct Stuff {
 *     float points[16][2];
 *     unsigned char colors[16][3];
 * } Stuff;
 *
 * TTypeInfo i1 = ttypeInfo(Stuff);
 * TTypeInfo i2 = ttypeInfo(struct Stuff);
 *
 * printf("%s\n", i1.hash == i2.hash ? "true" : "false"); // => false
 * \endcode
 */
typedef struct {
    const char *name; /**< The name of the type */
    uint64_t hash;    /**< The value of `tsvHash(tsvNewFromC(name))` */
    size_t size;      /**< Size of the type in bytes */
} TTypeInfo;

/**
 * Expands into a \ref "TTypeInfo" compound literal whose fields get populated according to `T`.
 * \param T The type to create the info for
 * \returns A compound literal of type \ref "TTypeInfo"
 */
#define ttypeInfo(T) ((TTypeInfo) { .name = #T, .hash = tsvHash(tsvNewFromL(#T)), .size = (sizeof (T)) })

#endif
