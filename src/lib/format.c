#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "format.h"
#include "hashmap.h"
#include "str.h"

#define fn_cast(T, value) ((T)(uintptr_t)(value))

static THashmap format_specs = {
    .n_buckets = 0,
};

static int fmtC(TString *out, TStringView prec, va_list args) {
    va_list cpy;
    va_copy(cpy, args);

    char prec_str[prec.length + 1];
    memcpy(prec_str, prec.data, prec.length);
    prec_str[prec.length] = '\0';

    int n = vsnprintf(NULL, 0, prec_str, cpy);
    if (n < 0) {
        return n;
    }

    char *buf = malloc(n + 1);
    vsnprintf(buf, n + 1, prec_str, args);

    TStringView sv = tsvNewFromBuf((const unsigned char *)buf, n);
    tstrCat(out, sv);

    free(buf);

    return TFMT_OK;
}

static void ullToStr(TString *out, unsigned long long n) {
    size_t i = 0;
    char buf[64];

    do {
        char c = n % 10 + '0';
        n /= 10;

        buf[i++] = c;
    } while (n != 0);

    for (int32_t j = i - 1; j >= 0; --j) {
        tstrAppend(out, buf[j]);
    }
}

static int fmtU32(TString *out, TStringView prec, va_list args) {
    uint32_t n = va_arg(args, uint32_t);
    ullToStr(out, n);

    return TFMT_OK;
}

static int fmtU64(TString *out, TStringView prec, va_list args) {
    uint64_t n = va_arg(args, uint64_t);
    ullToStr(out, n);

    return TFMT_OK;
}

static int fmtI32(TString *out, TStringView prec, va_list args) {
    int32_t n = va_arg(args, int32_t);

    if (n < 0) {
        n = -n;
        tstrAppend(out, '-');
    }

    ullToStr(out, n);

    return TFMT_OK;
}

static int fmtI64(TString *out, TStringView prec, va_list args) {
    int64_t n = va_arg(args, int64_t);

    if (n < 0) {
        n = -n;
        tstrAppend(out, '-');
    }

    ullToStr(out, n);

    return TFMT_OK;
}

static int fmtS(TString *out, TStringView prec, va_list args) {
    TString *s = va_arg(args, TString *);
    TStringView sv = tsvNewFromStr(s);

    tstrCat(out, sv);

    return TFMT_OK;
}

static int fmtTs(TString *out, TStringView prec, va_list args) {
    TString s = va_arg(args, TString);
    TStringView sv = tsvNewFromStr(&s);

    tstrCat(out, sv);
    tstrFree(&s);

    return TFMT_OK;
}

static int fmtSv(TString *out, TStringView prec, va_list args) {
    TStringView sv = va_arg(args, TStringView);
    tstrCat(out, sv);

    return TFMT_OK;
}

static int fmtCstr(TString *out, TStringView prec, va_list args) {
    const char *cstr = va_arg(args, const char *);
    tstrCat(out, tsvNewFromC(cstr));

    return TFMT_OK;
}

static int fmtChar(TString *out, TStringView prec, va_list args) {
    char c = va_arg(args, int);
    tstrAppend(out, c);

    return TFMT_OK;
}

static int fmtBool(TString *out, TStringView prec, va_list args) {
    bool b = va_arg(args, int);
    TStringView sv = tsvNewFromC(b ? "true" : "false");
    tstrCat(out, sv);

    return TFMT_OK;
}

void tFmtInitialise(void) {
    if (format_specs.n_buckets != 0) {
        return;
    }

    format_specs = tHashmapNew(16);

    tFmtSetSpec("i32",  fmtI32);
    tFmtSetSpec("i64",  fmtI64);
    tFmtSetSpec("u32",  fmtU32);
    tFmtSetSpec("u64",  fmtU64);
    tFmtSetSpec("c",    fmtC);
    tFmtSetSpec("s",    fmtS);
    tFmtSetSpec("ts",   fmtTs);
    tFmtSetSpec("sv",   fmtSv);
    tFmtSetSpec("cstr", fmtCstr);
    tFmtSetSpec("char", fmtChar);
    tFmtSetSpec("bool", fmtBool);
}

void tFmtDeinitialise(void) {
    if (format_specs.n_buckets == 0) {
        return;
    }

    tHashmapFree(&format_specs);
    format_specs = (THashmap) { 0 };
}

void tFmtSetSpec(const char *spec, TFmtSpecHandler handler) {
    tHashmapSet(&format_specs, tsvNewFromC(spec), fn_cast(void *, handler));
}

static int fstreamWriter(TStringView text, void *file) {
    fwrite(text.data, 1, text.length, file);

    if (feof(file)) {
        return EOF;
    } else if (ferror(file)) {
        return errno;
    }

    return TFMT_OK;
}

