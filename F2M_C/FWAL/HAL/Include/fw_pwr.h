#ifndef _FW_PWR_H_
#define _FW_PWR_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


#define PWR_DRV_NUM               0


/* 低功耗模式 */
typedef enum
{
    FW_LPM_Normal = 0,            //工作模式
    FW_LPM_Sleep,                 //睡眠模式
    FW_LPM_Stop,                  //停止模式(深度睡眠)
    FW_LPM_Standby,               //待机模式
}FW_LPM_Enum;


typedef enum
{
    FW_LVDT_1V0 = 10,
    FW_LVDT_1V1,
    FW_LVDT_1V2,
    FW_LVDT_1V3,
    FW_LVDT_1V4,
    FW_LVDT_1V5,
    FW_LVDT_1V6,
    FW_LVDT_1V7,
    FW_LVDT_1V8,
    FW_LVDT_1V9,
    FW_LVDT_2V0,
    FW_LVDT_2V1,
    FW_LVDT_2V2,
    FW_LVDT_2V3,
    FW_LVDT_2V4,
    FW_LVDT_2V5,
    FW_LVDT_2V6,
    FW_LVDT_2V7,
    FW_LVDT_2V8,
    FW_LVDT_2V9,
    FW_LVDT_3V0,
    FW_LVDT_3V1,
    FW_LVDT_3V2,
    FW_LVDT_3V3,
    FW_LVDT_3V4,
    FW_LVDT_3V5,
    FW_LVDT_3V6,
    FW_LVDT_3V7,
    FW_LVDT_3V8,
    FW_LVDT_3V9,
    FW_LVDT_4V0,
    FW_LVDT_4V1,
    FW_LVDT_4V2,
    FW_LVDT_4V3,
    FW_LVDT_4V4,
    FW_LVDT_4V5,
}FW_LVDT_Enum;


typedef struct
{
    void (*IH_CB)(void *);
    void *IH_Pdata;
    u32  LVDT : 8;
    u32  : 24;
}FW_PWR_Type;


typedef struct
{
    void (*Init)(void);
    void (*SetLPM)(FW_LPM_Enum lpm);
}FW_PWR_Driver_Type;


void FW_PWR_DeInit(void);
void FW_PWR_Init(void);
void FW_PWR_SetLPM(FW_LPM_Enum lpm);

FW_PWR_Type *FW_PWR_GetDevice(void);

void FW_PWR_SetLVDT(FW_LVDT_Enum lvdt);
void FW_PWR_SetCB(void (*cb)(void *), void *pdata);

void FW_PWR_IH(void);


#ifdef __cplusplus
}
#endif

#endif

