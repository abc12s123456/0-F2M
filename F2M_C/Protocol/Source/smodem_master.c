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
#include "smodem_master.h"

#include "libc.h"


#define SOH                  0x01              //标题开始
#define STX                  0x02              //正文开始
#define EOT                  0x04              //传输结束
#define ACK                  0x06              //收到通知
#define NAK                  0x15              //拒绝接收
#define CAN                  0x18              //取消传输，发送方连发5个CAN取消，接收方连发2个CAN取消
#define C                    0x43              //字符C

#define ABORT1               0x41              //
#define ABORT2               0x61

#define FILE_NAME_LEN        117               //最大文件名长度，包含末尾的‘\0’              
#define FILE_SIZE_LEN        11                //最大4G，包含末尾的‘\0’

#define NON_DATA_LEN         5                 //非数据长度

#define SEND_TIMEOUT         (60 * 1000)
#define RECV_TIMEOUT         (1 * 1000)        //接收超时时间，单位：ms
#define TIMEOUT_CNT_MAX      60                //发送的超时时间
#define ERR_CNT_MAX          10
#define RESEND_CNT_MAX       10


#define YMODEM_SOH_LEN       (128 + NON_DATA_LEN)
#define YMODEM_STX_LEN       (1024 + NON_DATA_LEN)


typedef enum
{
    Smodem_State_Cancel     = -5,                 //接收方取消
    Smodem_State_ResendErr  = -4,                 //重发超限
    Smodem_State_TimeoutErr = -3,                 //接收超时
    Smodem_State_FileErr    = -2,                 //文件读写出错
    Smodem_State_DataErr    = -1,                 //数据错误
    
    Smodem_State_OK = 0,                          //协议执行完成
    Smodem_State_Start = 1,                       //处理起始帧
    Smodem_State_Data,                            //处理数据帧
    Smodem_State_EOT,                             //
    Smodem_State_End,                             //处理结束帧
}Smodem_State_Enum;


__INLINE_STATIC_ u16 CRC16_Get(u8 *pdata, u32 num)
{
    const u16 crc_tab[] =
    {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 
        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF, 
        0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 
        0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE, 
        0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 
        0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D, 
        0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 
        0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC, 
        0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 
        0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, 
        0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 
        0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 
        0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 
        0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49, 
        0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 
        0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78, 
        0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 
        0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067, 
        0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 
        0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, 
        0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 
        0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
        0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 
        0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634, 
        0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 
        0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3, 
        0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 
        0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92, 
        0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 
        0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1, 
        0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 
        0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0, 
    };
    u16 crc = 0;
    u8 tmp;
    
    pdata += 3;
    num -= NON_DATA_LEN;
    
    while(num--)
    {
        tmp = crc >> 8;
        crc = (crc << 8) ^ crc_tab[(tmp ^ *pdata++) & 0xFF];
    }
    
    return crc;
}

