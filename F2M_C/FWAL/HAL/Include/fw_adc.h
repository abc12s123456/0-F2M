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
 
#ifndef _FW_ADC_H_
#define _FW_ADC_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
#include "rbuffer.h"


/* ADC驱动编号 */
#define ADC_DRV_NATIVE_NUM             0x00      //原生ADC
#define ADC_DRV_EXT_NUM                0x02      //外部扩展ADC


/* 用户未设置时，ADC数据位宽及缓存大 */
#define ADC_WIDTH_DEFAULT              FW_ADC_Width_16Bits
#define ADC_BUFFER_SIZE                10


/* ADC通道 */
typedef enum
{
    FW_ADC_CH0  = 0,
    FW_ADC_CH1  = 1,
    FW_ADC_CH2  = 2,
    FW_ADC_CH3  = 3,
    FW_ADC_CH4  = 4,
    FW_ADC_CH5  = 5,
    FW_ADC_CH6  = 6,
    FW_ADC_CH7  = 7,
    FW_ADC_CH8  = 8,
    FW_ADC_CH9  = 9,
    FW_ADC_CH10 = 10,
    FW_ADC_CH11 = 11,
    FW_ADC_CH12 = 12,
    FW_ADC_CH13 = 13,
    FW_ADC_CH14 = 14,
    FW_ADC_CH15 = 15,
    FW_ADC_CH16 = 16,
    FW_ADC_CH17 = 17,
    FW_ADC_CH18 = 18,
    FW_ADC_CH19 = 19,
    FW_ADC_CH20 = 20,
    FW_ADC_CH21 = 21,
    FW_ADC_CH22 = 22,
    FW_ADC_CH23 = 23,
    FW_ADC_CH24 = 24,
    FW_ADC_CH25 = 25,
    FW_ADC_CH26 = 26,
    FW_ADC_CH27 = 27,
    FW_ADC_CH28 = 28,
    FW_ADC_CH29 = 29,
    FW_ADC_CH30 = 30,
    FW_ADC_CH31 = 31,
    
    /* ADC自带的基准、温度、Vbat采样通道，需要在底层映射到具体的硬件通道上 */
//    FW_ADC_CH_Vref,
//    FW_ADC_CH_Temprature,
//    FW_ADC_CH_Vbat,
}FW_ADC_CH_Enum;

/* ADC采样精度 */
typedef enum
{
    FW_ADC_Resolution_6Bits = 6,
    FW_ADC_Resolution_7Bits,
    FW_ADC_Resolution_8Bits,
	FW_ADC_Resolution_9Bits,
	FW_ADC_Resolution_10Bits,
	FW_ADC_Resolution_11Bits,
	FW_ADC_Resolution_12Bits,
	FW_ADC_Resolution_13Bits,
	FW_ADC_Resolution_14Bits,
	FW_ADC_Resolution_16Bits,
	FW_ADC_Resolution_17Bits,
	FW_ADC_Resolution_18Bits,
	FW_ADC_Resolution_19Bits,
	FW_ADC_Resolution_20Bits,
	FW_ADC_Resolution_21Bits,
	FW_ADC_Resolution_22Bits,
	FW_ADC_Resolution_23Bits,
	FW_ADC_Resolution_24Bits,
}FW_ADC_Resolution_Enum;

/* ADC采样速度 */
typedef enum
{
    FW_ADC_Speed_Low    = 0,           //低速   
    FW_ADC_Speed_Medium = 1,           //中速
    FW_ADC_Speed_High   = 2,           //高速
    FW_ADC_Speed_Ultra  = 3,           //超高速
}FW_ADC_Speed_Enum;

/* ADC采样模式 */
typedef enum
{
    FW_ADC_Mode_POL = TRM_POL,
    FW_ADC_Mode_INT = TRM_INT,
    FW_ADC_Mode_DMA = TRM_DMA,
}FW_ADC_Mode_Enum;

