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
 
#include "fw_device.h"
#include "fw_print.h"

#include "libc.h"


extern void FW_SDI(void);
extern void FW_SEI(void);

extern void FW_Lock(void);
extern void FW_Unlock(void);


/**
 * @设备定义
 */
FW_DEVICE_DECLARE();


/**
 * @设备管理链表
 */
FW_List_Type Device_List;


/**
@功能：将设备绑定到设备管理链表上
@参数：无
@返回：无
@说明：1、该函数绑定的是静态定义的设备；动态定义的设备由FW_Device_Dynamic_Regist
          函数在执行注册时绑定到设备链表。
       2、静态注册的设备，位于Device_List的Prev方向。
       3、注册的设备不允许重复(以设备Name进行判定)。
*/
void FW_Device_PreBind(void)
{
    FW_Device_Section_Type *pstart, *pend, *pdev;
    FW_Device_Section_Type *ptmp;
    
    FW_List_Init(&Device_List);
    
    pstart = SECTION_GET_START(Device_Table, FW_Device_Section_Type);
    pend = SECTION_GET_END(Device_Table, FW_Device_Section_Type);

    for(pdev = pstart; pdev < pend; pdev++)
    {
        /* 检查注册设备的ID是否有重复 */
        ptmp = pdev + 1;
        while(1)
        {
            /* 逐一比对，直到最后一个设备为止 */
            if(ptmp == pend)  break;
            
            /*  */ 
            if(strncmp(pdev->Name, ptmp->Name, DEVICE_NAME_MAX) != 0)
            {
                ptmp++;
            }
            else
            {
                LOG_E("设备%s重复定义，这是不允许的\r\n", pdev->Name);
                while(1);
            }
        }
        
        /* 将设备插入链表 */
        FW_List_Init(&pdev->Device->List);
        if(pdev->Device != NULL)
        {
            FW_List_InsertBefore(&Device_List, &pdev->Device->List);
        }
        
        /* 写入设备Name */
        strcpy(pdev->Device->Name, pdev->Name);
    }
}

/**
@功能：设备前期初始化
@参数：无
@返回：无
@说明：1、用户注册设备时，对于一些固定的配置(例如使用的IO、速率、调用的实例等)，可以
          先进行配置。该函数会在进入Main之前调用。
       2、设备遍历过程中，自动匹配驱动。
       3、该函数只能对静态定义的设备进行初始配置。
*/
void FW_Device_PreInit(void)
{
    FW_Device_Section_Type *pstart, *pend, *pdev;
    void *pdrv;
    
    pstart = SECTION_GET_START(Device_Table, FW_Device_Section_Type);
    pend = SECTION_GET_END(Device_Table, FW_Device_Section_Type);
    
    if(pstart != NULL && pend != NULL)
    {
        for(pdev = pstart; pdev < pend; pdev++)
        {
            if(pdev->Init)  pdev->Init(pdev->Device);
            
            pdrv = FW_Driver_Find(pdev->Device->DID);
            if(pdrv != NULL)
            {
                pdev->Device->Driver = pdrv;
            }
        }
    }
}
/**/

/**
@功能：设备注册
@参数：dev_id, 设备ID
       dev, 待注册的设备
@返回：无
@说明：1、该函数可以注册动态及静态定义的设备。
       2、该函数注册的设备(无论静态还是动态定义)，位于Device_List的Next方向。
       3、该函数在注册设备时，同时探知所需的驱动。
*/
fw_ret_t FW_Device_Regist(const char *name, void *dev)
{
    FW_Device_Type *pdev, *ptmp;
    FW_List_Type *list, *ltmp;
    void *pdrv;
    
    pdev = (FW_Device_Type *)dev;
    
    /* 临界保护，防止被其它任务干扰 */
    FW_Lock();
    
    /* 遍历设备链表，查看设备是否已注册 */
    FW_LIST_FOREACH(list, ltmp, &Device_List)
    {
        ptmp = Container_Of(list, FW_Device_Type, List);
        if(strncmp(ptmp->Name, name, DEVICE_NAME_MAX) == 0)
        {
            LOG_D("设备%s已存在，无法再注册\r\n", name);
            FW_Unlock();
            return FW_RFAIL;
        }
    }
    
    /* 新设备 */
    FW_List_InsertAfter(&Device_List, &pdev->List);
    strcpy(pdev->Name, name);
    
    /* 设备注册完成，退出临界保护 */
    FW_Unlock();
    
    /* 探测设备驱动 */
    pdrv = FW_Driver_Find(pdev->DID);
    if(pdrv != NULL)
    {
        pdev->Driver = pdrv;
    }
    else
    {
        LOG_D("设备%d没有找到匹配的驱动\r\n", pdev->ID);
    }
    
    return FW_ROK;
}
/**/

/*
2023.11.05 使用FW_SDI,FW_SEI替代FW_Lock,FW_Unlock,
*/
void *FW_Device_Find(const char *name)
{
    FW_Device_Type *ptmp;
    FW_List_Type *list, *ltmp;
    
    FW_SDI();
    FW_LIST_FOREACH(list, ltmp, &Device_List)
    {
        ptmp = Container_Of(list, FW_Device_Type, List);
        if(strncmp(ptmp->Name, name, DEVICE_NAME_MAX) == 0)
        {
            FW_SEI();
            return ptmp;
        }
    }
    FW_SEI();
    
    return NULL;
}
/**/

/**
@功能：设备遍历
@参数：dev, 前一次调用时返回的设备，第一次应输入NULL
@返回：当前遍历结果，若为空，说明已遍历完整个设备序列
@说明：无
*/
void *FW_Device_Foreach(void *dev)
{
    FW_Device_Type *pdev;
    FW_List_Type *list;
    
    FW_Lock();
    
    if(dev == NULL)
    {
        /* 第一次执行 */
        list = FW_List_GetNext(&Device_List);
    }
    else
    {
        pdev = (FW_Device_Type *)dev;
        list = FW_List_GetNext(&pdev->List);
    }
    
    if(list == &Device_List)
    {
        FW_Unlock();
        return NULL;
    }
    
    pdev = Container_Of(list, FW_Device_Type, List);
    
    FW_Unlock();
    
    return pdev;
}
/**/






