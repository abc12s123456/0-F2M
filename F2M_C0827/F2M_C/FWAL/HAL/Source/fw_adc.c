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
#include "fw_adc.h"


#if defined(ADC_MOD_EN) && ADC_MOD_EN


#include "fw_print.h"
#include "fw_gpio.h"

#include "lib_int.h"
#include "lib_string.h"

#include "libc.h"


extern void FW_Lock(void);
extern void FW_Unlock(void);


static void IH_DMA_CB(FW_ADC_Bus_Type *bus);
static void IH_INT_CB(FW_ADC_Bus_Type *bus);
static void IH_POL_CB(FW_ADC_Bus_Type *bus);


static u32  FW_ADC_Bus_GetConvTime(FW_ADC_Bus_Type *bus);
static void FW_ADC_BindDevice(FW_ADC_Device_Type *dev);


/* 设置ADC总线端口 */
void FW_ADC_Bus_SetPort(FW_ADC_Bus_Type *bus, void *adc)
{
    if(bus->ADCx == NULL)  bus->ADCx = adc;
}

void *FW_ADC_Bus_GetPort(FW_ADC_Bus_Type *bus)
{
    return bus->ADCx;
}

/* ADC总线初始化 */
void FW_ADC_Bus_Init(void *pbus)
{
    FW_ADC_Bus_Type *bus;
    FW_ADC_Driver_Type *drv;
    FW_ADC_Device_Type *dev;
    
    bus = (FW_ADC_Bus_Type *)pbus;
    
    FW_ASSERT(bus);
    
    dev = NULL;
    
    drv = FW_Device_GetDriver(bus);
    if(drv == NULL)
    {
        LOG_E("ADC总线驱动%d不存在\r\n", FW_Device_GetDID(bus));
        return;
    }
    
    FW_ASSERT(drv->Init);
    FW_ASSERT(drv->Get_Clock);
    
    bus->NList = NULL;
    bus->Clock = drv->Get_Clock(bus);
    
    /* 根据配置，关联不同的中断服务函数 */
    if(bus->Mode == FW_ADC_Mode_DMA)
    {
        if(bus->IH_CB == NULL)  bus->IH_CB = IH_DMA_CB;
    }
    else if(bus->Mode == FW_ADC_Mode_INT)
    {
        if(bus->IH_CB == NULL)  bus->IH_CB = IH_INT_CB;
    }
    else if(bus->Mode == FW_ADC_Mode_POL)
    {
        if(bus->IH_CB == NULL)  bus->IH_CB = IH_POL_CB;
    }
    else
    {
        LOG_E("数据采集方式配置错误\r\n");
        while(1);
    }
    
    /* 将已注册的ADC设备绑定到总线上 */
    while(1)
    {
        dev = FW_Device_Foreach(dev);
        if(dev)
        {
            if((FW_ADC_Bus_Type *)dev->Device.Parent == bus)
            {
                FW_ADC_BindDevice(dev);
                bus->Channel_Num++;
            }
        }
        else
        {
            /* 遍历完成，没有新的ADC设备 */
            break;
        }
    }
    
    FW_ADC_Bus_GetConvTime(bus);
    
    drv->Init(bus);
}

/* 获取满量程值 */
u32  FW_ADC_Bus_GetFS(FW_ADC_Bus_Type *bus)
{
    return ((1 << bus->Resolution) - 1);
}

/* 获取ADC时钟 */
u32  FW_ADC_Bus_GetClock(FW_ADC_Bus_Type *bus)
{
    return bus->Clock;
}

/* 获取ADC采样通道编号 */
u8   FW_ADC_Bus_GetChannelNum(FW_ADC_Bus_Type *bus)
{
    return bus->Channel_Num;
}

/* ADC使能控制 */
void FW_ADC_Bus_CTL(FW_ADC_Bus_Type *bus, u8 state)
{
    FW_ADC_Driver_Type *drv = FW_Device_GetDriver(bus);
    FW_ASSERT(drv->CTL);
    drv->CTL(bus, state);
}

/* 中断服务函数，由底层中断调用 */
void FW_ADC_Bus_IH(FW_ADC_Bus_Type *bus)
{
    if(bus->IH_CB)  bus->IH_CB(bus);
}

void FW_ADC_Bus_SC_IH(FW_ADC_Bus_Type *bus)
{
    if(bus->Mode == FW_ADC_Mode_INT)
    {
        /* 使用中断方式采样时，一个采样序列采集完毕，将ADC设备管理链表重新指向起
           始位置，可以在转换序列受异常影响导致混乱时纠正转换序列 */
        bus->NList = NULL;
    }
}

static u32  FW_ADC_Bus_GetConvTime(FW_ADC_Bus_Type *bus)
{
    FW_ADC_Driver_Type *drv = FW_Device_GetDriver(bus);
    FW_SList_Type *list = bus->List.Next;
    FW_ADC_Device_Type *dev;
    u32 time = 0;
    
    FW_ASSERT(drv->Get_ConvTime);
    
    while(list)
    {
        dev = FW_ADC_GetDevice(list);
        if(dev != NULL)
        {
            /* 获取ADC使用的所有通道的转换时间 */
            time += drv->Get_ConvTime(bus, dev);
        }
        list = FW_SList_GetNext(list);
    }
    
    bus->Total_ConvTime = time;
    
    return time;
}


