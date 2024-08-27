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
 
#ifndef _SERIAL_H_
#define _SERIAL_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_define.h"
#include "rbuffer.h"


typedef struct
{
    /* DMA缓存 */
    u8  *Buffer;
    
    /* DMA缓存大小 */
    u32 Size;
}SGD_Type;  /* serial general dma type */


typedef struct
{
    /* 主从设备标志 */
    u32 MS_Flag : 1;
    
    /* default buffer size, 指定为dma或中断通讯方式时，未明确缓存大小，则使用该值
       该值若初始为0，则设置为DB_SIZE */
    u32 DB_Size : 25;
    
    /* 通讯方式：pol int dma */
    u32 TX_Mode : 2;
    u32 RX_Mode : 2;
    
    /* TX,RX控制 */
    u32 TX_EN : 1;
    u32 RX_EN : 1;
    
    /* dma */
//    u32 DMA_TX_Buffer_Size : 16;
//    u32 DMA_RX_Buffer_Size : 16;
    u32 TX_DB_Size : 16;
    u32 RX_DB_Size : 16;
    SGD_Type *TX_DMA;
    SGD_Type *RX_DMA;
    
    /* fifo */
//    u32 FIFO_TX_Buffer_Size : 16;               //对应FIFO的Block_Num
//    u32 FIFO_RX_Buffer_Size : 16;
//    FIFO_Type *TX_FIFO;
//    FIFO_Type *RX_FIFO;
    u32 TX_RB_Size : 16;
    u32 RX_RB_Size : 16;
    RB_Type *TX_FIFO;
    RB_Type *RX_FIFO;
    
    /* interrupt handler */
    void (*IH_TX)(void *dev);                   //发送
    void (*IH_TC)(void *dev);                   //发送完成
    void (*IH_RX)(void *dev);                   //接收
    void (*IH_RC)(void *dev);                   //接收完成
    
    u32 (*Write)(void *dev, u32 offset, const void *pdata, u32 num);
    u32 (*Read)(void *dev, u32 offset, void *pdata, u32 num);
}SGC_Type;  /* serial general config type */


typedef struct
{
    SGC_Type Config;
    
    void (*IH_TX)(void *dev);
    void (*IH_TC)(void *dev);
    void (*IH_RX)(void *dev);
    void (*IH_RC)(void *dev);
    
    u32  (*Write_POL)(void *dev, u32 offset, const void *pdata, u32 num);
    u32  (*Write_INT)(void *dev, u32 offset, const void *pdata, u32 num);
    u32  (*Write_DMA)(void *dev, u32 offset, const void *pdata, u32 num);
    
    u32  (*Read_POL)(void *dev, u32 offset, void *pdata, u32 num);
    u32  (*Read_INT)(void *dev, u32 offset, void *pdata, u32 num);
    u32  (*Read_DMA)(void *dev, u32 offset, void *pdata, u32 num);
}SGTR_Type;  /* serial general transfer & receive type */


void SGTR_DeInit(SGTR_Type *sgtr);
void SGTR_Config(SGTR_Type *sgtr);


#ifdef __cplusplus
}
#endif

#endif

