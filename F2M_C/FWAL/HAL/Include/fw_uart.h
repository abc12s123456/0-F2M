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
#ifndef _FW_UART_H_
#define _FW_UART_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
#include "serial.h"


/*
F2M框架中，UART的数据位遵循PC端的定义，即数据位不包含校验位
*/

/**
 * UART驱动编号
 * 外部扩展可以有多个不同的驱动
 */
#define UART_DRV_NAT_NUM          0x00      //原生UART
#define UART_DRV_ISM_NUM          0x01      //IO模拟UART
#define UART_DRV_EXT_NUM          0x02      //外部扩展UART


typedef enum
{
    FW_UART_DataBits_5 = 5,
    FW_UART_DataBits_6,
    FW_UART_DataBits_7,
    FW_UART_DataBits_8,
}FW_UART_DataBits_Enum;


typedef enum
{
    FW_UART_StopBits_0_5 = 1,
    FW_UART_StopBits_1,
    FW_UART_StopBits_1_5,
    FW_UART_StopBits_2,
}FW_UART_StopBits_Enum;


typedef enum
{
    FW_UART_Parity_None,              /* 无校验 */
    FW_UART_Parity_Odd,               /* 奇校验 */
    FW_UART_Parity_Even,              /* 偶校验 */
}FW_UART_Parity_Enum;


typedef struct FW_UART  FW_UART_Type;


typedef struct FW_UART_Driver
{
//    u32  Driver_ID;
//    
//    void (*Set_UARTx)(FW_UART_Type *dev);
    
    void (*DeInit)(FW_UART_Type *dev);
    void (*Init)(FW_UART_Type *dev);
    
    void (*CTL)(FW_UART_Type *dev, u8 state);
    void (*TX_CTL)(FW_UART_Type *dev, u8 state);
    void (*RX_CTL)(FW_UART_Type *dev, u8 state);
    
    void (*TX_Byte)(FW_UART_Type *dev, u8 value);
    u8   (*RX_Byte)(FW_UART_Type *dev);
    
    u8   (*Wait_TC)(FW_UART_Type *dev);
    u8   (*Wait_RC)(FW_UART_Type *dev);
    
    void (*Set_TDL)(FW_UART_Type *dev, u32 length);
    u32  (*Get_RDL)(FW_UART_Type *dev);
}FW_UART_Driver_Type;


/* UART Device */
struct FW_UART
{
    FW_Device_Type Parent;
    
    /* 串口通讯通用配置 */
    SGC_Type Config;
    
    /* IO */
    u32 TX_Pin : 16;
    u32 RX_Pin : 16;
    
    /* 收发缓存大小 */
//    u16 FIFO_TX_Buffer_Size;
//    u16 FIFO_RX_Buffer_Size;
    u32 TX_RB_Size : 16;
    u32 RX_RB_Size : 16;
    
    /* DMA缓存大小 */
//    u16 DMA_TX_Buffer_Size;
//    u16 DMA_RX_Buffer_Size;
    u32 TX_DB_Size : 16;
    u32 RX_DB_Size : 16;
    
    /* 波特率 */
    u32 Baudrate;
    
    /* 数据帧格式 */
    u32 Data_Bits : 4;
    u32 Stop_Bits : 4;
    u32 Parity :4;
    
    /* 模拟UART的延时时间，单位：us */
    u32 SDTime : 16;
    u32 : 4;
    
    /* 底层端口 */
    __RO_ void *UARTx;
};


void FW_UART_SetPort(FW_UART_Type *dev, void *uart);
void *FW_UART_GetPort(FW_UART_Type *dev);

u8   FW_UART_GetTRM(FW_UART_Type *dev, u8 tr);
void *FW_UART_GetDMABBase(FW_UART_Type *dev, u8 tr);
u32  FW_UART_GetDMABSize(FW_UART_Type *dev, u8 tr);

void FW_UART_DeInit(FW_UART_Type *dev);
void FW_UART_Init(FW_UART_Type *dev);

void FW_UART_CTL(FW_UART_Type *dev, u8 state);
void FW_UART_TXCTL(FW_UART_Type *dev, u8 state);
void FW_UART_RXCTL(FW_UART_Type *dev, u8 state);

u8  *FW_UART_GetFIFOBBase(FW_UART_Type *dev, u8 tr);
u32  FW_UART_GetFIFOSize(FW_UART_Type *dev, u8 tr);
u32  FW_UART_GetDataLength(FW_UART_Type *dev, u8 tr);
void FW_UART_ClearFIFO(FW_UART_Type *dev, u8 tr);
void FW_UART_FillFIFO(FW_UART_Type *dev, u8 value, u8 tr);

u32  FW_UART_Write(FW_UART_Type *dev, u32 offset, const u8 *pdata, u32 num);

u32  FW_UART_Read(FW_UART_Type *dev, u32 offset, u8 *pdata, u32 num);
u32  FW_UART_ReadAll(FW_UART_Type *dev, u8 *pdata, u32 cntout);
u32  FW_UART_ReadTo(FW_UART_Type *dev, const char *s, u8 *pdata, u32 cntout);
u32  FW_UART_ReadLine(FW_UART_Type *dev, u8 *pdata, u32 cntout);
u32  FW_UART_ReadInter(FW_UART_Type *dev, const char *s1, const char *s2, u8 *pdata, u32 cntout);
u32  FW_UART_ReadFirst(FW_UART_Type *dev, const char *s, u8 *pdata, u32 num);
u32  FW_UART_ReadHL(FW_UART_Type *dev, const u8 head, u8 length_site, u8 other_num, u8 *pdata, u32 cntout);

char *FW_UART_FindString(FW_UART_Type *dev, const char *s);
Bool FW_UART_StringIsExist(FW_UART_Type *dev, const char *s);

void FW_UART_RX_ISR(FW_UART_Type *dev);
void FW_UART_RC_ISR(FW_UART_Type *dev);
void FW_UART_TX_ISR(FW_UART_Type *dev);
void FW_UART_TC_ISR(FW_UART_Type *dev);

//void FW_UART_PrintInit(FW_UART_Type *dev);
//void FW_UART_SetPrint(FW_UART_Type *dev);

void UART_PrintInit(FW_UART_Type *dev, u16 pin, u32 baudrate);


/*
 * @Print输出端口配置
 * FW_UART_PrintInit(UARTx)
 * FW_UART_PrintInit(PA0, 9600)
 */
#define FW_UART_PrintInit(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 1)  UART_PrintInit((FW_UART_Type *)VA0(__VA_ARGS__), NULL, NULL);\
    else if(n == 2)  UART_PrintInit(NULL, (u32)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)


#ifdef __cplusplus
}
#endif

#endif

