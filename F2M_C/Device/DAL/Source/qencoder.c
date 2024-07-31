#include "qencoder.h"
#include "fw_debug.h"
#include "fw_print.h"


#define QENCODER_COUNTER_DEFAULT_MAX   0xFF      //
#define QENCODER_FILTER_TIMES          3


/*
变化前     变化后
A    B     A    B     状态            备注
0    0     0    0     不动
0    0     0    1      -1
0    0     1    0      +1
0    0     1    1      -2     假定A相跳变时采集数据
0    1     0    1     不动
0    1     1    0      +2     假定A相跳变时采集数据
0    1     1    1      -1
0    1     0    0      +1
1    0     0    0      -1
1    0     0    1      +2     假定A相跳变时采集数据
1    0     1    0     不动
1    0     1    1      +1
1    1     0    0      -2     假定A相跳变时采集数据
1    1     0    1      +1
1    1     1    0      -1
1    1     1    1     不动
*/

void QEncoder_Init(QEncoder_Type *dev)
{
    QEncoder_Driver_Type *drv;
    QEncoder_Config_Type *cfg;
    
    FW_ASSERT(dev);
    
    drv = FW_Device_GetDriver(dev);
    cfg = &dev->Config;
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Init);
    FW_ASSERT(drv->Get_Counter);
    
    if(cfg->Max == 0)  cfg->Max = QENCODER_COUNTER_DEFAULT_MAX;
    
    if(drv->Init)  drv->Init(dev);
    
    dev->BK_Value = 0;
    dev->Filter_Cnt = 0;
    
    dev->CValue = 0;
    dev->PValue = 0;
}

s32 QEncoder_Get(QEncoder_Type *dev)
{
    QEncoder_Driver_Type *drv = FW_Device_GetDriver(dev);
    QEncoder_Config_Type *cfg = &dev->Config;
    s32 value;
    
    FW_ASSERT(drv->Get_Counter);
    
    dev->CValue = drv->Get_Counter(dev) >> 1;
    
    /* Filter */
    if(dev->BK_Value == dev->CValue)
    {
        /* 滤波尚未完成 */
        if(++dev->Filter_Cnt < QENCODER_FILTER_TIMES)
        {
            return 0;
        }
    }
    else
    {
        /* 检测值一直在变化 */
        dev->Filter_Cnt = 0;
        dev->BK_Value = dev->CValue;
        return 0;
    }
    
    if(dev->PValue != dev->CValue)
    {
        if(dev->PValue == cfg->Min)
        {
            if(dev->CValue == cfg->Max)  value = -1;
            else  if(dev->CValue == cfg->Min + 1)  value = 1;
            else  value = 0;
        }
        else if(dev->PValue == cfg->Max)
        {
            if(dev->CValue == cfg->Min)  value = 1;
            else if(dev->CValue == cfg->Max - 1)  value = -1;
            else  value = 0;
        }
        else
        {
//            if(dev->CValue == dev->PValue + 1)  value = 1;
//            else if(dev->CValue == dev->PValue - 1)  value = -1;
//            else  value = 0;
            if(dev->CValue > dev->PValue)  value = 1;
            else if(dev->CValue < dev->PValue)  value = -1;
            else  value = 0;
        }
        
        dev->PValue = dev->CValue;
    }
    else
    {
        value = 0;
    }
    
    return value;
}


#include "project_debug.h"
#if MODULE_TEST && TIM_DEVICE_TEST && QENCODER_TEST
#include "libc.h"


#define SIZE     100

static u8 BK_RAM[SIZE] __AT_(0x2000B000);


void Test(void)
{
    u32 i;
    u8 *p = BK_RAM;
    
    if(*p != 0xA5)
    {
        memset(p, 0, SIZE);
        *p = 0xA5;
    }
    else
    {
        for(i = 0; i < SIZE; i++)
        {
            *p++ = i;
        }
    }
    
    while(1);
}


#endif

