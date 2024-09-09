#ifndef _LL_SYSTEM_H_
#define _LL_SYSTEM_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


typedef struct
{
    u32 Sysclk;
    u32 AHBClk;
    u32 APBClk;
}LL_Clocks_Type;


extern LL_Clocks_Type LL_Clocks;


#ifdef __cplusplus
}
#endif

#endif