/* ADC触发模式 */
typedef enum
{
    FW_ADC_Trigger_Soft = 0,           //软件触发
    FW_ADC_Trigger_Period,             //周期触发
    FW_ADC_Trigger_EXTI,               //外部触发(Timer触发实际也是一种周期触发)
}FW_ADC_Trigger_Enum;

/* ADC输入模式 */
typedef enum
{
    FW_ADC_Input_Single = 0,           //单端输入
    FW_ADC_Input_Differential,         //差分输入
}FW_ADC_Input_Enum;

/* ADC电压基准模式 */
typedef enum
{
    FW_ADC_Vref_IN,                    //内部基准
    FW_ADC_Vref_EX,                    //外部基准
}FW_ADC_Vref_Enum;

/* ADC转换后的数据宽度 */
typedef enum
{
    FW_ADC_Width_8Bits = 1,
    FW_ADC_Width_16Bits = 2,
    FW_ADC_Width_32Bits = 4,
}FW_ADC_DataWidth_Enum;


typedef struct
{
    u32  TSD_H;                        //高阈值
    u32  TSD_L;                        //低阈值
    
    u32  Channel : 8;                  //看门狗关联的通道
    u32  : 24;
    
    void (*IH_CB)(void *);             //看门狗中断服务函数
    void *IH_Pdata;
}FW_ADC_WDG_Type;


typedef struct FW_ADC_Bus         FW_ADC_Bus_Type;
typedef struct FW_ADC_Device      FW_ADC_Device_Type;


typedef struct FW_ADC_Bus
{
    __RO_ FW_Device_Type Device;
    
    __RO_ FW_SList_Type List;                //绑定在总线上的ADC设备(FW_ADC_Device)的管理链表
    
    __RO_ FW_SList_Type *NList;              //使用中断方式采样时，保存下一个通道设备的管理链表
                                             //起始值为Bus->List.Next
    u32 Width : 3;                           //数据宽度，也是DMA传输数据宽度
    u32 Mode : 2;                            //工作模式(DMA/中断/轮询)
    
    u32  Trigger : 2;                        //触发方式(软件触发/外部触发)
    
    u32  Resolution : 5;                     //采样精度，参考FW_ADC_Resolution_Enum
    u32  Total_ConvTime : 20;                //批量转换时间(所有绑定通道的转换时间之和)，单位：ns
    
    __RO_ void (*IH_CB)(struct FW_ADC_Bus *bus);  //转换完成后的中断服务函数
    
    __RO_ void *Buffer;                      //采样数据缓存
    __RO_ u32  Buffer_Num : 8;               //缓存的数量(数组的下标定义值, 0~32)
    __RO_ u32  Channel_Num : 8;              //采样通道数量 CH_Num <= Buffer_Num
    
    u32  Vref : 16;                          //ADC基准电压，扩大10000倍; 可以关联外部基准或使用内部基准
    u32  Clock;                              //ADC工作时钟
    
    FW_ADC_WDG_Type WDG;                     //ADC看门狗
    
    __RO_ void *ADCx;                        //底层使用的端口
}FW_ADC_Bus_Type;


typedef struct FW_ADC_Device
{
    __RO_ FW_Device_Type Device;             //总线为通道的父设备
    
    __RO_ FW_SList_Type List;
    
    u32  P_Pin : 16;                         //采样正输入IO
    u32  N_Pin : 16;                         //采样负输入IO，单独设置无效
    
    u32  Sample_Period : 16;                 //采样周期，单位：us; 0表示以批量转换时间为周期
    u32  Sample_Count : 16;                  //采样计数
    
    u32  Channel : 8;                        //采样通道

    u32  Input_Mode : 1;                     //采样输入模式，单端/差分
    u32  Speed : 2;                          //采样速率，默认FW_ADC_SampleSpeed_Low
    u32  BLock : 1;                          //缓存锁
    
    __RO_ u32  Vref_Flag : 1;                //基准标识，False：固定基准；True：通过底层获取
    
    u32 Buffer_Size: 16;                     //数据缓存大小，单位：Bus Width
    
    u32 : 3;
    
    RB_Type FIFO;                            //采样数据保存FIFO
}FW_ADC_Device_Type;


