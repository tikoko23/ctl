#ifndef CTL_FORMAT_H
#define CTL_FORMAT_H

#include "str.h"

#define TFMT_OK 0

#define tFmtL(fmt, ...) tFmt(tsvNewFromL(fmt), __VA_ARGS__)
#define tFmtLV(fmt, args) tFmtV(tsvNewFromL(fmt), args)

#define tstrCatFmtL(str_p, fmt, ...) tstrCatFmt(str_p, tsvNewFromL(fmt), __VA_ARGS__)
#define tstrCatFmtLV(str_p, fmt, args) tstrCatFmtV(str_p, tsvNewFromL(fmt), args)

#define tPrintFmtL(fmt, ...) tPrintFmt(tsvNewFromL(fmt), __VA_ARGS__)
#define tPrintFmtLV(fmt, args) tPrintFmtV(tsvNewFromL(fmt), args)

#define tErrFmtL(fmt, ...) tErrFmt(tsvNewFromL(fmt), __VA_ARGS__)
#define tErrFmtLV(fmt, args) tErrFmtV(tsvNewFromL(fmt), args)

typedef int (*TFmtSpecHandler)(TString *out, TStringView prec, va_list args);
typedef int (*TFmtWriter)(TStringView text, void *userdata);

void tFmtInitialise(void);
void tFmtDeinitialise(void);

void tFmtSetSpec(const char *spec, TFmtSpecHandler handler);

int tFmtWriteV(TStringView fmt, va_list args, TFmtWriter writer, void *userdata);

TString tFmt(TStringView fmt, ...);
TString tFmtV(TStringView fmt, va_list args);

int tstrCatFmt(TString *this, TStringView fmt, ...);
int tstrCatFmtV(TString *this, TStringView fmt, va_list args);

int tPrintFmt(TStringView fmt, ...);
int tPrintFmtV(TStringView fmt, va_list args);

int tErrFmt(TStringView fmt, ...);
int tErrFmtV(TStringView fmt, va_list args);

#endif
