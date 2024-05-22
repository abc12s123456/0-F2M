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
 
#include "serial.h"
#include "mm.h"
#include "fw_print.h"

#include "lib_string.h"


/* 默认的缓存大小，单位：byte */
#define DB_SIZE         32


/**
@功能: 通讯传输结构体缺省初始化
@参数: Type, 操作对象
@返回: 无
@备注: @将操作对象配置成已知状态
*/
void SGTR_DeInit(SGTR_Type *sgtr)
{
    memset(sgtr, 0, sizeof(SGTR_Type));
}
/**/

/**
@功能: 通讯传输结构体初始化
@参数: Type, 操作对象
@返回: 无
@备注: @适用于UART, SPI, I2C等串口通讯
       @未定义传输模式及缓存时，默认轮询收发
*/
void SGTR_Config(SGTR_Type *sgtr)
{
    SGC_Type *cfg = &sgtr->Config;
    struct
    {
        u8 dma : 1;
        u8 db : 1;
        u8 fifo : 1;
        u8 fb : 1;
        u8 : 4;
    }mm_flag;                                    //内存申请标志
    
    if(cfg->DB_Size == 0)  cfg->DB_Size = DB_SIZE;
    
    /* TX */
    memset(&mm_flag, 0, sizeof(mm_flag));
    
    /* 未指定发送方式时，根据缓存及大小设置发送模式；优先使用DMA */
    if(cfg->TX_Mode == TRM_UNK)
    {
        if(cfg->TX_DMA && cfg->TX_DMA->Size)
        {
            cfg->TX_Mode = TRM_DMA;
        }
        else if(cfg->TX_FIFO && cfg->TX_FIFO->Size)
        {
            cfg->TX_Mode = TRM_INT;
        }
        else 
        {
            goto POL_TX;
        }
    }
    
    if(cfg->TX_Mode == TRM_DMA)
    {
        if(cfg->TX_DMA == NULL)
        {
            cfg->TX_DMA = (SGD_Type *)MM_Malloc(sizeof(SGD_Type));
            if(cfg->TX_DMA == NULL)
            {
                LOG_E("发送DMA结构申请失败\r\n");
                goto POL_TX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->TX_DMA, 0, sizeof(SGD_Type));
            
            mm_flag.dma = True;
        }
        
        if(cfg->TX_DMA->Size == 0)  
        {
            if(cfg->TX_DB_Size)
            {
                cfg->TX_DMA->Size = cfg->TX_DB_Size;
            }
            else
            {
                cfg->TX_DMA->Size = cfg->DB_Size;
            }
        }
        
        if(cfg->TX_DMA->Buffer == NULL)
        {
            cfg->TX_DMA->Buffer = (u8 *)MM_Malloc(cfg->TX_DMA->Size);
            if(cfg->TX_DMA->Buffer == NULL)
            {
                LOG_E("发送DMA缓存申请失败\r\n");
                if(mm_flag.dma)  MM_Free(cfg->TX_DMA);
                goto POL_TX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->TX_DMA->Buffer, 0, cfg->TX_DMA->Size);
            
            mm_flag.db = True;
        }
        
        /* 使用DMA时，未指定发送FIFO及大小 */
        if(cfg->TX_FIFO == NULL)
        {
            cfg->TX_FIFO = (RB_Type *)MM_Malloc(sizeof(RB_Type));
            if(cfg->TX_FIFO == NULL)
            {
                LOG_E("发送FIFO结构申请失败\r\n");
                goto POL_TX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->TX_FIFO, 0, sizeof(RB_Type));
            
            mm_flag.fifo = True;
        }
        
        /* FIFO未指定缓存大小时，默认为DMA缓存大小的2倍 */
        if(cfg->TX_FIFO->Size == 0)
        {
            if(cfg->TX_RB_Size)
            {
                cfg->TX_FIFO->Size = cfg->TX_RB_Size;
            }
            else
            {
                cfg->TX_FIFO->Size = cfg->TX_DMA->Size * 2;
            }
        }
        
        if(cfg->TX_FIFO->Buffer == NULL)
        {
            cfg->TX_FIFO->Buffer = (u8 *)MM_Malloc(cfg->TX_FIFO->Size);
            if(cfg->TX_FIFO->Buffer == NULL)
            {
                LOG_E("发送FIFO缓存申请失败\r\n");
                goto POL_TX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->TX_FIFO->Buffer, 0, cfg->TX_FIFO->Size);
            
            mm_flag.fb = True;
        }
        
        /* DMA发送时，若用户未定义，则使用默认的发送完成中断处理函数 */
        if(cfg->IH_TC == NULL)  cfg->IH_TC = sgtr->IH_TC;
        
        cfg->Write = sgtr->Write_DMA;
    }
    
    else if(cfg->TX_Mode == TRM_INT)
    {
        if(cfg->TX_FIFO == NULL)
        {
            cfg->TX_FIFO = (RB_Type *)MM_Malloc(sizeof(RB_Type));
            if(cfg->TX_FIFO == NULL)
            {
                LOG_E("发送FIFO结构申请失败\r\n");
                goto POL_TX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->TX_FIFO, 0, sizeof(RB_Type));
            
            mm_flag.fifo = True;
        }
        
        /* 使用INT方式发送，FIFO未指定缓存大小时，默认为cfg->DB_Size */
        if(cfg->TX_FIFO->Size == 0)
        {
            if(cfg->TX_RB_Size)
            {
                cfg->TX_FIFO->Size = cfg->TX_RB_Size;
            }
            else
            {
                cfg->TX_FIFO->Size = cfg->DB_Size;
            }
        }
        
        if(cfg->TX_FIFO->Buffer == NULL)
        {
            cfg->TX_FIFO->Buffer = (u8 *)MM_Malloc(cfg->TX_FIFO->Size);
            if(cfg->TX_FIFO->Buffer == NULL)
            {
                LOG_E("发送FIFO缓存申请失败\r\n");
                goto POL_TX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->TX_FIFO->Buffer, 0, cfg->TX_FIFO->Size);
            
            mm_flag.fb = True;
        }
        
        /* 中断发送时，若用户未定义，则使用默认的发送中断处理函数 */
        if(cfg->IH_TX == NULL)  cfg->IH_TX = sgtr->IH_TX;
        
        cfg->Write = sgtr->Write_INT;
    }
    else
    {
        POL_TX:
        if(mm_flag.dma)  MM_Free(cfg->TX_DMA);
        if(mm_flag.db)   MM_Free(cfg->TX_DMA->Buffer);
        if(mm_flag.fifo) MM_Free(cfg->TX_FIFO);
        if(mm_flag.fb)   MM_Free(cfg->TX_FIFO->Buffer);
        
        cfg->TX_Mode = TRM_POL;
        cfg->Write = sgtr->Write_POL;
    }
    
    /* RX */
    memset(&mm_flag, 0, sizeof(mm_flag));
    
    /* 未指定接收方式时，根据缓存及大小设置接收模式，优先使用DMA */
    if(cfg->RX_Mode == TRM_UNK)
    {
        if(cfg->RX_DMA && cfg->RX_DMA->Size)
        {
            cfg->RX_Mode = TRM_DMA;
        }
        else if(cfg->RX_FIFO && cfg->RX_FIFO->Size)
        {
            cfg->RX_Mode = TRM_INT;
        }
        else  
        {
            goto POL_RX;
        }
    }
    
    if(cfg->RX_Mode == TRM_DMA)
    {
        if(cfg->RX_DMA == NULL)
        {
            cfg->RX_DMA = (SGD_Type *)MM_Malloc(sizeof(SGD_Type));
            if(cfg->RX_DMA == NULL)
            {
                LOG_E("接收DMA结构申请失败\r\n");
                goto POL_RX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->RX_DMA, 0, sizeof(SGD_Type));
            
            mm_flag.dma = True;
        }
        
        if(cfg->RX_DMA->Size == 0)
        {
            if(cfg->RX_DB_Size)
            {
                cfg->RX_DMA->Size = cfg->RX_DB_Size;
            }
            else
            {
                cfg->RX_DMA->Size = cfg->DB_Size;
            }
        }
        
        if(cfg->RX_DMA->Buffer == NULL)
        {
            cfg->RX_DMA->Buffer = (u8 *)MM_Malloc(cfg->RX_DMA->Size);
            if(cfg->RX_DMA->Buffer == NULL)
            {
                LOG_E("接收DMA缓存申请失败\r\n");
                if(mm_flag.dma)  MM_Free(cfg->RX_DMA);
                goto POL_RX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->RX_DMA->Buffer, 0, cfg->RX_DMA->Size);
            
            mm_flag.db = True;
        }
        
        /* 使用DMA时，未指定发送FIFO及大小 */
        if(cfg->RX_FIFO == NULL)
        {
            cfg->RX_FIFO = (RB_Type *)MM_Malloc(sizeof(RB_Type));
            if(cfg->RX_FIFO == NULL)
            {
                LOG_E("接收FIFO结构申请失败\r\n");
                goto POL_RX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->RX_FIFO, 0, sizeof(RB_Type));
            
            mm_flag.fifo = True;
        }
        
        if(cfg->RX_FIFO->Size == 0)
        {
            if(cfg->RX_RB_Size)
            {
                cfg->RX_FIFO->Size = cfg->RX_RB_Size;
            }
            else
            {
                cfg->RX_FIFO->Size = cfg->RX_DMA->Size * 2;
            }
        }
        
        if(cfg->RX_FIFO->Buffer == NULL)
        {
            cfg->RX_FIFO->Buffer = (u8 *)MM_Malloc(cfg->RX_FIFO->Size);
            if(cfg->RX_FIFO->Buffer == NULL)
            {
                LOG_E("接收FIFO缓存申请失败\r\n");
                goto POL_RX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->RX_FIFO->Buffer, 0, cfg->RX_FIFO->Size);
            
            mm_flag.fb = True;
        }
        
        /* DMA接收时，若用户未定义，则使用默认的接收完成中断处理函数 */
        if(cfg->IH_RC == NULL)  cfg->IH_RC = sgtr->IH_RC;
        
        cfg->Read = sgtr->Read_DMA;
    }
    
    else if(cfg->RX_Mode == TRM_INT)
    {
        if(cfg->RX_FIFO == NULL)
        {
            cfg->RX_FIFO = (RB_Type *)MM_Malloc(sizeof(RB_Type));
            if(cfg->RX_FIFO == NULL)
            {
                LOG_E("接收FIFO结构申请失败\r\n");
                goto POL_RX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->RX_FIFO, 0, sizeof(RB_Type));
            
            mm_flag.fifo = True;
        }
        
        /* 20230920 */
        if(cfg->RX_FIFO->Size == 0)
        {
            if(cfg->RX_RB_Size)
            {
                cfg->RX_FIFO->Size = cfg->RX_RB_Size;
            }
            else
            {
                cfg->RX_FIFO->Size = cfg->DB_Size;
            }
        }

        if(cfg->RX_FIFO->Buffer == NULL)
        {
            cfg->RX_FIFO->Buffer = (u8 *)MM_Malloc(cfg->RX_FIFO->Size);
            if(cfg->RX_FIFO->Buffer == NULL)
            {
                LOG_E("接收FIFO缓存申请失败\r\n");
                goto POL_RX;
            }
            
            /* 对申请的空间进行清0，防止脏数据 */
            memset(cfg->RX_FIFO->Buffer, 0, cfg->RX_FIFO->Size);
            
            mm_flag.fb = True;
        }
        
        /* 中断接收时，若用户未定义，则使用默认的接收中断处理函数 */
        if(cfg->IH_RX == NULL)  cfg->IH_RX = sgtr->IH_RX;
        
        cfg->Read = sgtr->Read_INT;
    }
    else
    {
        POL_RX:
        if(mm_flag.dma)  MM_Free(cfg->RX_DMA);
        if(mm_flag.db)   MM_Free(cfg->RX_DMA->Buffer);
        if(mm_flag.fifo) MM_Free(cfg->RX_FIFO);
        if(mm_flag.fb)   MM_Free(cfg->RX_FIFO->Buffer);
        
        cfg->RX_Mode = TRM_POL;
        cfg->Read = sgtr->Read_POL;
    }
}
/**/