typedef struct FW_ADC_Driver
{
    void (*Init)(FW_ADC_Bus_Type *bus);
    
    void (*CTL)(FW_ADC_Bus_Type *bus, u8 state);
    
    /* ADC校准 */
    void (*Calibration)(FW_ADC_Bus_Type *bus);
    
    /* ADC采样通道的转换时间，单位：ns */
    u32  (*Get_ConvTime)(FW_ADC_Bus_Type *bus, FW_ADC_Device_Type *dev);
    
    /*
     1、使用DMA方式时，应在底层定义一个数组并赋值给bus->Buffer;
     2、使用中断或轮询方式时，则通过Read读取ADC的转换值
     */
    u32  (*Read)(FW_ADC_Bus_Type *bus);
    
    /* ADC特殊通道采集值计算公式 */
    u32  (*Vref_Formula)(FW_ADC_Bus_Type *bus, u32 value);
    s32  (*Temp_Formula)(FW_ADC_Bus_Type *bus, u32 value);
    u32  (*Vbat_Formula)(FW_ADC_Bus_Type *bus, u32 value);
    
    u32  (*Get_Clock)(FW_ADC_Bus_Type *bus);
    
    void (*WDG_Config)(FW_ADC_Bus_Type *bus, FW_ADC_WDG_Type *wdg);
    
    /* ADC转换触发方式设置 */
//    void (*Trigger_Config)(FW_ADC_Bus_Type *bus);
}FW_ADC_Driver_Type;


/* 获取节点对应的设备 */
__INLINE_STATIC_ FW_ADC_Device_Type *FW_ADC_GetDevice(FW_SList_Type *list)
{
    return list ? (FW_ADC_Device_Type *)Container_Of(list, FW_ADC_Device_Type, List) : NULL;
}

/* 获取ADC Bus上第一个设备 */
__INLINE_STATIC_ FW_ADC_Device_Type *FW_ADC_Bus_GetHead(FW_ADC_Bus_Type *bus)
{
    return FW_ADC_GetDevice(bus->List.Next);
}

/* 获取当前设备关联的下一个设备 */
__INLINE_STATIC_ FW_ADC_Device_Type *FW_ADC_GetNext(FW_ADC_Device_Type *dev)
{
    return FW_ADC_GetDevice(dev->List.Next);
}


void FW_ADC_Bus_SetPort(FW_ADC_Bus_Type *bus, void *adc);
void *FW_ADC_Bus_GetPort(FW_ADC_Bus_Type *bus);

void FW_ADC_Bus_Init(void *pbus);

u32  FW_ADC_Bus_GetFS(FW_ADC_Bus_Type *bus);
u32  FW_ADC_Bus_GetClock(FW_ADC_Bus_Type *bus);
u8   FW_ADC_Bus_GetChannelNum(FW_ADC_Bus_Type *bus);

void FW_ADC_Bus_CTL(FW_ADC_Bus_Type *bus, u8 state);

void FW_ADC_Bus_IH(FW_ADC_Bus_Type *bus);
void FW_ADC_Bus_SC_IH(FW_ADC_Bus_Type *bus);


void FW_ADC_Init(FW_ADC_Device_Type *dev);

void FW_ADC_Lock(FW_ADC_Device_Type *dev);
void FW_ADC_Unlock(FW_ADC_Device_Type *dev);

void *FW_ADC_GetBuffer(FW_ADC_Device_Type *dev);
u32  FW_ADC_GetBSize(FW_ADC_Device_Type *dev);
u8   FW_ADC_GetWidth(FW_ADC_Device_Type *dev);
u32  FW_ADC_GetBDLength(FW_ADC_Device_Type *dev);

u32  FW_ADC_Read(FW_ADC_Device_Type *dev, void *pdata, u32 num);

u32  FW_ADC_GetVref(FW_ADC_Device_Type *dev);
s32  FW_ADC_GetTemp(FW_ADC_Device_Type *dev);
u32  FW_ADC_GetVbat(FW_ADC_Device_Type *dev);


#ifdef __cplusplus
}
#endif

#endif