/* ADC设备(通道)初始化 */
void FW_ADC_Init(FW_ADC_Device_Type *dev)
{
    FW_ADC_Bus_Type *bus = FW_Device_GetParent(dev);
    u8 block_size;
    
    if(bus == NULL)
    {
        LOG_E("ADC's bus is null\r\n");
        while(1);
    }
    
    /* ADC数据位宽配置 */
    if(bus->Width == FW_ADC_Width_16Bits)
    {
        block_size = sizeof(u16);
    }
    else if(bus->Width == FW_ADC_Width_8Bits)
    {
        block_size = sizeof(u8);
    }
    else if(bus->Width == FW_ADC_Width_32Bits)
    {
        block_size = sizeof(u32);
    }
    else
    {
        block_size = sizeof(u16);
    }
    
    /* ADC缓存大小配置 */
    if(dev->Buffer_Size == 0)
    {
        dev->Buffer_Size = ADC_BUFFER_SIZE;
    }
    
    /* 唤醒缓冲区配置 */
    if(RB_Init(&dev->FIFO, dev->Buffer_Size * block_size) != True)
    {
        LOG_E("ADC's buffer is null\r\n");
        while(1);
    }
    
    dev->List.Next = NULL;
    dev->BLock = False;
    
    /* 初始化ADC使用的IO；若使用差分输入，还需要初始化N_Pin */
    FW_GPIO_Init(dev->P_Pin, FW_GPIO_Mode_AIN, FW_GPIO_Speed_Low);
    if(dev->Input_Mode == FW_ADC_Input_Differential)
    {
        FW_GPIO_Init(dev->N_Pin, FW_GPIO_Mode_AIN, FW_GPIO_Speed_Low);
    }
}

void FW_ADC_Lock(FW_ADC_Device_Type *dev)
{
    dev->BLock = True;
}

void FW_ADC_Unlock(FW_ADC_Device_Type *dev)
{
    dev->BLock = False;
}

/* 获取ADC设备缓存(指针) */
void *FW_ADC_GetBuffer(FW_ADC_Device_Type *dev)
{
    return dev->FIFO.Buffer;
}

/* 获取ADC设备缓存大小 */
u32  FW_ADC_GetBSize(FW_ADC_Device_Type *dev)
{
    return dev->Buffer_Size;
}

/* 获取ADC设备的数据位宽 */
u8   FW_ADC_GetWidth(FW_ADC_Device_Type *dev)
{
    FW_ADC_Bus_Type *p = FW_Device_GetParent(dev);
    return p->Width;
}

/* 获取ADC设备缓存中数据数量，单位：Bus Width */
u32  FW_ADC_GetBDLength(FW_ADC_Device_Type *dev)
{
    FW_ADC_Bus_Type *p = FW_Device_GetParent(dev);
    return (RB_Get_DataLength(&dev->FIFO) / p->Width);
}

u32  FW_ADC_Read(FW_ADC_Device_Type *dev, void *pdata, u32 num)
{
    FW_ADC_Bus_Type *p = FW_Device_GetParent(dev);
    return RB_Read_All(&dev->FIFO, pdata, num * p->Width);
}

u32  FW_ADC_GetVref(FW_ADC_Device_Type *dev)
{
    FW_ADC_Bus_Type *bus = FW_Device_GetParent(dev);
    FW_ADC_Driver_Type *drv = FW_Device_GetDriver(bus);
    void *buffer;
    
    u32 sum = 0;
    u32 num;
    u32 i;
    
    u8 width = bus->Width;
    u32 (*rd_buf)(void *, u32);
    
    if(bus->Vref != 0 && dev->Vref_Flag == False)
    {
        /* 使用固定基准 */
        return bus->Vref;
    }
    
    dev->Vref_Flag = True;
    
    if(width == FW_ADC_Width_16Bits)
    {
        rd_buf = Int16_ReadBuffer;
    }
    else if(width == FW_ADC_Width_32Bits)
    {
        rd_buf = Int32_ReadBuffer;
    }
    else if(width == FW_ADC_Width_8Bits)
    {
        rd_buf = Int8_ReadBuffer;
    }
    else
    {
        LOG_E("ADC数据位宽设置错误\r\n");
        while(1);
    }
    
    buffer = FW_ADC_GetBuffer(dev);
    num = FW_ADC_GetBSize(dev);
    
    FW_ADC_Lock(dev);
    for(i = 0; i < num; i++)
    {
        sum += rd_buf(buffer, i);
    }
    FW_ADC_Unlock(dev);
    
    sum /= num;
    
    FW_ASSERT(drv->Vref);
    bus->Vref = drv->Vref_Formula(bus, sum);
    
    return bus->Vref;
}