static int stringWriter(TStringView text, void *str) {
    tstrCat(str, text);
    return TFMT_OK;
}

TString tFmt(TStringView fmt, ...) {
    va_list args;
    va_start(args, fmt);

    TString out = tFmtV(fmt, args);

    va_end(args);
    return out;
}

TString tFmtV(TStringView fmt, va_list args) {
    TString out;
    tFmtWriteV(fmt, args, stringWriter, &out);

    return out;
}

int tstrCatFmt(TString *this, TStringView fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int ret = tstrCatFmtV(this, fmt, args);

    va_end(args);

    return ret;
}

int tstrCatFmtV(TString *this, TStringView fmt, va_list args) {
    return tFmtWriteV(fmt, args, stringWriter, this);
}

int tPrintFmt(TStringView fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int ret = tPrintFmtV(fmt, args);

    va_end(args);

    return ret;
}

int tPrintFmtV(TStringView fmt, va_list args) {
    return tFmtWriteV(fmt, args, fstreamWriter, stdout);
}

int tErrFmt(TStringView fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int ret = tErrFmtV(fmt, args);

    va_end(args);

    return ret;
}

int tErrFmtV(TStringView fmt, va_list args) {
    return tFmtWriteV(fmt, args, fstreamWriter, stderr);
}

static int writeChar(char c, TFmtWriter writer, void *userdata) {
    return writer(
        (TStringView) {
            .data = (char [1]) { c },
            .length = 1
        },
        userdata
    );
}

int tFmtWriteV(TStringView fmt, va_list args, TFmtWriter writer, void *userdata) {
    if (format_specs.n_buckets == 0) {
        return 1;
    }

    int status;
    size_t adjacent_escape_count = 0;
    size_t chunk_start_index = 0;
    size_t spec_start_index = (size_t)-1;
    size_t prec_start_index = (size_t)-1;

    for (size_t i = 0; i < fmt.length; ++i) {
        char c = fmt.data[i];

        if (c == '$'
         && prec_start_index == (size_t)-1
         && adjacent_escape_count % 2 == 0
         && i + 1 != fmt.length
         && fmt.data[i + 1] == '[') {
            // Skip % and [
            spec_start_index = i + 2;

            if (i != 0) {
                TStringView sv = {
                    .length = i - chunk_start_index,
                    .data = fmt.data + chunk_start_index,
                };

                if ((status = writer(sv, userdata)) != TFMT_OK) {
                    return status;
                }
            }
        }

        if (c == '\\') {
            ++adjacent_escape_count;

            TStringView sv = {
                .length = i - chunk_start_index,
                .data = fmt.data + chunk_start_index,
            };

            chunk_start_index = i + 1;

            if ((status = writer(sv, userdata)) != TFMT_OK) {
                return status;
            }

            if (adjacent_escape_count % 2 == 0) {
                if ((status = writeChar('\\', writer, userdata)) != TFMT_OK) {
                    return status;
                }
            }
        } else {
            adjacent_escape_count = 0;
        }

        if (spec_start_index != (size_t)-1) {
            if (prec_start_index == (size_t)-1 && c == '|') {
                prec_start_index = i + 1;
            } else if (c == ']') {
                TStringView spec = {
                    .length = prec_start_index != (size_t)-1
                            ? prec_start_index - spec_start_index - 1
                            : i - spec_start_index,
                    .data = fmt.data + spec_start_index,
                };

                TStringView prec = {
                    .length = prec_start_index != (size_t)-1
                            ? i - prec_start_index
                            : 0,
                    .data = fmt.data + prec_start_index,
                };

                TFmtSpecHandler handler = fn_cast(TFmtSpecHandler, tHashmapGet(&format_specs, spec));
                if (handler) {
                    TString data = tstrNew();
                    if ((status = handler(&data, prec, args)) != TFMT_OK) {
                        tstrFree(&data);
                        return status;
                    }

                    if ((status = writer(tsvNewFromStr(&data), userdata)) != TFMT_OK) {
                        tstrFree(&data);
                        return status;
                    }

                    tstrFree(&data);
                } else {
                    if ((status = writer(tsvNewFromL("???"), userdata)) != TFMT_OK) {
                        return status;
                    }
                }

                chunk_start_index = i + 1;
                spec_start_index = (size_t)-1;
                prec_start_index = (size_t)-1;
            }
        }
    }

    TStringView sv = {
        .length = fmt.length - chunk_start_index,
        .data = fmt.data + chunk_start_index,
    };

    if ((status = writer(sv, userdata)) != TFMT_OK) {
        return status;
    }

    return TFMT_OK;
}
