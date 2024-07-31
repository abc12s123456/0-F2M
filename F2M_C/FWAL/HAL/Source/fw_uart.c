#include "fw_uart.h"
#include "fw_system.h"
#include "fw_debug.h"
#include "fw_print.h"

#include <stdio.h>
#include <string.h>


static void FW_UART_IH_RX(void *dev);
static void FW_UART_IH_RC(void *dev);
static void FW_UART_IH_TX(void *dev);
static void FW_UART_IH_TC(void *dev);

static u32  FW_UART_WritePOL(void *dev, u32 offset, const void *pdata, u32 num);
static u32  FW_UART_WriteINT(void *dev, u32 offset, const void *pdata, u32 num);
static u32  FW_UART_WriteDMA(void *dev, u32 offset, const void *pdata, u32 num);

static u32  FW_UART_ReadPOL(void *dev, u32 offset, void *pdata, u32 num);
static u32  FW_UART_ReadINT(void *dev, u32 offset, void *pdata, u32 num);
static u32  FW_UART_ReadDMA(void *dev, u32 offset, void *pdata, u32 num);


void FW_UART_SetPort(FW_UART_Type *dev, void *uart)
{
    if(dev->UARTx == NULL)  dev->UARTx = uart;
}

void *FW_UART_GetPort(FW_UART_Type *dev)
{
    return dev->UARTx;
}

u8   FW_UART_GetTRM(FW_UART_Type *dev, u8 tr)
{
    return (tr == TOR_TX) ? dev->Config.TX_Mode : dev->Config.RX_Mode;
}

void *FW_UART_GetDMABBase(FW_UART_Type *dev, u8 tr)
{
    SGD_Type *dma = (tr == TOR_TX) ? dev->Config.TX_DMA : dev->Config.RX_DMA;
    if(dma)  return dma->Buffer;
    else  return 0;
}

u32  FW_UART_GetDMABSize(FW_UART_Type *dev, u8 tr)
{
    SGD_Type *dma = (tr == TOR_TX) ? dev->Config.TX_DMA : dev->Config.RX_DMA;
    if(dma)  return dma->Size;
    else  return 0;
}

void FW_UART_DeInit(FW_UART_Type *dev)
{
    FW_UART_Driver_Type *drv;
    
    drv = FW_Device_GetDriver(dev);
    if(drv == NULL)
    {
        LOG_D("uart%d driver is invalid\r\n", dev->Parent.Device_ID);
        return;
    }
    
    dev->Baudrate = 9600;
    dev->Data_Bits = FW_UART_DataBits_8;
    dev->Stop_Bits = FW_UART_StopBits_1;
    dev->Parity = FW_UART_Parity_None;
    
    /*  */
//    if(drv->Set_UARTx)  drv->Set_UARTx(dev);
    if(drv->DeInit)  drv->DeInit(dev);
}

void FW_UART_Init(FW_UART_Type *dev)
{
    FW_UART_Driver_Type *drv;
    SGTR_Type sgtr;
    
    SGTR_DeInit(&sgtr);
    
    memcpy(&sgtr.Config, &dev->Config, sizeof(SGC_Type));
    
    sgtr.Config.TX_DB_Size = dev->TX_DB_Size;
    sgtr.Config.RX_DB_Size = dev->RX_DB_Size;
    
    sgtr.Config.TX_RB_Size = dev->TX_RB_Size;
    sgtr.Config.RX_RB_Size = dev->RX_RB_Size;
    
    sgtr.IH_RC = FW_UART_IH_RC;
    sgtr.IH_RX = FW_UART_IH_RX;
    sgtr.IH_TC = FW_UART_IH_TC;
    sgtr.IH_TX = FW_UART_IH_TX;
    
    sgtr.Write_POL = FW_UART_WritePOL;
    sgtr.Write_INT = FW_UART_WriteINT;
    sgtr.Write_DMA = FW_UART_WriteDMA;
    
    sgtr.Read_POL = FW_UART_ReadPOL;
    sgtr.Read_INT = FW_UART_ReadINT;
    sgtr.Read_DMA = FW_UART_ReadDMA;
    
    SGTR_Config(&sgtr);
    
    memcpy(&dev->Config, &sgtr.Config, sizeof(SGC_Type));
    
    drv = FW_Device_GetDriver(dev);
    if(drv == NULL)
    {
        LOG_D("uart%d driver is invalid\r\n", dev->Parent.Device_ID);
        return;
    }
    
    /*  */
//    if(drv->Set_UARTx)  drv->Set_UARTx(dev);
    
    drv->Init(dev);
}