s32  FW_ADC_GetTemp(FW_ADC_Device_Type *dev)
{
    FW_ADC_Bus_Type *bus = FW_Device_GetParent(dev);
    FW_ADC_Driver_Type *drv = FW_Device_GetDriver(bus);
    void *buffer = FW_ADC_GetBuffer(dev);
    
    u32 sum = 0;
    u32 num = FW_ADC_GetBSize(dev);
    u32 i;
    
    u8 width = bus->Width;
    u32 (*rd_buf)(void *, u32);
    
    if(width == FW_ADC_Width_16Bits)
    {
        rd_buf = Int16_ReadBuffer;
    }
    else if(width == FW_ADC_Width_32Bits)
    {
        rd_buf = Int32_ReadBuffer;
    }
    else if(width == FW_ADC_Width_8Bits)
    {
        rd_buf = Int8_ReadBuffer;
    }
    else
    {
        LOG_E("ADC数据位宽设置错误\r\n");
        while(1);
    }
    
    FW_ADC_Lock(dev);
    for(i = 0; i < num; i++)
    {
        sum += rd_buf(buffer, i);
    }
    FW_ADC_Unlock(dev);
    
    sum /= num;
    
    FW_ASSERT(drv->Temp_Formula);
    return drv->Temp_Formula(bus, sum);
}

static void IH_DMA_CB(FW_ADC_Bus_Type *bus)
{
    FW_ADC_Device_Type *dev;
    FW_SList_Type *list = &bus->List;
    u8 *buffer = (u8 *)bus->Buffer;
    u8 offset = 0;
    
    /* 指向第一个设备节点 */
    list = list->Next;
    
    while(list)
    {
        dev = FW_ADC_GetDevice(list);
        if(dev)
        {
            /* 当有其它任务在读缓存，则暂不更新缓存数据 */
            if(dev->BLock != True)
            {
                /* 以设定的采样周期将数据存入对应的FIFO */
                if((dev->Sample_Count * bus->Total_ConvTime) >= (dev->Sample_Period * 1000))
                {
                    dev->Sample_Count = 0;
                    RB_Write(&dev->FIFO, buffer + offset, bus->Width);
                }
                else
                {
                    /* 未到保存采样值的时间 */
                    dev->Sample_Count++;
                }
            }
                
            /* 指向下一个采样通道对应的缓存 */
            offset += bus->Width;
        }
        
        list = list->Next;
    }
}

static void IH_INT_CB(FW_ADC_Bus_Type *bus)
{
    FW_ADC_Device_Type *dev;
    FW_ADC_Driver_Type *drv;
    FW_SList_Type *list;
    u32 value;
    
    /* 中断方式采集时，始终从当前保存的设备节点开始处理 */
    if(bus->NList == NULL)
    {
        bus->NList = bus->List.Next;
    }
    
    list = bus->NList;
    
    if(list)
    {
        dev = FW_ADC_GetDevice(list);
        if(dev)
        {
            if(dev->BLock != True)
            {
                if((dev->Sample_Count * bus->Total_ConvTime) >= (dev->Sample_Period * 1000))
                {
                    dev->Sample_Count = 0;
                    drv = FW_Device_GetDriver(bus);
                    value = drv->Read(bus);
                    RB_Write(&dev->FIFO, (u8 *)&value, bus->Width);
                }
                else
                {
                    dev->Sample_Count++;
                }
            }
        }
        
        bus->NList = FW_SList_GetNext(list);
    }
}

static void IH_POL_CB(FW_ADC_Bus_Type *bus)
{
    
}

static void FW_ADC_BindDevice(FW_ADC_Device_Type *dev)
{
    FW_ADC_Bus_Type *bus = FW_Device_GetParent(dev);
    FW_ADC_Device_Type *pdev;
    FW_SList_Type *c, *p;
    
    /* 始终从第二个位置插入(第一个位置为头节点，绑定ADC Bus) */
    c = bus->List.Next;
    p = &bus->List;
    
    while(c)
    {
        pdev = FW_ADC_GetDevice(c);
        if(pdev != NULL)
        {
            /* 按照Channel的大小由小至大进行排列 */
            if(dev->Channel < pdev->Channel)
            {
                break;
            }
            else
            {
                /* 已经存在相同的采样通道 */
                if(dev->Channel == pdev->Channel)
                {
                    if(dev == pdev)
                    {
                        /* 同一个设备 */
                        return;
                    }
                    else
                    {
                        LOG_E("不允许不同的设备使用相同的采样通道\r\n");
                        while(1);
                    }
                }
                p = c;
                c = c->Next;
            }
        }
//        else
//        {
//            /* 空对象依然占据一个有效的节点 */
//            p = c;
//            c = c->Next;
//        }
    }
    
    /* 检索到合适的位置或空结点，则插入当前结点 */
    FW_SList_Insert(p, &dev->List);
}


#endif  /* defined(ADC_MOD_EN) && ADC_MOD_EN */

