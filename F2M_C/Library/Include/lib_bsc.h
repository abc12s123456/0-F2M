#ifndef _LIB_BSC_H_
#define _LIB_BSC_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


u32  Rand(u32 min, u32 max);
u32  Pow(u32 x, u32 y);
s32  Round(double value);

//s32  Atoi(const char *pstr, Base_Enum base, u8 len);
//u8   Itoa(const s32 value, Base_Enum base, char *pstr);


#ifdef __cplusplus
}
#endif

#endif

