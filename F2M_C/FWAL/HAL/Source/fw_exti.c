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
 
#include "fw_exti.h"
#include "fw_system.h"
#include "fw_gpio.h"
#include "fw_debug.h"
#include "fw_print.h"


#define FW_EXTI_DRV_ID       FW_DRIVER_ID(FW_Device_EXTI, EXTI_DRV_NUM)


void FW_EXTI_DeInit(FW_EXTI_Type *dev)
{
    FW_EXTI_Driver_Type *drv;
    
    FW_ASSERT(dev);
    
    if(dev == NULL)  return;
    
    dev->IH_CB = NULL;
    dev->IH_Pdata = NULL;
    
    dev->Pin = PIN_NULL;
    
    dev->EN = OFF;
    
    dev->List.Next = NULL;
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    
    if(drv->DeInit)  drv->DeInit(dev);
}
/**/

void FW_EXTI_Init(FW_EXTI_Type *dev)
{
    FW_EXTI_Driver_Type *drv;
    FW_EXTI_IRQ_Type *irq;
    FW_SList_Type *list;
    u16 pin;
    
    FW_ASSERT(dev);
    if(dev == NULL)  return;
    
    drv = FW_Device_GetDriver(dev);
    if(drv == NULL)
    {
        /* EXTI只对应一种驱动，在此适配即可，无需在每个EXTI设备定义时指定 */
        drv = FW_Driver_Find("ll->exti");
        FW_Device_SetDriver(dev, drv);
    }
    FW_ASSERT(drv);
    
    if(dev->Bind_Flag == False)
    {
        irq = drv->Attach_IRQ(dev);
        FW_ASSERT(irq);
        if(irq == NULL)  return;
        
        /* 当前设备在次节点位置插入 */
        list = &irq->List;
        FW_SList_Insert(list, &dev->List);
        
        dev->Bind_Flag = True;
    }

    pin = dev->Pin;
    
    if(dev->Trigger == FW_EXTI_Trigger_Falling ||
       dev->Trigger == FW_EXTI_Trigger_LL)
    {
        FW_GPIO_Init(pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
    }
    else if(dev->Trigger == FW_EXTI_Trigger_Rising ||
            dev->Trigger == FW_EXTI_Trigger_HL)
    {
        FW_GPIO_Init(pin, FW_GPIO_Mode_IPD, FW_GPIO_Speed_Low);
    }
    else if(dev->Trigger == FW_EXTI_Trigger_Both)
    {
        FW_GPIO_Init(pin, FW_GPIO_Mode_IPN, FW_GPIO_Speed_Low);
    }
    else
    {
        LOG_E("EXTI触发方式配置异常\r\n");
        return;
    }
    
    FW_ASSERT(drv->Init);
    drv->Init(dev);
    
    FW_EXTI_CTL(dev, dev->EN);
}


void FW_EXTI_CTL(FW_EXTI_Type *dev, u8 state)
{
    FW_EXTI_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->CTL);
    
    drv->CTL(dev, state);
}


void FW_EXTI_BindCB(FW_EXTI_Type *dev, void (*cb)(void *), void *pdata)
{
    FW_Lock();
    dev->IH_CB = cb;
    dev->IH_Pdata = pdata;
    FW_Unlock();
}


void FW_EXTI_IH_ISR(FW_EXTI_IRQ_Type *irq)
{
    FW_EXTI_Type *exti;
    FW_SList_Type *list;
    
    list = irq->List.Next;
    
    while(list)
    {
        exti = (FW_EXTI_Type *)Container_Of(list, FW_EXTI_Type, List);
        if(exti && exti->IH_CB)
        {
            exti->IH_CB(exti->IH_Pdata);
        }
        list = list->Next;
    }
}






//#include "fw_exti.h"
//#include "fw_system.h"
//#include "fw_gpio.h"
//#include "fw_debug.h"
//#include "fw_print.h"


//#define FW_EXTI_DRV_ID       FW_DRIVER_ID(FW_Device_EXTI, EXTI_DRV_NUM)