void FW_UART_CTL(FW_UART_Type *dev, u8 state)
{
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->CTL(dev, state);
}

void FW_UART_TXCTL(FW_UART_Type *dev, u8 state)
{
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->TX_CTL(dev, state);
}

void FW_UART_RXCTL(FW_UART_Type *dev, u8 state)
{
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(dev);
    drv->RX_CTL(dev, state);
}

u8  *FW_UART_GetFIFOBBase(FW_UART_Type *dev, u8 tr)
{
    RB_Type *fifo =(tr == TOR_TX) ? dev->Config.TX_FIFO : dev->Config.RX_FIFO;
    if(fifo)  return RB_Get_Buffer(fifo);
    else  return 0;
}

/**
@功能: 获取FIFO大小
@参数: dev, 操作对象
       tr, TOR_TX/TOR_RX
@返回: FIFO大小
@备注: 此处FIFO大小指FIFO缓存的空间，非FIFO块大小
*/
u32  FW_UART_GetFIFOSize(FW_UART_Type *dev, u8 tr)
{
    RB_Type *fifo =(tr == TOR_TX) ? dev->Config.TX_FIFO : dev->Config.RX_FIFO;
    if(fifo)  return RB_Get_BufferSize(fifo);
    else  return 0;
}
/**/

/**
@功能: 获取FIFO中的有效数据长度
@参数: dev, 操作对象
       tr, TOR_TX/TOR_RX
@返回: FIFO中的有效数据长度
@备注: 无
*/
u32  FW_UART_GetDataLength(FW_UART_Type *dev, u8 tr)
{
    RB_Type *fifo = (tr == TOR_TX) ? dev->Config.TX_FIFO : dev->Config.RX_FIFO;
    if(fifo)  return RB_Get_DataLength(fifo);
    else  return 0;
}
/**/

void FW_UART_ClearFIFO(FW_UART_Type *dev, u8 tr)
{
    RB_Type *fifo = (tr == TOR_TX) ? dev->Config.TX_FIFO : dev->Config.RX_FIFO;
    if(fifo)  RB_Clear_Buffer(fifo);
}
/**/

void FW_UART_FillFIFO(FW_UART_Type *dev, u8 value, u8 tr)
{
    RB_Type *fifo = (tr == TOR_TX) ? dev->Config.TX_FIFO : dev->Config.RX_FIFO;
    if(fifo)  RB_Fill_Buffer(fifo, value);
}
/**/

u32  FW_UART_Write(FW_UART_Type *dev, u32 offset, const u8 *pdata, u32 num)
{
    return dev->Config.Write(dev, offset, pdata, num);
}

u32  FW_UART_Read(FW_UART_Type *dev, u32 offset, u8 *pdata, u32 num)
{
    return dev->Config.Read(dev, offset, pdata, num);
}

u32  FW_UART_ReadAll(FW_UART_Type *dev, u8 *pdata, u32 cntout)
{
    return RB_Read_All(dev->Config.RX_FIFO, pdata, cntout);
}

u32  FW_UART_ReadTo(FW_UART_Type *dev, const char *s, u8 *pdata, u32 cntout)
{
    return RB_Read_To(dev->Config.RX_FIFO, s, pdata, cntout);
}

u32  FW_UART_ReadLine(FW_UART_Type *dev, u8 *pdata, u32 cntout)
{
    return RB_Read_Line(dev->Config.RX_FIFO, pdata, cntout);
}

u32  FW_UART_ReadInter(FW_UART_Type *dev, const char *s1, const char *s2, u8 *pdata, u32 cntout)
{
    return RB_Read_Inter(dev->Config.RX_FIFO, s1, s2, pdata, cntout);
}

u32  FW_UART_ReadFirst(FW_UART_Type *dev, const char *s, u8 *pdata, u32 num)
{
    return RB_Read_Fisrt(dev->Config.RX_FIFO, s, pdata, num);
}

