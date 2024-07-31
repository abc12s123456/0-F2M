#ifndef _FW_LED_H_
#define _FW_LED_H_

#ifdef __cplusplus
extern "{"
#endif


#include "fw_device.h"
#include "stiming.h"


typedef struct
{
    FW_Device_Type Parent;
    
    u32 Pin : 16;                      //LED驱动IO
    
    u32 Level : 1;                     //LED打开电平，LEVEL_L/H
    u32 State : 1;                     //LED工作状态，ON: 亮；OFF: 灭
    
    u32 : 14;
    
    STiming_Type *Timing;
}FW_LED_Type;


void FW_LED_Init(FW_LED_Type *pdev);

void FW_LED_SetPin(FW_LED_Type *pdev, u16 pin);
u16  FW_LED_GetPin(FW_LED_Type *pdev);
void FW_LED_SetLevel(FW_LED_Type *pdev, u8 level);
u8   FW_LED_GetLevel(FW_LED_Type *pdev);

void FW_LED_ON(FW_LED_Type *pdev);
void FW_LED_OFF(FW_LED_Type *pdev);

u8   FW_LED_GetState(FW_LED_Type *pdev);

void FW_LED_Shine(FW_LED_Type *pdev, u16 period, u8 times);
void FW_LED_Blink(FW_LED_Type *pdev, u16 period);
void FW_LED_AdjBlink(FW_LED_Type *pdev, u16 period, u8 dc);

void FW_LED_Breath(FW_LED_Type *pdev, u16 period);
void FW_LED_ExqBreath(FW_LED_Type *pdev, u16 period);


#ifdef __cplusplus
}
#endif

#endif

