#ifndef _FW_BKP_H_
#define _FW_BKP_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"

    
typedef enum
{
    FW_BKP_State_OK = 0,
    FW_BKP_State_AddrErr,
    FW_BKP_State_DataErr,
    FW_BKP_State_ArgsErr,
}FW_BKP_State_Enum;


typedef struct
{
    void (*Init)(void);
    
    u32  (*Get_Base)(void);
    u32  (*Get_Size)(void);
    
    u32  (*Write)(u32 addr, const void *pdata, u32 num);
    u32  (*Read)(u32 addr, void *pdata, u32 num);
}FW_BKP_Driver_Type;


/**
 * @API
 */
void FW_BKP_Init(void);

u32  FW_BKP_GetBase(void);
u32  FW_BKP_GetSize(void);

u32  FW_BKP_Write(u32 addr, const void *pdata, u32 num);
u32  FW_BKP_Read(u32 addr, void *pdata, u32 num);


#ifdef __cplusplus
}
#endif

#endif

