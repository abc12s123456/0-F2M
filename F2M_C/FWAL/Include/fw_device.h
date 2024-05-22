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

#ifndef _FW_DEVICE_H_
#define _FW_DEVICE_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_define.h"
#include "fw_config.h"
#include "fw_list.h"

#include "fw_driver.h"

#include "libc.h"


/**
 * @device flag
 */
#define FW_DEVICE_READ                           (0x01)
#define FW_DEVICE_WRITE                          (0x02)
#define FW_DEVICE_WR                             (FW_DEVICE_READ | FW_DEVICE_WRITE)

#define FW_DEVICE_REMOVE
#define FW_DEVICE_ACTIVE
#define FW_DEVICE_SUSPEND

/**
 * @device open flag
 */
#define FW_DEVICE_OFLAG_CLOSE                    (0x00)                         /* 设备已关 */
#define FW_DEVICE_OFLAG_RO                       (0x01)                         /* 设备只读访问 */
#define FW_DEVICE_OFLAG_WO                       (0x02)                         /* 设备只写访问 */
#define FW_DEVICE_OFLAG_WR                       (0x03)                         /* 设备读写访问 */
#define FW_DEVICE_OFLAG_OPEN                     (0x08)                         /* 设备已打开 */
#define FW_DEVICE_OFLAG_MASK                     (0x0F)                         /* 设备打开方式掩码 */

/**
 * @device state flag
 */
#define FW_DEVICE_SFLAG_UNINIT                   (0x0000)                       /* 设备未初始化 */

#define FW_DEVICE_SFLAG_READ                     (0x0001)                       /* 设备读 */
#define FW_DEVICE_SFLAG_WRITE                    (0x0002)                       /* 设备写 */
#define FW_DEVICE_SFLAG_WR                       (0x0003)                       /* 设备读写 */

#define FW_DEVICE_SFLAG_REMOVE                   (0x0004)                       /* 设备已移除 */
#define FW_DEVICE_SFLAG_STANDALONE               (0x0008)                       /* 独立设备 */
#define FW_DEVICE_SFLAG_INIT                     (0x0010)                       /* 设备已初始化 */
#define FW_DEVICE_SFLAG_SUSPEND                  (0x0020)                       /* 设备挂起 */
#define FW_DEVICE_SFLAG_STREAM                   (0x0040)                       /* 设备以流模式工作 */

#define FW_DEVICE_SFLAG_POL_RX                   (0x0100)                       /* 轮询接收 */
#define FW_DEVICE_SFLAG_INT_RX                   (0x0200)                       /* 中断接收 */
#define FW_DEVICE_SFLAG_DMA_RX                   (0x0400)                       /* DMA接收 */

#define FW_DEVICE_SFLAG_POL_TX                   (0x1000)                       /* 轮询发送 */
#define FW_DEVICE_SFLAG_INT_TX                   (0x2000)                       /* 中断发送 */
#define FW_DEVICE_SFLAG_DMA_TX                   (0x4000)                       /* DMA发送 */

#define FW_DEVICE_SFLAG_MASK                     (0xFFFF)


typedef enum
{
    FW_Device_NULL = 0,
    
    FW_Device_System,                            //
    FW_Device_Delay,                             //
    FW_Device_GPIO,                              //
    
    FW_Device_BKP,                               //备份寄存器
    FW_Device_S2RAM,                             //S2RAM
    FW_Device_EXTI,                              //外部中断
    FW_Device_Flash,                             //闪存
    FW_Device_RTC,                               //RTC
    FW_Device_WDG,                               //看门狗
    FW_Device_PWR,
    
    FW_Device_Bus,                               //系统总线
    FW_Device_Bus_Device,                        //
    
    FW_Device_UART,                              //UART
    FW_Device_UART_Device,                       //
    
    FW_Device_SPI,                               //SPI
    FW_Device_SPI_Device,                        //
    
    FW_Device_I2C,                               //I2C
    FW_Device_I2C_Device,                        //
    
    FW_Device_Timer,                             //定时器
    FW_Device_PWM,                               //PWM
    FW_Device_Encoder,                           //编码器
    
    FW_Device_CAN,                               //CAN
    FW_Device_QSPI,                              //QSPI
    
    FW_Device_USB,                               //USB
    
    FW_Device_ADC,                               //ADC
    FW_Device_DAC,                               //DAC
    
    FW_Device_Mifare,                            //Mifare卡
    FW_Device_WiFi,                              //WiFi模组
    FW_Device_Sensor,                            //通用传感器
    FW_Device_TP,                                //触摸屏
    FW_Device_LCD,                               //
    
    FW_Device_OS,                                //RTOS
}FW_Device_Enum;


