#ifndef CTL_STR
#define CTL_STR

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define tstrNewFromL(literal) tstrNewFromBuf((const unsigned char *)(literal), (sizeof literal) - 1)
#define tsvNewFromL(literal) tsvNewFromBuf((const unsigned char *)(literal), (sizeof literal) - 1)

typedef struct {
    size_t length;
    size_t capacity;
    char *data;
} TString;

typedef struct {
    size_t length;
    const char *data;
} TStringView;

// String constructors
TString tstrNew(void);
TString tstrNewFromC(const char *cstr);
TString tstrNewFromBuf(const unsigned char *buf, size_t n);
TString tstrNewFromView(TStringView view);
TString tstrDup(const TString *this);

// StringView constructors
TStringView tsvNew(void);
TStringView tsvNewFromC(const char *cstr);
TStringView tsvNewFromBuf(const unsigned char *buf, size_t n);
TStringView tsvNewFromStr(const TString *str);
TStringView tsvNewFromStrBounds(const TString *str, size_t start, size_t length);

// Memory operations
TString tstrMove(TString *old);
void tstrReserve(TString *this, size_t min_cap);
void tstrShrink(TString *this);
void tstrFree(TString *this);

// Modification
void tstrAppend(TString *this, char c);
void tstrPop(TString *this);
void tstrCat(TString *this, TStringView that);
void tstrTrunc(TString *this, size_t new_len_max);

// Query
int tstrCmp(const TString *this, const TString *that);
int tstrCmpC(const TString *this, const char *cstr);
int tsvCmp(TStringView this, TStringView that);
int tsvCmpC(TStringView this, const char *cstr);
bool tstrEq(const TString *this, const TString *that);
bool tstrEqC(const TString *this, const char *cstr);
bool tsvEq(TStringView this, TStringView that);
bool tsvEqC(TStringView this, const char *cstr);

#endif
