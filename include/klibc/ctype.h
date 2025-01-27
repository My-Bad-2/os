#ifndef KLIBC_CTYPE_H
#define KLIBC_CTYPE_H 1

#include <compiler.h>
#include <stdint.h>

__CDECLS_BEGIN

int isalnum(int symbol);
int isalpha(int symbol);
int isascii(int symbol);
int isblank(int symbol);
int iscntrl(int symbol);
int isdigit(int symbol);
int isgraph(int symbol);
int islower(int symbol);
int isprint(int symbol);
int ispunct(int symbol);
int isspace(int symbol);
int isupper(int symbol);
int isxdigit(int symbol);

int tolower(int symbol);
int toupper(int symbol);
int toascii(int symbol);

__CDECLS_END

#endif // KLIBC_CTYPE_H