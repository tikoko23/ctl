#ifndef CTL_FORMAT_H
#define CTL_FORMAT_H

#include "ctl/str.h"

/**
 * Constant which signifies success for \ref "TFmtWriter" and \ref "TFmtSpecHandler" callbacks
 * and most formatting functions.
 */
#define TFMT_OK 0

#define tFmtL(fmt, ...) tFmt(tsvNewFromL(fmt), __VA_ARGS__)
#define tFmtLV(fmt, args) tFmtV(tsvNewFromL(fmt), args)

#define tstrCatFmtL(str_p, fmt, ...) tstrCatFmt(str_p, tsvNewFromL(fmt), __VA_ARGS__)
#define tstrCatFmtLV(str_p, fmt, args) tstrCatFmtV(str_p, tsvNewFromL(fmt), args)

#define tPrintFmtL(fmt, ...) tPrintFmt(tsvNewFromL(fmt), __VA_ARGS__)
#define tPrintFmtLV(fmt, args) tPrintFmtV(tsvNewFromL(fmt), args)

#define tErrFmtL(fmt, ...) tErrFmt(tsvNewFromL(fmt), __VA_ARGS__)
#define tErrFmtLV(fmt, args) tErrFmtV(tsvNewFromL(fmt), args)

/**
 * \ref "TFmtSpecHandler" is a callback type which can be used with
 * \ref "tFmtSetSpec" to override or add new format specifiers.
 *
 * \param out   The string which should be appended to
 * \param prec  The precision part of the specifier in the format string
 * \param args  The arguments associated with this specifiers
 * \returns     \ref "TFMT_OK" on success, another value otherwise
 */
typedef int (*TFmtSpecHandler)(TString *out, TStringView prec, va_list args);

/**
 * \ref "TFmtWriter" is a callback type which allows formatted output to
 * any destination without unnecessary copies.
 *
 * \param text      A chunk of text to be outputted
 * \param userdata  A pointer value passed without modification from \ref "tFmtWriteV"
 * \returns         \ref "TFMT_OK" on success, another value otherwise
 */
typedef int (*TFmtWriter)(TStringView text, void *userdata);

/**
 * Initialises necessary data structures for formatting.
 * All functions will fail until \ref "tFmtInitialise" is called.
 */
void tFmtInitialise(void);

/**
 * Deallocates all resources associated with the formatting module.
 * All functions will fail until \ref "tFmtInitialise" is called again.
 */
void tFmtDeinitialise(void);

/**
 * Replaces an existing specifier, or adds a new one.
 * If `handler` is `NULL`, the formatting module will stop handling
 * `spec` specifiers until a new callback is regsitered. 
 * \param spec      A C-style null-terminated string
 * \param handler   A callback to be called to handle the specifier `spec`.
 */
void tFmtSetSpec(const char *spec, TFmtSpecHandler handler);

/**
 * Writes formatted data to an arbitrary destination.
 * \param fmt       The format string `fmt` can contain format specifiers of the following format:
 *                  `$[<spec>|<prec>]`
 *                  If `<prec>` is not provided, the `|` character may be omitted.
 *                  If a `$` character directly follows a non-escaped `$` character, it is escaped.
 *                  Format specifiers available by default:
 *                  | Specifier | Type              |
 *                  |-----------|-------------------|
 *                  | `i32`     | `int32_t`         |
 *                  | `i64`     | `int64_t`         |
 *                  | `u32`     | `uint32_t`        |
 *                  | `u32`     | `uint32_t`        |
 *                  | `c`       | Any (1)           |
 *                  | `s`       | `TString *`       |
 *                  | `ts`      | `TString` (2)     |
 *                  | `sv`      | `TStringView`     |
 *                  | `cstr`    | `const char *`    |
 *                  | `char`    | `int` (3)         |
 *                  | `bool`    | `_Bool` or `bool` |
 *
 *                  1. The `c` specifier enables the use of `printf` format specifiers.
 *                     In a specifier such as `$[c|%02d]`, the precision part (`%02d`) is
 *                     directly passed to `vsnprintf` as the `fmt` argument. `vsnprintf`
 *                     will consume any amount of arguments as needed. Any error produced by
 *                     `vsnprintf` will be propagated by the format specifier handler.
 *                     Multiple `printf` specifiers may be used at once:
 *                     \code{c}
 *                      tPrintFmtL("$[cstr]: $[c|%ux%u]\n", "Screen resolution", 1920u, 1080u);
 *                      // Is equivalent to:
 *                      printf("%s: %ux%x\n", "Screen resolution", 1920u, 1080u);
 *                     \endcode
 *                  2. The `ts` specifier enables the use of rvalues of type \ref "TString"
 *                     for formatting. The given string is passed to \ref "tstrFree" after it has been used.
 *                     \code{c}
 *                      TString getString(void);
 *
 *                      tPrintFmtL("$[ts]\n", getString());
 *                      // Is equivalent to:
 *                      TString s = getString();
 *                      tPrintFmtL("$[s]\n", &s);
 *                      tstrFree(&s);
 *                     \endcode
 *                  3. The `char` specifier is equivalent to `$[c|%c]`
 * \param userdata  A pointer value which is passed unmodified to `writer`
 * \returns         The first return value not equal to \ref "TFMT_OK" from any of the following:
 *                    - `writer`
 *                    - The format specifier handler
 *
 * \code{c}
 * TString str = tFmtL("$[i32] $[i32|]", 23, -37); // `$[i32]` and `$[i32|]` are identical
 * assert(tstrEqC(&str, "23 -37"));
 * \endcode
 */
int tFmtWriteV(TStringView fmt, va_list args, TFmtWriter writer, void *userdata);

/**
 * Formats data into a string.
 * \returns The data produced by \ref "tFmtWriteV" stored into a string
 */
TString tFmt(TStringView fmt, ...);

/**
 * Formats data into a string.
 * \returns The data produced by \ref "tFmtWriteV" stored into a string
 */
TString tFmtV(TStringView fmt, va_list args);

/**
 * Formats data and directly concatenates the result with `this`, and eliminates unnecessary copies.
 * \returns The status code propagated from \ref "tFmtWriteV"
 */
int tstrCatFmt(TString *this, TStringView fmt, ...);

/**
 * Formats data and directly concatenates the result with `this`, and eliminates unnecessary copies.
 * \returns The status code propagated from \ref "tFmtWriteV"
 */
int tstrCatFmtV(TString *this, TStringView fmt, va_list args);

/**
 * Formats data and directly writes it to the libc `stdout` stream.
 * \returns The status code propagated from \ref "tFmtWriteV"
 */
int tPrintFmt(TStringView fmt, ...);

/**
 * Formats data and directly writes it to the libc `stdout` stream.
 * \returns The status code propagated from \ref "tFmtWriteV"
 */
int tPrintFmtV(TStringView fmt, va_list args);

/**
 * Formats data and directly writes it to the libc `stderr` stream.
 * \returns The status code propagated from \ref "tFmtWriteV"
 */
int tErrFmt(TStringView fmt, ...);

/**
 * Formats data and directly writes it to the libc `stderr` stream.
 * \returns The status code propagated from \ref "tFmtWriteV"
 */
int tErrFmtV(TStringView fmt, va_list args);

#endif
