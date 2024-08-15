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
#ifndef _FW_CAN_H_
#define _FW_CAN_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"
#include "serial.h"


#define FW_CAN_BAUDRATE_MAX            1000000
#define FW_CAN_BAUDRATE_MIN            1000


typedef enum
{
    FW_CAN_Mode_Normal,
    FW_CAN_Mode_LoopBack,
    FW_CAN_Mode_Silent,
    FW_CAN_Mode_Silent_LoopBack,
}FW_CAN_Mode_Enum;


typedef enum
{
    FW_CAN_Filter_List,
    FW_CAN_Filter_Mask,
}FW_CAN_Filter_Enum;


typedef enum
{
    FW_CAN_FT_Data,
    FW_CAN_FT_Remote,
}FW_CAN_FT_Enum;

typedef enum
{
    FW_CAN_FF_STD,
    FW_CAN_FF_EXT,
}FW_CAN_FF_Enum;


typedef enum
{
    FW_CAN_TX_State_OK,
    FW_CAN_TX_State_Failed,
    FW_CAN_TX_State_Pending,
    FW_CAN_TX_State_NoMailbox,
    FW_CAN_TX_State_Unknown,
}FW_CAN_TX_State_Enum;


//typedef struct
//{
//    u32 ID;
//    u32 Type : 1;
//    u32 Format : 1;
//    u32 Length : 4;
//    u32 : 26;
//}FW_CAN_FI_Type;


typedef struct
{
//    u32 STD_ID;                   //
//    u32 EXT_ID;                   //
    u32 ID;                       //帧ID
    u32 Type : 1;                 //帧类型，数据帧/远程帧
    u32 Format : 1;               //标识符，标准标识符/标准标识符+扩展标识符
    u32 Length : 4;               //数据长度0~8
    u32 : 26;
    u8  Data[8];
}FW_CAN_Frame_Type;


//typedef struct
//{
////    FW_CAN_Frame_Type *Buffer;
//    RB_Type FIFO;
//    FW_SList_Type List;
//}FW_CAN_Node_Type;


typedef struct
{
    FW_Device_Type Device;
    
    SGC_Type Config;
    
    u32 TX_Pin : 16;
    u32 RX_Pin : 16;
    
    u32 ID;
    
    u32 Baudrate : 20;            //波特率，Bmax = 1Mbps
    
    u32 Mode : 2;                 //工作模式(正常、回环、静默、静默回环)
    u32 Fiter : 1;                //接收过滤方式(列表、掩码)
    
    u32 Mailbox_Num : 8;          //邮箱编号
    
    u32 : 1;
//    u32 Mult_Node_EN : 1;         //多节点模式使能位
//    u32 Node_Buffer_Size : 8;     //CAN节点接收缓存大小
    
    __RO_ void *CANx;             //
}FW_CAN_Type;


typedef struct
{
    void (*DeInit)(FW_CAN_Type *dev);
    void (*Init)(FW_CAN_Type *dev);
    
    void (*CTL)(FW_CAN_Type *dev, u8 state);
    void (*TX_CTL)(FW_CAN_Type *dev, u8 state);
    void (*RX_CTL)(FW_CAN_Type *dev, u8 state);
    
    u8   (*Wait_TC)(FW_CAN_Type *dev);
    u8   (*Wait_RC)(FW_CAN_Type *dev);
    
    u8   (*Frame_Write)(FW_CAN_Type *dev, FW_CAN_Frame_Type *msg);
    u8   (*Frame_Read)(FW_CAN_Type *dev, FW_CAN_Frame_Type *msg);
    
    u32  (*Write)(FW_CAN_Type *dev, const u8 *pdata, u32 num);
    u32  (*Read)(FW_CAN_Type *dev, u8 *pdata, u32 num);
}FW_CAN_Driver_Type;


void FW_CAN_SetPort(FW_CAN_Type *dev, void *can);
void *FW_CAN_GetPort(FW_CAN_Type *dev);

u8   FW_CAN_GetTRM(FW_CAN_Type *dev, u8 tr);
void FW_CAN_SetTRM(FW_CAN_Type *dev, u8 tr, u8 mode);

void FW_CAN_DeInit(FW_CAN_Type *dev);
void FW_CAN_Init(FW_CAN_Type *dev);

void FW_CAN_CTL(FW_CAN_Type *dev, u8 state);
void FW_CAN_TXCTL(FW_CAN_Type *dev, u8 state);
void FW_CAN_RXCTL(FW_CAN_Type *dev, u8 state);

u32  FW_CAN_Write(FW_CAN_Type *dev, FW_CAN_Frame_Type *frame, const u8 *pdata, u32 num);
u32  FW_CAN_Read(FW_CAN_Type *dev, FW_CAN_Frame_Type *frame, u8 *pdata, u32 num);

void FW_CAN_RX_ISR(FW_CAN_Type *dev);
void FW_CAN_TX_ISR(FW_CAN_Type *dev);


#ifdef __cplusplus
}
#endif

#endif

