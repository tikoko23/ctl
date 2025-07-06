#ifndef CTL_FORMAT_H
#define CTL_FORMAT_H

#include "str.h"

typedef int (*TFmtSpecHandler)(TString *out, va_list args);

void tFmtSetSpec(const char *spec, TFmtSpecHandler handler); // Pass `NULL` for `handler` to remove

TString tFmtC(const char *fmt, ...);
TString tFmtCV(const char *fmt, va_list args);
TString tFmtSv(TStringView fmt, ...);
TString tFmtSvV(TStringView fmt, va_list args);

void tstrCatFmtC(TString *this, const char *fmt, ...);
void tstrCatFmtCV(TString *this, const char *fmt, va_list args);
void tstrCatFmtSv(TString *this, TStringView fmt, ...);
void tstrCatFmtSvV(TString *this, TStringView fmt, va_list args);

int tPrintFmtC(const char *fmt, ...);
int tPrintFmtCV(const char *fmt, va_list args);
int tPrintFmtSv(TStringView fmt, ...);
int tPrintFmtSvV(TStringView fmt, va_list args);

int tErrFmtC(const char *fmt, ...);
int tErrFmtCV(const char *fmt, va_list args);
int tErrFmtSv(TStringView fmt, ...);
int tErrFmtSvV(TStringView fmt, va_list args);

int tFPrintFmtC(FILE *f, const char *fmt, ...);
int tFPrintFmtCV(FILE *f, const char *fmt, va_list args);
int tFPrintFmtSv(FILE *f, TStringView fmt, ...);
int tFPrintFmtSvV(FILE *f, TStringView fmt, va_list args);

#endif