/**
 * @device ops
 */
typedef struct FW_Device_OPS
{
    fw_err_t  (*Init)(void *dev);
    fw_err_t  (*Open)(void *dev, u32 oflag, u32 timeout);
    fw_err_t  (*Close)(void *dev);
    u32       (*Write)(void *dev, u32 offset, const void *pdata, u32 num);
    u32       (*Read)(void *dev, u32 offset, void *pdata, u32 num);
    fw_err_t  (*Control)(void *dev, s32 cmd, void *args);
}FW_Device_OPS_Type, FD_OPS_Type;


/**
 * @设备对象(基类)
 */
typedef struct FW_Device
{
    char Name[DEVICE_NAME_MAX];                  //设备名称
    
    u32 State_Flag : 4;                          //状态标识
    u32 Open_Flag : 16;                          //打开标识
    u32 RCount : 12;                             //引用计数
    
    struct FW_Device *Parent;                    //父设备(依赖的设备对象，可以为空)
    const void *Driver;                          //Drvier实例
        
#if MULTI_TASK_EN
    void *Mutex;                                 //互斥量(多任务时使用)
#endif
    
#if DEVICE_OPS_EN
    FW_Device_OPS_Type *OPS;                     //
#endif
    
    void *Pdata;                                 //私有数据
    
    FW_List_Type List;                           //设备管理链表
}FW_Device_Type;


/**
 * @设备关联定义
 */
typedef struct
{
    char Name[DEVICE_NAME_MAX];                  //设备名称
    FW_Device_Type *Device;                      //设备对象
    void (*Init)(void *);                        //前期初始函数
}FW_Device_Section_Type;

#define FW_DEVICE_SECTION __USED_ __SECTION_("Device_Table")

#define FW_DEVICE_STATIC_REGIST(name, dev, init, desc)\
FW_DEVICE_SECTION const FW_Device_Section_Type __Device_##desc =\
{\
    name,\
    (FW_Device_Type *)(dev),\
    init,\
}

#define FW_DEVICE_DECLARE()  SECTION_DECLARE(Device_Table, FW_Device_Section_Type)
/**/


fw_ret_t FW_Device_Regist(const char *name, void *dev);

void *FW_Device_Find(const char *name);
void *FW_Device_MaskFind(u32 dev_id, u32 mask);
void *FW_Device_Foreach(void *dev);

void FW_Device_Print(void);


/**
 * @设备名称
 */
__INLINE_STATIC_ char *FW_Device_GetName(void *dev)
{
    return (void *)((FW_Device_Type *)dev)->Name;
}

__INLINE_STATIC_ void FW_Device_SetName(void *dev, const char *name)
{
    strncpy(((FW_Device_Type *)dev)->Name, name, DEVICE_NAME_MAX);
}

/**
 * @设备驱动
 */
__INLINE_STATIC_ void *FW_Device_GetDriver(void *dev)
{
    return (void *)((FW_Device_Type *)dev)->Driver;
}

__INLINE_STATIC_ void FW_Device_SetDriver(void *dev, const void *drv)
{
    ((FW_Device_Type *)dev)->Driver = drv;
}

/**
 * @父设备
 */
__INLINE_STATIC_ void *FW_Device_GetParent(void *dev)
{
    return (void *)((FW_Device_Type *)dev)->Parent;
}

__INLINE_STATIC_ void FW_Device_SetParent(void *dev, void *parent)
{
    ((FW_Device_Type *)dev)->Parent = parent;
}
/**/


/**
 * @设备比对
 */
#define Isdev(name, pstr)  (!strncmp(name, pstr, DEVICE_NAME_MAX))


/**
 * @设备底层中断服务处理
 * @dev, 设备对象
 * @pf, 中断服务函数，若有中断触发返回True，无则返回False
        pf不能为空
 * @适用于一个中断服务函数中包含多个对象的情况，当判断前面已经有中断触发，就无需
    继续执行后面的逻辑，减少了中断处理的耗时
 */
#define FW_DEVICE_LIH(dev, pf)\
    if(pf(dev))  return ;\


fw_err_t FW_Device_Init(FW_Device_Type *dev);

fw_err_t FW_Device_Open(FW_Device_Type *dev, u16 oflag, u32 timeout);
fw_err_t FW_Device_Close(FW_Device_Type *dev);

u32      FW_Device_Write(FW_Device_Type *dev, u32 offset, const void *pdata, u32 num);
u32      FW_Device_Read(FW_Device_Type *dev, u32 offset, void *pdata, u32 num);

fw_err_t FW_Device_IOCTL(FW_Device_Type *dev, s32 cmd, void *args);


#ifdef __cplusplus
}
#endif

#endif

