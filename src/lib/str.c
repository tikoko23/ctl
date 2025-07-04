#include <stdlib.h>
#include <string.h>

#include "str.h"

TString tstrNew(void) {
    return (TString) {
        .length = 0,
        .capacity = 0,
        .data = NULL,
    };
}

TString tstrNewFromC(const char *cstr) {
    return tstrNewFromBuf((const unsigned char *)cstr, strlen(cstr));
}

TString tstrNewFromBuf(const unsigned char *buf, size_t n) {
    TString str = tstrNew();
    tstrReserve(&str, n + 1);
    if (!str.data) {
        return tstrNew();
    }

    str.length = n;
    memcpy(str.data, buf, n);
    str.data[n] = '\0';

    return str;
}

TString tstrNewFromView(TStringView view) {
    return tstrNewFromBuf((const unsigned char *)view.data, view.length);
}

TString tstrDup(const TString *this) {
    return tstrNewFromBuf((const unsigned char *)this->data, this->length);
}

// StringView constructors
TStringView tsvNew(void) {
    return (TStringView) {
        .length = 0,
        .data = NULL,
    };
}

TStringView tsvNewFromC(const char *cstr) {
    return (TStringView) {
        .length = strlen(cstr),
        .data = cstr,
    };
}

TStringView tsvNewFromBuf(const unsigned char *buf, size_t n) {
    return (TStringView) {
        .length = n,
        .data = (const char *)buf,
    };
}

TStringView tsvNewFromStr(const TString *str) {
    return (TStringView) {
        .length = str->length,
        .data = str->data,
    };
}

TStringView tsvNewFromStrBounds(const TString *str, size_t start, size_t length) {
    return (TStringView) {
        .length = length,
        .data = str->data + start,
    };
}

// Memory operations
TString tstrMove(TString *old) {
    TString ret = *old;
    *old = tstrNew();

    return ret;
}

void tstrReserve(TString *this, size_t min_cap) {
    if (this->capacity >= min_cap) {
        return;
    }

    this->capacity *= 2;
    if (this->capacity < min_cap) {
        this->capacity = min_cap;
    }

    this->data = realloc(this->data, this->capacity);
    if (!this->data) {
        this->length = 0;
        this->capacity = 0;
    }
}

void tstrShrink(TString *this) {
    if (this->capacity == this->length) {
        return;
    }

    this->capacity = this->length;
    this->data = realloc(this->data, this->capacity);
    if (!this->data) {
        this->length = 0;
        this->capacity = 0;
    }
}

void tstrFree(TString *this) {
    free(this->data);
}

// Modification
void tstrAppend(TString *this, char c) {
    tstrReserve(this, this->length + 2); // New char + null terminator

    if (!this->data) {
        return;
    }

    this->data[this->length] = c;
    this->data[this->length + 1] = '\0';
    ++this->length;
}

void tstrPop(TString *this) {
    if (this->length == 0) {
        return;
    }

    --this->length;
}

void tstrCat(TString *this, TStringView that) {
    tstrReserve(this, this->length + that.length + 1); // +1 for the null terminator
    if (!this->data) {
        return;
    }

    memcpy(this->data + this->length, that.data, that.length);
    this->length += that.length;
    this->data[this->length] = '\0';
}

void tstrTrunc(TString *this, size_t new_len_max) {
    if (this->length <= new_len_max) {
        return;
    }

    this->length = new_len_max;
}

// Query
int tstrCmp(const TString *this, const TString *that) {
    return tsvCmp(tsvNewFromStr(this), tsvNewFromStr(that));
}

int tstrCmpC(const TString *this, const char *cstr) {
    return tsvCmp(tsvNewFromStr(this), tsvNewFromC(cstr));
}

int tsvCmp(TStringView this, TStringView that) {
    if (this.length == 0 && that.length == 0) {
        return 0;
    }

    for (size_t i = 0; i < this.length && i < that.length; ++i) {
        char a = this.data[i];
        char b = that.data[i];

        if (a != b) {
            return (unsigned char)a - (unsigned char)b;
        }
    }

    return (int)this.length - (int)that.length;
}

int tsvCmpC(TStringView this, const char *cstr) {
    return tsvCmp(this, tsvNewFromC(cstr));
}

bool tstrEq(const TString *this, const TString *that) {
    return tstrCmp(this, that) == 0;
}

bool tstrEqC(const TString *this, const char *cstr) {
    return tstrCmpC(this, cstr) == 0;
}

bool tsvEq(TStringView this, TStringView that) {
    return tsvCmp(this, that) == 0;
}

bool tsvEqC(TStringView this, const char *cstr) {
    return tsvCmpC(this, cstr) == 0;
}