static u8   Itoa(u32 value, char *pdata)
{
    const char alpha_tab[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    u8 len = 0;
    u32 tmp = value;
    u8 i, j;
    
    do
    {
        pdata[len++] = alpha_tab[tmp % 10];
        tmp /= 10;
    }while(tmp);
    
    j = len >> 1;
    
    for(i = 0; i < j; i++)                             //将转换后的字符串倒序(不包含"-")
    {
        pdata[i]       = pdata[i] ^ pdata[len - i];
        pdata[len - i] = pdata[i] ^ pdata[len - i];
        pdata[i]       = pdata[i] ^ pdata[len - i];
    }
    
    pdata[len] = '\0';
    
    return (len + 1);
}

static void Smodem_TX_Byte(Smodem_Master_Type *smodem, u8 value)
{
    smodem->Packet_TX(&value, 1);
}

static void Smodem_TX_Packet(Smodem_Master_Type *smodem, u8 head, u8 packet_num)
{
    u8 *pdata = smodem->Packet_Buffer;
    u32 num;
    u16 crc;
    
    if(head == EOT)  
    {
        smodem->Packet_TX(&head, 1);
        return;
    }
    
    if(head == SOH)  num = YMODEM_SOH_LEN;
    else  num = smodem->Packet_Length;
    
    pdata[0] = head;
    pdata[1] = packet_num;
    pdata[2] = (u8)(packet_num ^ 0xFF);
    crc = CRC16_Get(pdata, num);
    pdata[num - 2] = (u8)(crc >> 8);
    pdata[num - 1] = (u8)(crc & 0xFF);
    
    smodem->Packet_TX(pdata, num);
}

static Bool Smodem_RX_Response(Smodem_Master_Type *smodem, u8 *c, u32 timeout)
{
    while(1)
    {
        if(smodem->Packet_RX(c, 1) != 0)  
        {
            smodem->Timeout_Count = 0;
            return True;
        }
        
        if(STiming_Start(&smodem->Timing, timeout) == True)
        {
            if(smodem->Timeout_Count++ > TIMEOUT_CNT_MAX)
            {
                return False;
            }
        }
    }
}

static void Smodem_Cancel(Smodem_Master_Type *smodem)
{
    /* 发送方发送5个CAN取消传输 */
    Smodem_TX_Byte(smodem, CAN);
    Smodem_TX_Byte(smodem, CAN);
    Smodem_TX_Byte(smodem, CAN);
    Smodem_TX_Byte(smodem, CAN);
    Smodem_TX_Byte(smodem, CAN);
}

__INLINE_STATIC_ Smodem_State_Enum Smodem_State(Smodem_Master_Type *smodem, Smodem_State_Enum state)
{
    smodem->State = state;
    return state;
}

static Smodem_State_Enum Smodem_Error(Smodem_Master_Type *smodem)
{
    if(smodem->Err_Count >= ERR_CNT_MAX)
    {
        Smodem_Cancel(smodem);
        return Smodem_State(smodem, Smodem_State_DataErr);
    }
    
    smodem->Err_Count++;
    if(smodem->Packet_Clear)  smodem->Packet_Clear();
    
    return (Smodem_State_Enum)smodem->State;
}

static Smodem_State_Enum Smodem_RTX_Packet(Smodem_Master_Type *smodem)
{
    u8 *pdata = smodem->Packet_Buffer;
    u32 num;
    
    if(smodem->Resend_Count++ >= RESEND_CNT_MAX)
    {
        return Smodem_State(smodem, Smodem_State_ResendErr);
    }
    
    if(pdata[0] == EOT)  num = 1;
    else  num = smodem->Packet_Length;
    
    smodem->Packet_TX(pdata, num);
    
    return (Smodem_State_Enum)smodem->State;
}

static Smodem_State_Enum Smodem_TX_File(Smodem_Master_Type *smodem)
{
    u8 *pdata = smodem->Packet_Buffer + 3;
    u32 len, offset;
    u8 head;
    
    if(smodem->File_Remain > 0)
    {
        if(smodem->File_Remain >= (smodem->Packet_Length - NON_DATA_LEN))
        {
            head = STX;
            len = smodem->Packet_Length - NON_DATA_LEN;
        }
        else if(smodem->File_Remain <= YMODEM_SOH_LEN - NON_DATA_LEN)
        {
            /* 剩余数据不超过128字节 */
            memset(smodem->Packet_Buffer, 0x1A, YMODEM_SOH_LEN);
            head = SOH;
            len = smodem->File_Remain;
        }
        else
        {
            memset(smodem->Packet_Buffer, 0x1A, smodem->Packet_Length);
            head = STX;
            len = smodem->File_Remain;
        }
        
        offset = smodem->File_Size - smodem->File_Remain;
        while(1)
        {
            if(smodem->File_Read(smodem->File_Ptr, offset, pdata, len) != 0)
            {
                smodem->Resend_Count = 0;
                smodem->Err_Count = 0;
                smodem->Timeout_Count = 0;
                
                smodem->Packet_Num++;
                Smodem_TX_Packet(smodem, head, smodem->Packet_Num);
                
                smodem->File_Remain -= len;
                
                return Smodem_State(smodem, Smodem_State_Data);
            }
            else
            {
                /* 连续多次文件读取失败 */
                if(smodem->Err_Count++ >= ERR_CNT_MAX)
                {
                    Smodem_Cancel(smodem);
                    return Smodem_State(smodem, Smodem_State_FileErr);
                }
            }
        }
    }
    else
    {
        /* 文件数据已发送完成 */
        Smodem_TX_Packet(smodem, EOT, 0);
        return Smodem_State(smodem, Smodem_State_EOT);
    }
}

void Smodem_Master_Init(Smodem_Master_Type *smodem, u32 packet_len)
{
    smodem->Packet_Length = packet_len + NON_DATA_LEN;
    
    smodem->Packet_Num = 0;
    
    smodem->Resend_Count = 0;
    smodem->Err_Count = 0;
    smodem->Timeout_Count = 0;
    
    smodem->State = Smodem_State_Start;
    
//    smodem->File_Num = 0;
    
    STiming_Init(&smodem->Timing, NULL);
}

s8   Smodem_Master_Handler(Smodem_Master_Type *smodem)
{
    u32 len;
    u8 resp;
    char asc_fsize[FILE_SIZE_LEN];
    
    /* Packet_Buffer作为数据缓存 */
    u8 *pdata = smodem->Packet_Buffer + 3;
    
    if(Smodem_RX_Response(smodem, &resp, RECV_TIMEOUT) == False)
    {
        Smodem_Cancel(smodem);
        return Smodem_State(smodem, Smodem_State_TimeoutErr);
    }
    
    if(resp == C)
    {
        /* 接收到第1个C */
        if(smodem->State == Smodem_State_Start)
        {
            /* 文件名 */
            len = strlen(smodem->File_Name);
            memcpy(pdata, smodem->File_Name, len);
            pdata[len] = '\0';
            
            /* 文件大小 */
            Itoa(smodem->File_Size, asc_fsize);
            strcpy((char *)pdata + len + 1, asc_fsize);
            smodem->File_Remain = smodem->File_Size;
            
            /* 起始帧 */
            smodem->Packet_Num = 0;
            Smodem_TX_Packet(smodem, SOH, 0x00);
            
            return smodem->State;
        }
        
        /* 接收到第2个C */
        else if(smodem->State == Smodem_State_Data)
        {
            /* 发送第一包数据 */
            return Smodem_TX_File(smodem);
        }
        
        /* 结束C */
        else if(smodem->State == Smodem_State_End)
        {
            /* 若需要继续发送新的文件，在此处理 */
            memset(pdata, '\0', smodem->Packet_Length - NON_DATA_LEN);
            Smodem_TX_Packet(smodem, SOH, 0x00);
            return smodem->State;
        }
        
        else
        {
            return Smodem_Error(smodem);
        }
    }
    
    else if(resp == ACK)
    {
        /* 第一个ACK */
        if(smodem->State == Smodem_State_Start)
        {
            return Smodem_State(smodem, Smodem_State_Data);
        }
        
        /* 数据包应答 */
        else if(smodem->State == Smodem_State_Data)
        {
            return Smodem_TX_File(smodem);
        }
        
        else if(smodem->State == Smodem_State_EOT)
        {
            return Smodem_State(smodem, Smodem_State_End);
        }
        
        else if(smodem->State == Smodem_State_End)
        {
            /* 协议执行完成 */
            smodem->Packet_Num = 0;
            
            smodem->Resend_Count = 0;
            smodem->Err_Count = 0;
            smodem->Timeout_Count = 0;
            
            return Smodem_State(smodem, Smodem_State_OK);
        }
        
        else
        {
            return Smodem_Error(smodem);
        }
    }
    
    else if(resp == NAK)
    {
        /* 接收方请求数据重发 */
        return Smodem_RTX_Packet(smodem);
    }
    
    /* 取消传输 */
    else if(resp == CAN)
    {
        if(Smodem_RX_Response(smodem, &resp, RECV_TIMEOUT) == True)
        {
            Smodem_Cancel(smodem);
            return Smodem_State(smodem, Smodem_State_TimeoutErr);
        }
        
        if(resp == CAN)
        {
            /* 接收方取消了传输 */
            return Smodem_State(smodem, Smodem_State_Cancel);
        }
        
        return smodem->State;
    }
    
    /* 异常数据 */
    else
    {
        return Smodem_Error(smodem);
    }
}

u8   Smodem_Master_GetProgress(Smodem_Master_Type *smodem)
{
    u8 progress;
    
    switch(smodem->State)
    {
        case Smodem_State_Start:
            progress = 0;
            break;
        
        case Smodem_State_Data:
            progress = (smodem->File_Size - smodem->File_Remain) * 100 / smodem->File_Size;
            break;
        
        case Smodem_State_OK:
        case Smodem_State_EOT:
        case Smodem_State_End:
            progress = 100;
            break;
        
        default:
            progress = 0xFF;
            break;
    }
    
    return progress;
}



#include "fw_debug.h"
#if MODULE_TEST && PROTOCOL_TEST && SMODEM_MASTER_TEST
#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_delay.h"


static u8 File_Buffer[10 * 1024] =
{
"\
aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffffgggggggggghhhhhhhhhhiiiiiiiiiijjjjjjjjjjkkkkkkkkkkllllllllllmmmmmmmmmm\
nnnnnnnnnnooooooooooppppppppppqqqqqqqqqqrrrrrrrrrrssssssssssttttttttttuuuuuuuuuuvvvvvvvvvvwwwwwwwwwwxxxxxxxxxxyyyyyyyyyyzzzzzzzzzz\
aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffffgggggggggghhhhhhhhhhiiiiiiiiiijjjjjjjjjjkkkkkkkkkkllllllllllmmmmmmmmmm\
nnnnnnnnnnooooooooooppppppppppqqqqqqqqqqrrrrrrrrrrssssssssssttttttttttuuuuuuuuuuvvvvvvvvvvwwwwwwwwwwxxxxxxxxxxyyyyyyyyyyzzzzzzzzzz\
aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffffgggggggggghhhhhhhhhhiiiiiiiiiijjjjjjjjjjkkkkkkkkkkllllllllllmmmmmmmmmm\
nnnnnnnnnnooooooooooppppppppppqqqqqqqqqqrrrrrrrrrrssssssssssttttttttttuuuuuuuuuuvvvvvvvvvvwwwwwwwwwwxxxxxxxxxxyyyyyyyyyyzzzzzzzzzz\
====================================================================================================================\
"
};


#define UART_NAME            "uart0"
static  FW_UART_Type *UART_Smodem;
static  void UART_Pre_Init(void *pdata)
{
    FW_UART_Type *uart = FW_Device_Find(UART_NAME);
    uart->TX_Pin = PA9;
    uart->RX_Pin = PA10;
    uart->Baudrate = 115200;
    uart->RX_RB_Size = 1100;
    uart->Config.RX_Mode = TRM_INT;
    
    UART_Smodem = uart;
}
FW_PRE_INIT(UART_Pre_Init, NULL);

static Smodem_Master_Type Smodem;
static char File_Name[10] = "master.txt";
static u8 Packet_Buffer[1100];
u16    Packet_Length = 1024;

static void Packet_TX(u8 *pdata, u32 num)
{
    FW_UART_Write(UART_Smodem, 0, pdata, num);
}

static u32  Packet_RX(u8 *pdata, u32 num)
{
    return FW_UART_Read(UART_Smodem, 0, pdata, num);
}

static void Packet_Clear(void)
{
    FW_UART_ClearFIFO(UART_Smodem, TOR_RX);
}

static u32  File_Read(u8 file_num, u32 offset, u8 *pdata, u32 num)
{
//    u8 *p = File_Buffer;
//    
//    memcpy(pdata, p + offset, num);
//    
//    return num;
    u8 *p = File_Buffer;
    memset(pdata, 0x5A, num);
    return num;
}


u8 Progress;

void Test(void)
{
    FW_UART_Type *uart = FW_Device_Find(UART_NAME);
    Smodem_Master_Type *smodem = &Smodem;
    u16 VGND = PA8;
    
    u8 state;
    u16 crc;
    
    FW_GPIO_Init(VGND, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_CLR(VGND);
    
    FW_UART_Init(uart);
        
    smodem->File_Name = File_Name;
//    smodem->File_Size = strlen((char *)File_Buffer);
    smodem->File_Size = 1216800;
    
    smodem->Packet_Buffer = Packet_Buffer;
    smodem->Packet_Length = Packet_Length;
    
    smodem->Packet_TX = Packet_TX;
    smodem->Packet_RX = Packet_RX;
    smodem->Packet_Clear = Packet_Clear;
    
    smodem->File_Read = File_Read;
    
    Smodem_Master_Init(smodem, Packet_Length);
    
    while(1)
    {
        state = Smodem_Master_Handler(smodem);
        if(state == Smodem_State_OK)
        {
//            crc = CRC16_Get(File_Buffer - 3, smodem->File_Size + 5);
//            if(crc != 0x46F8)
//            {
//                state = Smodem_State_DataErr;
//            }
//            else
//            {
//                while(1);
//            }
            while(1);
        }
        
        Progress = Smodem_Master_GetProgress(smodem);
        
        FW_Delay_Ms(10);
    }
}


#endif

