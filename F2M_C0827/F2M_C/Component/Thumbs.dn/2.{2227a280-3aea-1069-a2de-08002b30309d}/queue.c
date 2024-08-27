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
 
#include ".\queue.h"
#include "mm.h"
#include "libc.h"


extern void FW_Lock(void);
extern void FW_Unlock(void);


#define Q_Lock               FW_Lock
#define Q_Unlock             FW_Unlock

#define QNode_Get(list)      Container_Of((list), QNode_Type, List)


typedef struct
{
    void *Buffer;
    u32  Size;
    FW_SList_Type List;
}QNode_Type;


Queue_Type *Queue_Create(void)
{
    Queue_Type *q = MM_Malloc(sizeof(Queue_Type));
    if(q != NULL)
    {
        q->Head = NULL;
        q->Tail = NULL;
    }
    return q;
}

void Queue_Delete(Queue_Type *q)
{
    QNode_Type *node;
    FW_SList_Type *list;
    
    if(q == NULL)  return;
    
    list = q->Head;
    
    Q_Lock();
    while(list)
    {
        node = QNode_Get(list);
        list = FW_SList_GetNext(list);
        MM_Free(node->Buffer);
        MM_Free(node);
    }
    Q_Unlock();
    
    MM_Free(q);
}

u32  Queue_Get_Count(Queue_Type *q)
{
    FW_SList_Type *list;
    u32 count;
    
    Q_Lock();
    if(q->Head == NULL || q->Tail == NULL)
    {
        Q_Unlock();
        return 0;
    }
    
    if(q->Head == q->Tail)
    {
        Q_Unlock();
        return 1;
    }
    
    list = q->Head;
    count = 0;
    
    while(list)
    {
        count++;
        list = FW_SList_GetNext(list);
    }
    Q_Unlock();
    
    return count;
}

u32  Queue_Write(Queue_Type *q, const void *pdata, u32 num)
{
    QNode_Type *node;
    
    node = MM_Malloc(sizeof(QNode_Type));
    if(node == NULL)  return 0;
    
    node->Buffer = MM_Malloc(num);
    if(node->Buffer == NULL)
    {
        MM_Free(node);
        return 0;
    }
    
    Q_Lock();
    memcpy(node->Buffer, pdata, num);       //此处只能使用memcpy进行拷贝而不能使用指针直接指向
    node->Size = num;
    
    if(q->Head == NULL || q->Tail == NULL)
    {
        /* 写入第一个队员 */
        q->Head = &node->List;
        q->Tail = &node->List;
        FW_SList_Insert(q->Head, q->Tail);
    }
    else
    {
        FW_SList_Insert(q->Tail, &node->List);
        q->Tail = &node->List;
        q->Tail->Next = NULL;
    }
    Q_Unlock();
    
    return num;
}

u32  Queue_Read(Queue_Type *q, void *pdata)
{
    QNode_Type *node;
    u32 num;
    
    Q_Lock();
    if(q->Head == NULL || q->Tail == NULL)  
    {
        Q_Unlock();
        return 0;
    }
    
    node = QNode_Get(q->Head);
    if(node == NULL)  
    {
        Q_Unlock();
        return 0;
    }
    
    memcpy(pdata, node->Buffer, node->Size);
    num = node->Size;
    
    MM_Free(node->Buffer);
    MM_Free(node);
    
    if(q->Head == q->Tail)
    {
        /* 读取最后一个队员 */
        q->Head = NULL;
        q->Head = NULL;
    }
    else
    {
        q->Head = FW_SList_GetNext(&node->List);
    }
    Q_Unlock();
    
    return num;
}


#include "project_debug.h"
#if MODULE_TEST && ALGO_TEST && QUEUE_TEST
#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_delay.h"


#define LOG_DEV_ID      "log"
#define UART_DRV_ID     FW_DRIVER_ID(FW_Device_UART, UART_IOSIM_DRV_NUM)

static FW_UART_Type Log_UART;

static void Log_UART_Config(void *pdata)
{
    FW_UART_Driver_Type *drv = FW_Driver_Find(UART_DRV_ID);
    FW_UART_Type *dev = pdata;
    
    dev->Baudrate = 9600;
    dev->TX_Pin = PA9;
    
    FW_Device_SetDriver(dev, drv);
    FW_Device_SetDID(dev, UART_DRV_ID);
}
FW_DEVICE_STATIC_REGIST(LOG_DEV_ID, &Log_UART, Log_UART_Config, Log_UART);


void Test(void)
{
    Queue_Type *q;
    FW_UART_Type *log;
    u32 count = 0;
    u32 tmp;
    u32 i;
    u8 r;
    
    log = FW_Device_Find(LOG_DEV_ID);
    if(log == NULL)  while(1);
    
    FW_UART_Init(log);
    FW_UART_PrintInit(log);
    
    printf("tx ok\r\n");
    
    q = Queue_Create();
    if(q == NULL)
    {
        printf("队列创建失败\r\n");
        while(1);
    }
    
    while(1)
    {
        tmp = FW_Delay_GetTick();
        srand(tmp);
        r = rand() % 10;
        if(r == 0)  continue;
        
        for(i = 0; i < r; i++)
        {
            tmp = Queue_Write(q, &count, sizeof(count));
            if(tmp)
            {
                printf("队列写入成功, value=%d\r\n", count);
            }
            else
            {
                printf("队列写入%d失败\r\n", count);
                while(1);
            }
            
            count++;
        }
        
        while(1)
        {
            tmp = Queue_Get_Count(q);
            printf("当前队列长度%d\r\n", tmp);
            
            if(tmp == 0)  break;
            
            if(Queue_Read(q, &tmp))
            {
                printf("队列读取成功，value=%d\r\n", tmp);
            }
            else
            {
                printf("队列读取失败\r\n");
                while(1);
            }
            
            FW_Delay_Ms(100);
        }
    }
}

#endif

