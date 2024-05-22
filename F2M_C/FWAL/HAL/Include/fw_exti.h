#ifndef _FW_EXTI_H_
#define _FW_EXTI_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


/* 外部中断触发方式 */
typedef enum
{
    FW_EXTI_Trigger_Falling,           //下降沿触发
    FW_EXTI_Trigger_Rising,            //上升沿触发
    FW_EXTI_Trigger_Both,              //边沿触发
    FW_EXTI_Trigger_LL,                //低电平触发
    FW_EXTI_Trigger_HL,                //高电平触发
}FW_EXTI_Trigger_Enum;


typedef struct FW_EXTI
{
    __RO_ FW_Device_Type Device;
    
    __WS_ void (*IH_CB)(void *);
    __WS_ void *IH_Pdata;
    
    __WM_ u32 Pin : 16;                //外部触发IO
    
    __WM_ u32 Trigger : 3;             //触发方式
    __WM_ u32 EN : 1;                  //使能标识
    
    __RO_ u32 Bind_Flag :1;            //绑定标识，True：已绑定到irq，False：未绑定
    
    __RO_ u32 : 11;
    
    __RO_ FW_SList_Type List;          //使用单向链表进行节点管理
}FW_EXTI_Type;


/* 底层中断，在ll_exti根据实际使用的型号定义；用户在应用中定义的exti设备，会自动
   绑定到相应的irq中 */
typedef struct
{
    __RO_ FW_SList_Type List;
}FW_EXTI_IRQ_Type;


typedef struct FW_EXTI_Driver
{
    void (*DeInit)(FW_EXTI_Type *dev);
    void (*Init)(FW_EXTI_Type *dev);
    void (*CTL)(FW_EXTI_Type *dev, u8 state);
    FW_EXTI_IRQ_Type *(*Attach_IRQ)(FW_EXTI_Type *dev);
}FW_EXTI_Driver_Type;


#define FW_EXTI_LIH(pf, irq, args)\
    if(pf(irq, (void *)args) == True)  return;\


void FW_EXTI_DeInit(FW_EXTI_Type *dev);
void FW_EXTI_Init(FW_EXTI_Type *dev);

void FW_EXTI_CTL(FW_EXTI_Type *dev, u8 state);

void FW_EXTI_BindCB(FW_EXTI_Type *dev, void (*cb)(void *), void *pdata);

//void FW_EXTI_IH_ISR(FW_EXTI_Type *dev);
void FW_EXTI_IH_ISR(FW_EXTI_IRQ_Type *irq);


#ifdef __cplusplus
}
#endif

#endif

