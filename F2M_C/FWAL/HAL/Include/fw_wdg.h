#ifndef _FW_WDG_H_
#define _FW_WDG_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/* 看门时间单位 */
#define WDGT_UINT_MS         0
#define WDGT_UINT_US         1


typedef struct FW_WDG        FW_WDG_Type;


typedef struct
{
    void (*Init)(FW_WDG_Type *dev, u32 wdg_time);
    void (*FeedDog)(FW_WDG_Type *dev);
    
    u32  (*Get_MaxTime)(FW_WDG_Type *dev);
    u32  (*Get_MinTime)(FW_WDG_Type *dev);
}WDG_Driver_Type;


struct FW_WDG
{
    FW_Device_Type Device;
    
    /* 看门时间 */
    u32 WDG_Time : 16;
    
    /* 最大、最小看门时间 */
    u32 Max_Time : 16;
    u32 Min_Time : 15;
    
    /* 咬狗时间单位 */
    u32 WDGT_Uint : 2;
    
    /* 咬狗标志 */
    u32 Bit_Flag : 1;
    
    /* 任务监控计数，一般MCU的看门时间最大不超过30s */
    u32 Monitor_Tick : 14;
    
    /* 看门狗中断回调函数&入参 */
    void (*IH_CB)(void *);
    void *IH_Pdata;
};


void WDG_Init(FW_WDG_Type *dev, u32 wdg_time);
void WDG_FeedDog(FW_WDG_Type *dev);
void WDG_BitDog(FW_WDG_Type *dev);

void WDG_BindCB(FW_WDG_Type *dev, void (*cb)(void *), void *pdata);



/**
 * @IWDG
 */
extern FW_WDG_Type IWDG;

#if defined(IWDG_RELOAD_EN) && (IWDG_RELOAD_EN)
#define FW_IWDG_Init(...)\
do{\
    if n = VA_NUM(__VA_ARGS__);\
    if(n == 1)  WDG_Init(&IWDG, (u32)VA0(__VA_ARGS__));\
    else if(n == 2)  WDG_Init((FW_WDG_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_IWDG_FeedDog(...)\
do{\
    if n = VA_NUM(__VA_ARGS__);\
    if(n == 0)  WDG_FeedDog(&IWDG);\
    else if(n == 1)  WDG_FeedDog((FW_WDG_Type *)VA0(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_IWDG_BitDog(...)\
do{\
    if n = VA_NUM(__VA_ARGS__);\
    if(n == 0)  WDG_BitDog(&IWDG);\
    else if(n == 1)  WDG_BitDog((FW_WDG_Type *)VA0(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_IWDG_BindCB(...)\
do{\
    if n = VA_NUM(__VA_ARGS__);\
    if(n == 2)  WDG_BindCB(&IWDG, (void *)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__));\
    else if(n == 1)  WDG_BindCB((FW_WDG_Type *)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__), (void *)VA2(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#else

__INLINE_STATIC_ void FW_IWDG_Init(u32 wdg_time)
{
    WDG_Init(&IWDG, wdg_time);
}

__INLINE_STATIC_ void FW_IWDG_FeedDog(void)
{
    WDG_FeedDog(&IWDG);
}

__INLINE_STATIC_ void FW_IWDG_BitDog(void)
{
    WDG_BitDog(&IWDG);
}

#endif /* #if defined(WDG_RELOAD_EN) && (WDG_RELOAD_EN) */


/**
 * @WWDG
 */
extern FW_WDG_Type WWDG;

__INLINE_STATIC_ void FW_WWDG_Init(u32 wdg_time)
{
    WDG_Init(&WWDG, wdg_time);
}

__INLINE_STATIC_ void FW_WWDG_FeedDog(void)
{
    WDG_FeedDog(&WWDG);
}

__INLINE_STATIC_ void FW_WWDG_BitDog(void)
{
    WDG_BitDog(&WWDG);
}

__INLINE_STATIC_ void FW_WWDG_BindCB(void (*cb)(void *), void *pdata)
{
    WDG_BindCB(&WWDG, cb, pdata);
}


#ifdef __cplusplus
}
#endif

#endif

