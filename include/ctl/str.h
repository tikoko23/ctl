#ifndef CTL_STR_H
#define CTL_STR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Allocates a string from a string literal.
 * This macro will correctly handle string literals with null bytes, unlike \ref "tstrNewFromC".
 * \code{c}
 * TString str1 = tstrFromC("hello\0world"); // "world" will be truncated
 * TString str2 = tstrFromL("hello\0world"); // "world" and '\0' will be included
 *
 * printf("%zu\n", str1.length); // => 5
 * printf("%zu\n", str2.length); // => 11
 *
 * tstrFree(&str1);
 * tstrFree(&str2);
 * \endcode
 *
 * For runtime-generated strings with null bytes, consider using \ref "tstrNewFromBuf".
 *
 * \param literal Must be a string literal or a macro that expands to a string literal
 * \returns       A heap allocated string that holds the full string literal contents
 */
#define tstrNewFromL(literal) tstrNewFromBuf((const unsigned char *)(literal), (sizeof literal) - 1)

/**
 * Constructs a string view from a string literal.
 * This macro will correctly handle string literals with null bytes, unlike \ref "tsvNewFromC".
 * \code{c}
 * TStringView sv1 = tsvFromC("hello\0world"); // "world" will be truncated
 * TStringView sv2 = tsvFromL("hello\0world"); // "world" and '\0' will be included
 *
 * printf("%zu\n", sv1.length); // => 5
 * printf("%zu\n", sv2.length); // => 11
 * \endcode
 *
 * For runtime-generated strings with null bytes, consider using \ref "tsvNewFromBuf".
 *
 * \param literal Must be a string literal or a macro that expands to a string literal
 * \returns       A heap allocated string that holds the full string literal contents
 */
#define tsvNewFromL(literal) tsvNewFromBuf((const unsigned char *)(literal), (sizeof literal) - 1)

/**
 * \ref "TString" is a type which holds a reference to an owned region of memory representing a string.
 * Unless manually modified, the underlying region is guaranteed to be null-terminated.
 */
typedef struct {
    size_t length;   /**< Length of the string excluding the null-terminator */
    size_t capacity; /**< Size of the allocated memory region */
    char *data;      /**< Pointer to the memory region */
} TString;

/**
 * \ref "TStringView" is a type which holds a reference to a **non**-owned region of memory representing a string slice.
 * Unlike \ref "TString", \ref "TStringView" does not guarantee a null-terminator.
 */
typedef struct {
    size_t length;    /**< Length of the view */
    const char *data; /**< Starting point of the view */
} TStringView;

/**
 * Creates an empty string with nothing allocated.
 */
TString tstrNew(void);

/**
 * Creates a \ref "TString" from a C-style null-terminated string.
 * The data in `cstr` is copied and not used after the function returns.
 *
 * This function is equivalent to:
 * \code{c}
 * tstrNewFromBuf((cosnt unsigned char *)cstr, strlen(cstr));
 * \endcode
 */
TString tstrNewFromC(const char *cstr);

/**
 * Creates a \ref "TString" from a buffer with specified length.
 * The data in `buf` is copied and not used after the function returns.
 * A null-terminator is always added to the string after copying `buf`.
 *
 * \param n The size of the buffer in bytes
 */
TString tstrNewFromBuf(const unsigned char *buf, size_t n);

/**
 * Creates a \ref "TString" from a \ref "TStringView".
 * The data in `view` is copied and not used after the function returns.
 *
 * This function is equivalent to:
 * \code{c}
 * tstrNewFromBuf((cosnt unsigned char *)view.data, view.length);
 * \endcode
 */
TString tstrNewFromView(TStringView view);

/**
 * Duplicates the contents of `this` into a new string.
 *
 * This function is equivalent to:
 * \code{c}
 * tstrNewFromBuf((cosnt unsigned char *)this->data, this->length);
 * \endcode
 */
TString tstrDup(const TString *this);


/**
 * Creates an empty string view with nothing referenced.
 */
TStringView tsvNew(void);

