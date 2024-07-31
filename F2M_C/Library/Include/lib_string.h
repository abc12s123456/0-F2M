#ifndef _LIB_STRING_H_
#define _LIB_STRING_H_


#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"
#include "libc.h"


char *StrFind(const char *dst, const char *s, u32 num);
s8   StrCompare(const char *src, const char *dst, u32 num);
u32  StrSplit(char *src, const char *delim, const char **pstr);
void Strcpy(char *dst, const char *src, u32 num);

s32  Atoi(const char *pstr, u8 num);
u32  Atoh(const char *pstr, u8 num);


#ifdef __cplusplus
}
#endif


#endif
