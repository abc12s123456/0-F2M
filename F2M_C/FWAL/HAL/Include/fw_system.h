/*
 * F2M
 * Copyright (C) 2024 abc12s123456 382797263@qq.com.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://github.com/abc12s123456/F2M
 *
 */
#ifndef _FW_SYSTEM_H_
#define _FW_SYSTEM_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


typedef enum
{
    FW_RST_Source_POR,            /* 上电复位 */
    FW_RST_Source_PDR,            /* 掉电复位 */
    FW_RST_Source_BOR,            /* 欠压复位 */
    FW_RST_Source_Pin,            /* 引脚复位 */
    FW_RST_Source_LPW,            /* 低功耗复位 */
    FW_RST_Source_IWDG,           /* 独立看门狗复位 */
    FW_RST_Source_WWDG,           /* 窗口看门狗复位 */
    FW_RST_Source_SOFT,           /* 软件复位 */
    FW_RST_Source_Other,          /* 其它复位 */
}FW_RST_Source_Enum;


typedef enum
{
    FW_Vector_Table_RAM,
    FW_Vector_Table_Flash,
}FW_Vector_Table_Enum;


typedef struct
{
    u8  Vector;
    u32 Offset;
}FW_Vector_Table_Type;


typedef enum
{
    FW_Sysclk_High,               /* 高性能 */
    FW_Sysclk_Medium,             /* 均衡 */
    FW_Sysclk_Low,                /* 低功耗 */
}FW_Sysclk_Enum;


typedef enum
{
    FW_System_Cmd_DisableIRQ = 1, /* 失能中断 */
    FW_System_Cmd_EnableIRQ,      /* 使能中断 */
    FW_System_Cmd_GetIRQState,    /* 获取中断状态 */
    
    FW_System_Cmd_SetSysclk,      /* 设置系统时钟 */
    FW_System_Cmd_GetSysclk,      /* 获取系统时钟 */
    
    FW_System_Cmd_SetVectorTable, /* 设置中断向量表 */
    
    FW_System_Cmd_SoftReset,      /* 软件复位 */
    
    FW_System_Cmd_GetRSTSource,   /* 获取复位源 */
    FW_System_Cmd_ClearRSTSource, /* 清除复位源 */
}FW_System_Cmd_Enum;


typedef struct
{
    __WM_ void (*Init)(void);
    
    __WM_ void (*Disable_IRQ)(void);
    __WM_ void (*Enable_IRQ)(void);
    __WM_ u8   (*Get_IRQState)(void);
    
    __WS_ void (*Set_Sysclk)(FW_Sysclk_Enum mode);
    __WM_ u32  (*Get_Sysclk)(void);
    
    __WS_ void (*Set_VectorTable)(FW_Vector_Table_Enum vector, u32 offset);
    
    __WS_ void (*Soft_Reset)(void);
    
    __WS_ FW_RST_Source_Enum (*Get_RSTSource)(void);
    __WS_ void               (*Clear_RSTSource)(void);
}FW_System_Driver_Type;


/**
 * @API
 */
void FW_System_Init(void);

void FW_System_DisableIRQ(void);
void FW_System_EnableIRQ(void);

void FW_System_EnterCritical(void);
void FW_System_ExitCritical(void);

void FW_SDI(void);
void FW_SEI(void);

void FW_Enter_Critical(void);
void FW_Exit_Critical(void);

void FW_Lock(void);
void FW_Unlock(void);

void FW_System_SetSysclk(FW_Sysclk_Enum mode);
u32  FW_System_GetSysclk(void);

void FW_System_SetVectorTable(FW_Vector_Table_Enum vector, u32 offset);

void FW_System_SoftReset(void);

u8   FW_System_GetRSTSource(void);
void FW_System_ClearRSTSource(void);


#ifdef __cplusplus
}
#endif

#endif