u32  FW_UART_ReadHL(FW_UART_Type *dev, const u8 head, u8 length_site, u8 other_num, u8 *pdata, u32 cntout)
{
    return RB_Read_HL(dev->Config.RX_FIFO, head, length_site, other_num, pdata, cntout);
}

char *FW_UART_FindString(FW_UART_Type *dev, const char *s)
{
    return RB_Find_String(dev->Config.RX_FIFO, s);
}

Bool FW_UART_StringIsExist(FW_UART_Type *dev, const char *s)
{
    return RB_String_IsExist(dev->Config.RX_FIFO, s);
}

void FW_UART_RX_ISR(FW_UART_Type *dev)
{
    if(dev->Config.IH_RX)  dev->Config.IH_RX(dev);
}

void FW_UART_RC_ISR(FW_UART_Type *dev)
{
    if(dev->Config.IH_RC)  dev->Config.IH_RC(dev);
}

void FW_UART_TX_ISR(FW_UART_Type *dev)
{
    if(dev->Config.IH_TX)  dev->Config.IH_TX(dev);
}

void FW_UART_TC_ISR(FW_UART_Type *dev)
{
    if(dev->Config.IH_TC)  dev->Config.IH_TC(dev);
}


static FW_UART_Type PUART;
static FW_UART_Type *Printer;

/**
@功能: printf格式化函数
@参数: ch，发送的数据
       f, 文件
@返回: ch，不关心
@备注: @该函数为标准的printf函数提供一个格式化输出，通过定义输出端口，使printf
        函数能够输出数据
       @允许使用isr和dma方式发送
*/
__WEAK_ int fputc(int ch, FILE *f)
{
    u8 value = ch;
    FW_UART_Write(Printer, 0, &value, 1);
	return ch;
}
/**/

//void FW_UART_PrintInit(FW_UART_Type *dev)
//{
//    Printer = dev;
//}
///**/

//void FW_UART_SetPrint(FW_UART_Type *dev)
//{
//    Printer = dev;
//}

void UART_PrintInit(FW_UART_Type *dev, u16 pin, u32 baudrate)
{
    FW_UART_Driver_Type *drv;
    
    if(dev != NULL)
    {
        Printer = dev;
        return;
    }
    
    drv = FW_Driver_Find("io->uart");
    
    dev = &PUART;
    
    FW_Device_SetDriver(dev, drv);
    
    dev->Baudrate = baudrate;
    dev->TX_Pin = pin;
    
    FW_UART_Init(dev);
    
    Printer = dev;
}




/**
@功能: 接收中断任务处理
@参数: dev, 操作对象
@返回: 无
@备注: 无
*/
static void FW_UART_IH_RX(void *dev)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(uart);
    RB_Type *fifo = uart->Config.RX_FIFO;
    u8 value;
    
    FW_Lock();
    if(fifo->Size)
    {
        value = drv->RX_Byte(uart);
        RB_Write(fifo, &value, sizeof(u8));
    }
    FW_Unlock();
}

/**
@功能: 接收完成中断任务处理
@参数: dev, 操作对象
@返回: 无
@备注: 通过接收完成中断与DMA方式来实现不定长数据帧的接收
*/
static void FW_UART_IH_RC(void *dev)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(uart);
    RB_Type *fifo = uart->Config.RX_FIFO;
    SGD_Type *dma = uart->Config.RX_DMA;
    u32 num;
    
    FW_Lock();
    drv->RX_CTL(uart, Disable);
    num = drv->Get_RDL(uart);
    RB_Write(fifo, dma->Buffer, num);
    drv->RX_CTL(uart, Enable);
    FW_Unlock();
}
/**/

static void FW_UART_IH_TX(void *dev)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(uart);
    RB_Type *fifo = uart->Config.TX_FIFO;
    u8 value;
    
    FW_Lock();
    if(fifo->Size)
    {
        if(RB_Read(fifo, &value, sizeof(u8)) > 0)
        {
            /* 从缓存中读取一个字节发送出去 */
            drv->TX_Byte(uart, value);
        }
        else
        {
            /* 发送缓存中已经没有数据，暂时关闭发送中断 */
            drv->TX_CTL(uart, Disable);
        }
    }
    FW_Unlock();
}

