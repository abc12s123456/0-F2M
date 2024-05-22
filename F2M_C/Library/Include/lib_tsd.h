#ifndef _LIB_TSD_H_
#define _LIB_TSD_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_type.h"


typedef struct
{
    float Max;
    float Min;
    
    float Range;
    
    u32   Times : 16;
    u32   Increase_Count : 8;
    u32   Reduce_Count : 8;
    
    float Sum;
    float Value;
}TSD_Type;


void TSD_Init(TSD_Type *tsd, float max, float min, float range, u16 times);
float TSD_GetValue(TSD_Type *tsd, float value);
float TSD_GetAverage(TSD_Type *tsd, float value);


#ifdef __cplusplus
}
#endif

#endif