///**
//@功能：查找与当前设备类型一致的头设备
//@参数：dev，当前设备
//@返回：查找到的设备
//@说明：1、Pin[0:7]相同即属于同一类型
//*/
//void *FW_EXTI_FindHead(u16 pin)
//{
//    FW_Device_Type *dev;
//    FW_List_Type *list, *ltmp;
//    FW_EXTI_Type *exti;
//    
//    FW_Lock();
//    
//    extern FW_List_Type Device_List;
//    FW_LIST_FOREACH(list, ltmp, &Device_List)
//    {
//        dev = Container_Of(list, FW_Device_Type, List);
//        if(dev == NULL)
//        {
//            FW_Unlock();
//            return NULL;
//        }
//        
//        /* 通过驱动查找外部中断设备 */
//        if(dev->DID == FW_EXTI_DRV_ID)
//        {
//            exti = Container_Of(dev, FW_EXTI_Type, Device);
//            if(exti == NULL)
//            {
//                FW_Unlock();
//                return NULL;
//            }
//            
//            /* 相同Pin[0:7]的设备归为一类，ST、GD等Cortex-M核MCU都可以按照此方法
//               进行归类，但无法保证会适用其它类型MCU */
//            if((exti->Pin & PIN_MASK) == (pin & PIN_MASK))
//            {
//                FW_Unlock();
//                return exti;
//            }
//        }
//    }
//    
//    FW_Unlock();
//    
//    return NULL;
//}


//void FW_EXTI_DeInit(FW_EXTI_Type *dev)
//{
//    FW_EXTI_Driver_Type *drv;
//    
//    FW_ASSERT(dev);
//    
//    if(dev == NULL)  return;
//    
//    dev->IH_CB = NULL;
//    dev->IH_Pdata = NULL;
//    
//    dev->Pin = PIN_NULL;
//    
//    dev->EN = OFF;
//    
//    dev->List.Next = NULL;
//    
//    drv = FW_Device_GetDriver(dev);
//    
//    FW_ASSERT(drv);
//    
//    if(drv->DeInit)  drv->DeInit(dev);
//}
///**/

//void FW_EXTI_Init(FW_EXTI_Type *dev)
//{
//    FW_EXTI_Driver_Type *drv;
//    FW_EXTI_Type *head_dev;
//    FW_SList_Type *list;
////    u32 dev_id;
//    u16 pin;
//    
//    FW_ASSERT(dev);
//    
//    if(dev == NULL)  return;
//    
////    dev_id = FW_Device_GetID(dev);
//    
//    head_dev = FW_EXTI_FindHead(dev->Pin);
//    
//    FW_ASSERT(head);
//    
//    if(head_dev == dev)
//    {
//        /* 返回自身，说明当前设备为首节点 */
//        list = &dev->List;
//        list->Next = NULL;
//    }
//    else
//    {
//        /* 非首节点，则在次节点位置插入 */
//        list = &head_dev->List;
//        FW_SList_Insert(list, &dev->List);
//    }
//    
//    drv = FW_Device_GetDriver(dev);
//    
//    FW_ASSERT(drv);
//    FW_ASSERT(drv->Init);
//    
//    pin = dev->Pin;
//    
//    if(dev->Trigger == FW_EXTI_Trigger_Falling ||
//       dev->Trigger == FW_EXTI_Trigger_LL)
//    {
//        FW_GPIO_Init(pin, FW_GPIO_Mode_IPU, FW_GPIO_Speed_Low);
//    }
//    else if(dev->Trigger == FW_EXTI_Trigger_Rising ||
//            dev->Trigger == FW_EXTI_Trigger_HL)
//    {
//        FW_GPIO_Init(pin, FW_GPIO_Mode_IPD, FW_GPIO_Speed_Low);
//    }
//    else if(dev->Trigger == FW_EXTI_Trigger_Both)
//    {
//        FW_GPIO_Init(pin, FW_GPIO_Mode_IPN, FW_GPIO_Speed_Low);
//    }
//    else
//    {
//        LOG_E("EXTI触发方式配置异常\r\n");
//        return;
//    }
//    
//    drv->Init(dev);
//    
//    FW_EXTI_CTL(dev, dev->EN);
//}


//void FW_EXTI_CTL(FW_EXTI_Type *dev, u8 state)
//{
//    FW_EXTI_Driver_Type *drv = FW_Device_GetDriver(dev);
//    
//    FW_ASSERT(drv->CTL);
//    
//    drv->CTL(dev, state);
//}


//void FW_EXTI_BindCB(FW_EXTI_Type *dev, void (*cb)(void *), void *pdata)
//{
//    FW_Lock();
//    dev->IH_CB = cb;
//    dev->IH_Pdata = pdata;
//    FW_Unlock();
//}


//void FW_EXTI_IH_ISR(FW_EXTI_Type *dev)
//{
//    FW_EXTI_Type *exti;
//    FW_SList_Type *list;
//    
//    if(dev == NULL)  return;
//    
//    list = &dev->List;
//    
//    while(list)
//    {
//        exti = (FW_EXTI_Type *)Container_Of(list, FW_EXTI_Type, List);
//        if(exti && exti->IH_CB)
//        {
//            exti->IH_CB(exti->IH_Pdata);
//        }
//        list = list->Next;
//    }
//}




