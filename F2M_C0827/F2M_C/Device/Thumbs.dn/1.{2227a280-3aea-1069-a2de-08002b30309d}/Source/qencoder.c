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