static void FW_UART_IH_TC(void *dev)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(uart);
    RB_Type *fifo = uart->Config.TX_FIFO;
    SGD_Type *dma = uart->Config.TX_DMA;
    u32 num;

    FW_Lock();
    if(dma->Size)
    {
        num = RB_Get_DataLength(fifo);
        if(num)
        {
            if(num > dma->Size)
            {
                num = dma->Size;
            }
            
            /* DMA暂停，但TX_EN = True */
            drv->TX_CTL(uart, Disable);
            
            /* 将数据从发送缓存读取到DMA缓存 */
            RB_Read(fifo, dma->Buffer, num);
            
            /* 设置DMA发送数据长度 */
            drv->Set_TDL(uart, num);
            
            /* 开启DMA发送 */
            drv->TX_CTL(uart, Enable);
        }
        else
        {
            /* 发送缓存中的数据已经发送完成，暂时关闭DMA发送 */
            drv->TX_CTL(uart, Disable);
        }
    }
    FW_Unlock();
}

__INLINE_STATIC_ u8 FW_UART_WaitNull(FW_UART_Type *dev){return True;}

static u32  FW_UART_WritePOL(void *dev, u32 offset, const void *pdata, u32 num)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(uart);
    
    u8 *p = (u8 *)pdata;
    u8 (*pf)(FW_UART_Type *) = FW_UART_WaitNull;
    
    if(drv->Wait_TC)  pf = drv->Wait_TC;
    
    while(num--)
    {
        drv->TX_Byte(uart, *p++);
        while(pf(uart) != True);
    }
    
    return num;
}

static u32  FW_UART_WriteINT(void *dev, u32 offset, const void *pdata, u32 num)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(uart);
    RB_Type *fifo = uart->Config.TX_FIFO;

    FW_Lock();
    
    /* 没有发送数据或发送缓存空间不足 */
    if(num == 0 ||
       num > fifo->Size - RB_Get_DataLength(fifo))
    {
        FW_Unlock();
        return 0;
    }
    
    /* 将发送数据填入发送缓存 */
    RB_Write(fifo, pdata, num);
    
    FW_Unlock();
    
    /* 开启发送中断 */
    drv->TX_CTL(uart, Enable);
    
    return num;
}

static u32  FW_UART_WriteDMA(void *dev, u32 offset, const void *pdata, u32 num)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(uart);
    RB_Type *fifo = uart->Config.TX_FIFO;
    SGD_Type *dma = uart->Config.TX_DMA;
    
    FW_Lock();
    
    /* 没有数据或发送缓存空间不足 */
    if(num == 0 ||
       num > fifo->Size - RB_Get_DataLength(fifo))
    {
        FW_Unlock();
        return 0;
    }
    
    /* 先将发送数据写入发送缓存 */
    RB_Write(fifo, pdata, num);
    
    if(uart->Config.TX_EN == False)
    {
        /* 先发送一个字节，唤起发送完成中断，剩余数据在TC中断中进行发送 */
        RB_Read(fifo, dma->Buffer, sizeof(u8));
        drv->TX_CTL(uart, Disable);
        drv->Set_TDL(uart, sizeof(u8));
        drv->TX_CTL(uart, Enable);
        while(drv->Wait_TC(uart) != True);
    }
    
    FW_Unlock();
    
    return num;
}

static u32  FW_UART_ReadPOL(void *dev, u32 offset, void *pdata, u32 num)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    FW_UART_Driver_Type *drv = FW_Device_GetDriver(uart);
    
    u8 *p = (u8 *)pdata;
    u8 (*pf)(FW_UART_Type *) = FW_UART_WaitNull;
    u32 i;
    
    if(drv->Wait_RC)  pf = drv->Wait_RC;
    
    for(i = 0; i < num; i++)
    {
        while(pf(uart) != True);
        *p++ = drv->RX_Byte(uart);
    }
    
    return num;
}

static u32  FW_UART_ReadINT(void *dev, u32 offset, void *pdata, u32 num)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    return RB_Read(uart->Config.RX_FIFO, pdata, num);
}

static u32  FW_UART_ReadDMA(void *dev, u32 offset, void *pdata, u32 num)
{
    FW_UART_Type *uart = (FW_UART_Type *)dev;
    return RB_Read(uart->Config.RX_FIFO, pdata, num);
}