/**
 * Wraps a C-style null-terminated string in a \ref "TStringView".
 * The returned string view is valid as long as `cstr` is alive.
 *
 * This function is equivalent to:
 * \code{c}
 * tsvNewFromBuf((const unsigned char *)cstr, strlen(cstr));
 * \endcode
 */
TStringView tsvNewFromC(const char *cstr);

/**
 * Wraps a buffer in a \ref "TStringView".
 * The returned string view is valid as long as `buf` is alive and has a size of at least `n`.
 *
 * This function is equivalent to:
 * \code{c}
 * tsvNewFromBuf((const unsigned char *)buf, n);
 * \endcode
 */
TStringView tsvNewFromBuf(const unsigned char *buf, size_t n);

/**
 * Wraps a \ref "TString" in a \ref "TStringView".
 * The returned string view is valid as long as `str` is alive.
 * Changing `str->length` in any way, direct or indirect, will invalidate the view.
 *
 * This function is equivalent to:
 * \code{c}
 * tsvNewFromBuf((const unsigned char *)str->data, str->length);
 * \endcode
 */
TStringView tsvNewFromStr(const TString *str);

/**
 * Creates a \ref "TStringView" by selecting a portion of a \ref "TString".
 * The returned string view is valid as long as `str` is alive.
 *
 * This function is equivalent to:
 * \code{c}
 * tsvNewFromBuf((const unsigned char *)str->data + start, length);
 * \endcode
 */
TStringView tsvNewFromStrBounds(const TString *str, size_t start, size_t length);

/**
 * Transfers ownership of `old`'s buffer and leaves `old` in a valid state.
 * \returns The new string which owns the memory
 */
TString tstrMove(TString *old);

/**
 * Allocates space in the internal buffer for at least `min_cap` characters ahead of time.
 */
void tstrReserve(TString *this, size_t min_cap);

/**
 * Resizes the internal buffer to be as tiny as possible to hold all characters.
 */
void tstrShrink(TString *this);

/**
 * Deallocates any owned memory, invalidates all pointers and \ref "TStringView" objects referencing
 * this string and leaves `this` in a valid state.
 */
void tstrFree(TString *this);

/**
 * Appends a single character `c` to the end of the string.
 */
void tstrAppend(TString *this, char c);

/**
 * Removes the last character of the string if there is any.
 */
void tstrPop(TString *this);

/**
 * Extends the string with all the characters from `that`.
 */
void tstrCat(TString *this, TStringView that);

/**
 * Keeps only the first `new_len_max` characters in the string.
 */
void tstrTrunc(TString *this, size_t new_len_max);

/**
 * Compares two \ref "TString" objects by lexicographic order.
 * \returns A negative value if `this < that`, `0` if `this == that` and a positive value if `this > that`.
 */
int tstrCmp(const TString *this, const TString *that);

/**
 * Compares a \ref "TString" with a C-style null-terminated string by lexicographic order.
 * \returns A negative value if `this < cstr`, `0` if `this == cstr` and a positive value if `this > cstr`.
 */
int tstrCmpC(const TString *this, const char *cstr);

/**
 * Compares two \ref "TStringView" objects by lexicographic order.
 * \returns A negative value if `this < that`, `0` if `this == that` and a positive value if `this > that`.
 */
int tsvCmp(TStringView this, TStringView that);

/**
 * Compares a \ref "TStringView" with a C-style null-terminated string by lexicographic order.
 * \returns A negative value if `this < cstr`, `0` if `this == cstr` and a positive value if `this > cstr`.
 */
int tsvCmpC(TStringView this, const char *cstr);

/**
 * Determines whether two \ref "TString" objects are identical.
 */
bool tstrEq(const TString *this, const TString *that);

/**
 * Determines whether a \ref "TString" and a C-style null-terminated string are identical.
 */
bool tstrEqC(const TString *this, const char *cstr);

/**
 * Determines whether two \ref "TStringView" objects are identical.
 */
bool tsvEq(TStringView this, TStringView that);

/**
 * Determines whether a \ref "TStringView" and a C-style null-terminated string are identical.
 */
bool tsvEqC(TStringView this, const char *cstr);

/**
 * Computes the FNV-1a hash of `sv`.
 */
uint64_t tsvHash(TStringView sv);

#endif