//#include "project_debug.h"
//#if MODULE_TEST && EXTI_TEST
//#include "fw_system.h"
//#include "fw_delay.h"
//#include "fw_led.h"


///*  */
//#define EXTI_PA0_DID       FW_DEVICE_ID(FW_DT_EXTI, EXTI_DRV_NUM, PORTx(PA0), PINx(PA0))

//static FW_EXTI_Type EXTI_PA0;

//static void EXTI_PA0_Config(void *dev)
//{
//    EXTI_PA0.Pin = PA0;
//    EXTI_PA0.Trigger = FW_EXTI_Trigger_Rising;
//    EXTI_PA0.EN = ON;
//    EXTI_PA0.DID_Mask = FW_DID_MASK_SN;
//}
//FW_DEVICE_STATIC_REGIST(EXTI_PA0_DID, &EXTI_PA0, EXTI_PA0_Config, EXTI_PA0);


///*  */
//#define EXTI_PC8_DID       FW_DEVICE_ID(FW_DT_EXTI, EXTI_DRV_NUM, PORTx(PC8), PINx(PC8))

//static FW_EXTI_Type EXTI_PC8;

//static void EXTI_PC8_Config(void *dev)
//{
//    EXTI_PC8.Pin = PC8;
//    EXTI_PC8.Trigger = FW_EXTI_Trigger_Falling;
//    EXTI_PC8.EN = ON;
//    EXTI_PC8.DID_Mask = FW_DID_MASK_SN;
//}
//FW_DEVICE_STATIC_REGIST(EXTI_PC8_DID, &EXTI_PC8, EXTI_PC8_Config, EXTI_PC8);


///*  */
//#define EXTI_PC9_DID       FW_DEVICE_ID(FW_DT_EXTI, EXTI_DRV_NUM, PORTx(PC9), PINx(PC9))

//static FW_EXTI_Type EXTI_PC9;

//static void EXTI_PC9_Config(void *dev)
//{
//    EXTI_PC9.Pin = PC9;
//    EXTI_PC9.Trigger = FW_EXTI_Trigger_Falling;
//    EXTI_PC9.EN = ON;
//    EXTI_PC9.DID_Mask = FW_DID_MASK_SN;
//}
//FW_DEVICE_STATIC_REGIST(EXTI_PC9_DID, &EXTI_PC9, EXTI_PC9_Config, EXTI_PC9);


///*  */
//static void IH_CB0(void *pdata)
//{
//    FW_LED_Type *led = (FW_LED_Type *)pdata;
//    
//    FW_LED_Shine(led, 500, 2);
//}

//static void IH_CB8(void *pdata)
//{
//    FW_LED_Type *led = (FW_LED_Type *)pdata;
//    
//    FW_LED_Shine(led, 500, 2);
//}

//static void IH_CB9(void *pdata)
//{
//    FW_LED_Type *led = (FW_LED_Type *)pdata;
//    
//    FW_LED_Shine(led, 500, 2);
//}


//void Test(void)
//{
//    FW_EXTI_Type *fd;
//    FW_LED_Type led1, led2, led3;
//    u16 VCC_EN = PC13;
//    
//    FW_System_Init();
//    FW_Delay_Init();
//    
//    fd = FW_Device_Find(EXTI_PA0_DID);
//    if(fd == NULL)
//    {
//        while(1);
//    }
//    
//    fd->IH_CB = IH_CB0;
//    fd->IH_Pdata = &led1;
//    
//    FW_EXTI_Init(fd);
//    
//    fd = FW_Device_Find(EXTI_PC8_DID);
//    if(fd == NULL)
//    {
//        while(1);
//    }
//    
//    fd->IH_CB = IH_CB8;
//    fd->IH_Pdata = &led2;
//    
//    FW_EXTI_Init(fd);
//    fd = FW_Device_Find(EXTI_PC9_DID);
//    if(fd == NULL)
//    {
//        while(1);
//    }
//    
//    fd->IH_CB = IH_CB9;
//    fd->IH_Pdata = &led3;
//    
//    FW_EXTI_Init(fd);
//    
//    led1.Level = LEVEL_L;
//    led1.Pin = PD2;
//    FW_LED_Init(&led1);
//    
//    led2.Level = LEVEL_L;
//    led2.Pin = PD3;
//    FW_LED_Init(&led2);
//    
//    led3.Level = LEVEL_H;
//    led3.Pin = PC4;
//    FW_LED_Init(&led3);
//    
//    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
//    FW_GPIO_CLR(VCC_EN);
//    
//    while(1);
//}


//#endif

