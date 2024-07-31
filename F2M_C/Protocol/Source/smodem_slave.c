/**
协议
@file_size在发送时，需要转换成ASCII码表示的16进制格式，例如文件大小为100字节，则
转换后为ASCII的"64"
       
@传输过程(由接收方发起)：
        发送端                              接收端
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< C
          SOH 00 FF "name1" "size1" CRC >>>>>     //起始帧
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK //起始帧应答
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< C
          STX 01 FE Data[length] CRC >>>>>>>>
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK
          STX 02 FD Data[length] CRC >>>>>>>>
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK
          ……
          STX n (FF ^ n) Data[length] CRC >>>    
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< NAK //若长时间未收到数据包，接收方
                                                  //也会发送NAK
          STX n (FF ^ n) Data[length] CRC >>>     //数据重发
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK
          ……
          STX m (FF ^ m) Data[x] 1A[y] CRC >>     //最后一包数据, x+y=length
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK
          EOT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< NAK
          EOT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< C   //是否继续发送文件，由接收方确
                                                  //定(发送C就是继续接收下一个文
                                                  //件，若发送方在1min内未接收到
                                                  //C，则会主动结束协议)
         
          SOH 00 FF "name2" "size2" CRC >>>>>     //发送第二个文件
          ……                                      //重复起始帧应答
          
          SOH 00 FF 00[length] CRC >>>>>>>>>>     //结束帧(空包，不携带任何数据)
              <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ACK //发送方设置一个超时时间，可以
                                                  //不检测最后的这个应答

协议握手：
由发送方开启握手。接收方收到握手指令后，启动协议。

超时处理：
1、启动传输时，接收方等待一个信息包的超时时限为10s，每个超时后发送NAK;
2、当收到包时，接收过程中每个字符串的超时间隔为1s;
3、为保持”接收方驱动“，发送方在等待启动字节(C)时不应该采用超时处理；
4、一旦传输开始，发送方采用单独的1min超时时限，给接收方充足的时间发送ACK,NAK,CAN
   之前的必须处理时间；
5、所有超时及错误事件至少重试10次。

协议取消：
为保持“接收方驱动”，协议取消由接收方发起。当协议出错或接收方无法再继续接收文件时，
接收方连续发送5个CAN来取消协议传输。而接收方再连续收到不少于2个CAN时(防止码)，即
可停止发送。

*/
#include "smodem_slave.h"

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
#define TIMEOUT_CNT_MAX      10
#define ERR_CNT_MAX          10
#define RESEND_CNT_MAX       10


#define YMODEM_SOH_LEN       (128 + NON_DATA_LEN)
#define YMODEM_STX_LEN       (1024 + NON_DATA_LEN)


//typedef enum
//{
//    Smodem_State_Cancel     = -5,                 //发送方取消
//    Smodem_State_ResendErr  = -4,                 //重发超限
//    Smodem_State_TimeoutErr = -3,                 //接收超时
//    Smodem_State_FileErr    = -2,                 //文件读写出错
//    Smodem_State_DataErr    = -1,                 //数据错误
//    
//    Smodem_State_OK = 0,                          //协议执行完成
//    Smodem_State_Start = 1,                       //处理起始帧
//    Smodem_State_Data,                            //处理数据帧
//    Smodem_State_EOT,                             //
//    Smodem_State_End,                             //处理结束帧
//}Smodem_State_Enum;


__INLINE_STATIC_ u16 Modem_CRC16_Get(u8 *pdata, u32 num)
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

static Bool Packet_Verify(u8 *pdata, u32 num)
{
    u16 crc = Modem_CRC16_Get(pdata, num);
    if(pdata[1] + pdata[2] != 0xFF ||
       (pdata[num - 2] << 8) + pdata[num - 1] != crc)
    {
        return False;
    }
    return True;
}

static void Smodem_TX_Byte(Smodem_Slave_Type *smodem, u8 value)
{
    smodem->Packet_TX(&value, 1);
}

static void Smodem_Response(Smodem_Slave_Type *smodem, u8 cmd)
{
    smodem->Response = cmd;            //保存当前的应答指令
    
    smodem->Timeout_Count = 0;         //接收到一包有效数据，所有异常计数清0
    smodem->Err_Count = 0;
    smodem->Resend_Count = 0;
    
    STiming_Reset(&smodem->Timing);
    
    Smodem_TX_Byte(smodem, cmd);
}

static void Smodem_Cancel(Smodem_Slave_Type *smodem)
{
    /* 接收方发送2个CAN取消传输 */
    Smodem_TX_Byte(smodem, CAN);
    Smodem_TX_Byte(smodem, CAN);
}

__INLINE_STATIC_ Smodem_State_Enum Smodem_State(Smodem_Slave_Type *smodem, Smodem_State_Enum state)
{
    smodem->State = state;
    return state;
}

static Smodem_State_Enum Smodem_Error(Smodem_Slave_Type *smodem)
{
    if(smodem->Err_Count >= ERR_CNT_MAX)
    {
        Smodem_Cancel(smodem);
        return Smodem_State(smodem, Smodem_State_DataErr);
    } 
    
    smodem->Err_Count++;
    if(smodem->Packet_Clear)  smodem->Packet_Clear();
    
    /* 当前数据包数据错误，请求重发 */
    Smodem_TX_Byte(smodem, NAK);
    
    return (Smodem_State_Enum)smodem->State;
}

static Smodem_State_Enum Smodem_Resend_Data(Smodem_Slave_Type *smodem)
{
    if(smodem->Resend_Count >= RESEND_CNT_MAX)
    {
        Smodem_Cancel(smodem);
        return Smodem_State(smodem, Smodem_State_ResendErr);
    }
    
    smodem->Resend_Count++;
    
    /* 收发正常，请求下一包数据 */
    smodem->Err_Count = 0;
    smodem->Timeout_Count = 0;
    Smodem_TX_Byte(smodem, ACK);
    
    return (Smodem_State_Enum)smodem->State;
}

static Bool Smodem_RX_Head(Smodem_Slave_Type *smodem, u8 *c, u32 timeout)
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
            if(smodem->Timeout_Count++ >= TIMEOUT_CNT_MAX)
            {
                smodem->Timeout_Count = 0;
                return False;
            }
            
            /* 文件数据已接收完成 */
            if(smodem->File_Recv >= smodem->File_Size)
            {
                /* 针对win10超级终端通讯过程中EOT问题 */
                smodem->Response = C;
            }
            
            /* 等待超时，重发当前应答 */
            Smodem_TX_Byte(smodem, smodem->Response);
        }
    }
}

static u32  Smodem_RX_Packet(Smodem_Slave_Type *smodem, u8 *pdata, u32 num, u32 timeout)
{
    u32 len;
    
    while(1)
    {
        len = smodem->Packet_RX(pdata, num);
        if(len)  
        {
            return len;
        }
        
        if(STiming_Start(&smodem->Timing, timeout) == True)
        {
            /* 等待超时，重发当前应答 */
            if(smodem->Response == ACK)  smodem->Response = NAK;
            Smodem_TX_Byte(smodem, smodem->Response);
            return 0;
        }
    }
}

static Smodem_State_Enum Smodem_Write_File(Smodem_Slave_Type *smodem, u32 len)
{
    u8 *pdata = smodem->Packet_Buffer;
    
    if(pdata[1] == smodem->Packet_Num)
    {
        /* 上一包数据的重发 */
        return Smodem_Resend_Data(smodem);
    }
    
    if(pdata[1] == (u8)(smodem->Packet_Num + 1))
    {
        /* 有效数据 */
        smodem->Packet_Num = pdata[1];
        
        /* 即将写入的数据长度 */
        if(smodem->File_Recv >= smodem->File_Size)
        {
            smodem->Resend_Count = 0;
            smodem->Err_Count = 0;
            smodem->Timeout_Count = 0;
            return Smodem_State(smodem, Smodem_State_Data);
        }
        else
        {
            if((smodem->File_Recv + len - NON_DATA_LEN) > smodem->File_Size)
            {
                len = smodem->File_Size - smodem->File_Recv;
                len += NON_DATA_LEN;
            }
        }
        
        while(1)
        {
            if(smodem->File_Write(smodem->File_Num, smodem->File_Recv, pdata + 3, len - NON_DATA_LEN) != 0)
            {
                smodem->Resend_Count = 0;
                smodem->Err_Count = 0;
                smodem->Timeout_Count = 0;
                
                smodem->File_Recv += len - NON_DATA_LEN;
                Smodem_Response(smodem, ACK);
                return Smodem_State(smodem, Smodem_State_Data);
            }
            else
            {
                if(smodem->Err_Count++ >= ERR_CNT_MAX)
                {
                    Smodem_Cancel(smodem);
                    return Smodem_State(smodem, Smodem_State_FileErr);
                }
            }
        }
    }
    
    /* 执行至此(数据包编号异常)，说明数据已经遗漏，需重启协议 */
    Smodem_Cancel(smodem);
    return Smodem_State(smodem, Smodem_State_DataErr);
}

void Smodem_Slave_Init(Smodem_Slave_Type *smodem, u32 packet_len)
{
    memset(smodem->File_Name, '\0', FILE_NAME_LEN);
    smodem->File_Size = 0;
    smodem->File_Recv = 0;
    
    smodem->Packet_Num = 0;
    
    smodem->Resend_Count = 0;
    smodem->Err_Count = 0;
    smodem->Timeout_Count = 0;
    
    smodem->State = Smodem_State_Start;
    smodem->Response = C;
    
    smodem->File_Num = 0;                                  //针对一次接收多个文件的情况，若每次只接收一个文件，则始终为0
    
    STiming_Init(&smodem->Timing, NULL);

    smodem->Packet_Length = packet_len + NON_DATA_LEN;
    
    smodem->Delay_Mul = smodem->Packet_Length / (YMODEM_STX_LEN - NON_DATA_LEN) + 1;
}

#define SMODEM_MULT_TEST  OFF

u8   Smodem_Slave_Handler(Smodem_Slave_Type *smodem)
{
    u32 len;
    u8 *pdata = smodem->Packet_Buffer;
    u8 i;
    
    #if SMODEM_MULT_TEST
    u8 mult_flag = False;
    #endif
    
    /* 由接收方启动协议 */
    if(smodem->File_Recv == 0 && smodem->State == Smodem_State_Start)
    {
        Smodem_TX_Byte(smodem, C);
    }
    
    if(Smodem_RX_Head(smodem, pdata, RECV_TIMEOUT) == False)
    {
        if(smodem->File_Recv == 0 && smodem->State == Smodem_State_Start)
        {
            /* 起始 */
            return smodem->State;
        }
        else
        {
            /* 接收超时后，取消协议通讯 */
            Smodem_Cancel(smodem);
            return Smodem_State(smodem, Smodem_State_TimeoutErr);
        }
    }
    
    /* 起始帧/尾数据包(最后剩余的数据<=128字节)/结束帧 */
    if(pdata[0] == SOH)
    {
        len = YMODEM_SOH_LEN;
        if(Smodem_RX_Packet(smodem, pdata + 1, len - 1, RECV_TIMEOUT * smodem->Delay_Mul) == 0)
        {
            return smodem->State;
        }
        
        if(Packet_Verify(pdata, len) != True)
        {
            return Smodem_Error(smodem);
        }
        
        if(smodem->State == Smodem_State_EOT)
        {
            for(i = 3; i < len; i++)
            {
                if(pdata[i] != 0x00)
                {
                    /* 有非零数据，说明连续发送了新的文件 */
                    smodem->State = Smodem_State_Start;
                    smodem->File_Recv = 0;
                    smodem->File_Size = 0;
                    smodem->File_Num++;
                    
                    #if SMODEM_MULT_TEST
                    mult_flag = True;
                    #endif
                    break;
                }
            }
            
            #if SMODEM_MULT_TEST
            if(mult_flag == False)
            {
                smodem->State = Smodem_State_Start;
                smodem->File_Recv = 0;
                smodem->File_Num++;
                Smodem_TX_Byte(smodem, ACK);
                return smodem->State;
            }
            #endif
        }
        
        if(smodem->State == Smodem_State_Start)
        {
            /* 有效数据, 保存当前数据包编号 */
            smodem->Packet_Num = pdata[1];
            
            /* 文件名 */
            strcpy(smodem->File_Name, (char *)pdata + 3);
            
            /* 文件大小，标准ymodem中，文件大小是按hex格式存储，而win10超级终端中
               则是按10进制存储的。这里按照10进制方式进行处理 */
            smodem->File_Size = atoi((char *)pdata + 3 + strlen(smodem->File_Name) + 1);
            
            /* 一帧数据接收成功，计数清0 */
            smodem->Timeout_Count = 0;
            smodem->Err_Count = 0;
            smodem->Resend_Count = 0;
            
            /* 告知sender起始帧已接收，可以发送文件数据 */
            Smodem_Response(smodem, ACK);
            Smodem_Response(smodem, C);
            
            return Smodem_State(smodem, Smodem_State_Data);
        }
        else if(smodem->State == Smodem_State_Data)
        {
            if(pdata[1] != 0)
            {
                /* 只有一包数据&不大于128字节 */
                return Smodem_Write_File(smodem, len);
            }
            else
            {
                Smodem_Response(smodem, C);
                return smodem->State;
            }
        }
        else if(smodem->State == Smodem_State_EOT)
        {
            /* 结束帧 */
            smodem->File_Recv = 0;
            smodem->File_Num = 0;
            smodem->File_Size = 0;
            Smodem_TX_Byte(smodem, ACK);
            return Smodem_State(smodem, Smodem_State_OK);
        }
        else
        {
            return Smodem_Error(smodem);
        }
    }
    
    /* 数据帧 */
    else if(pdata[0] == STX)
    {
        len = smodem->Packet_Length;
        if(Smodem_RX_Packet(smodem, pdata + 1, len - 1, RECV_TIMEOUT * smodem->Delay_Mul) == 0)
        {
            return smodem->State;
        }
        
        if(Packet_Verify(pdata, len) != True)
        {
            return Smodem_Error(smodem);
        }
        
        if(smodem->State == Smodem_State_Start || smodem->State == Smodem_State_Data)
        {
            return Smodem_Write_File(smodem, len);
        }
        else
        {
            return Smodem_Error(smodem);
        }
    }
    
    /* EOT */
    else if(pdata[0] == EOT)
    {
        /* 接收方收到最后一帧数据并应答ACK后，若未接收到发送方发送的EOT，则在超时
           时，直接发送C即可 */
        if(smodem->State == Smodem_State_Data)
        {
            /* 第一个EOT */
            Smodem_Response(smodem, NAK);
            return Smodem_State(smodem, Smodem_State_EOT);
        }
        else if(smodem->State == Smodem_State_EOT)
        {
            /* 第二个EOT */
            Smodem_Response(smodem, ACK);
            Smodem_Response(smodem, C);
            /* 防止发送方未接收到应答，持续发送EOT的情况 */
            return Smodem_State(smodem, Smodem_State_EOT);
        }
        else
        {
            return Smodem_Error(smodem);
        }
    }
    
    /* Cancel */
    else if(pdata[0] == CAN)
    {
        len = 4;
        if(Smodem_RX_Packet(smodem, pdata + 1, len - 1, RECV_TIMEOUT) == 0)
        {
            return smodem->State;
        }
        
        return Smodem_State(smodem, Smodem_State_Cancel);
    }
    
    /* Abort */
//    else if(pdata[0] == ABORT1 || pdata[0] == ABORT2)
//    {
//    
//    }
    
    /* 异常数据 */
    else
    {
        return Smodem_Error(smodem);
    }
}

u8   Smodem_Slave_GetProgress(Smodem_Slave_Type *smodem)
{
    u8 progress;

    switch(smodem->State)
    {
        case Smodem_State_Start:
            progress = 0;
            break;
        
        case Smodem_State_Data:
            progress = smodem->File_Recv * 100 / smodem->File_Size;
            break;
        
        case Smodem_State_OK:
        case Smodem_State_EOT:
            progress = 100;
            break;
        
        default:
            progress = 0;
            break;
    }
    
    return progress;
}

Bool Smodem_Slave_IsFault(Smodem_Slave_Type *smodem)
{
    s8 state = smodem->State;
    if(state < Smodem_State_OK)  return True;
    else  return False;
}


#include "fw_debug.h"
#if MODULE_TEST && PROTOCOL_TEST && SMODEM_SLAVE_TEST
#include "fw_uart.h"
#include "fw_gpio.h"
#include "fw_delay.h"


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

static Smodem_Slave_Type Smodem;
static char File_Name[10];
static u8 File_Buffer[10 * 1024];
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

u32 Sum = 0;
static u32 Cnt = 0;

static u32  File_Write(u8 file_num, u32 addr, u8 *pdata, u32 num)
{
    u8 *p = File_Buffer;
    u32 i;
    Smodem_Slave_Type *smodem = &Smodem;
    
    Cnt += num;
    if(Cnt >= smodem->File_Size)
    {
        num = 1024 - (Cnt - smodem->File_Size);
    }
    
//    memcpy(p + addr, pdata, num);
    for(i = 0; i < num; i++)
    {
        Sum += pdata[i];
    }
    
    return num + 1;
}

u8 Progress;

void Test(void)
{
    FW_UART_Type *uart = FW_Device_Find(UART_NAME);
    Smodem_Slave_Type *smodem = &Smodem;
    u16 VGND = PA8;
    
    u16 crc;
    u8 state;
    u8 progress;
    
    FW_GPIO_Init(VGND, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_CLR(VGND);
    
    FW_UART_Init(uart);
    
    smodem->File_Name = File_Name;
    
    smodem->Packet_Buffer = Packet_Buffer;
    smodem->Packet_Length = Packet_Length;
    
    smodem->Packet_TX = Packet_TX;
    smodem->Packet_RX = Packet_RX;
    smodem->Packet_Clear = Packet_Clear;
    
    smodem->File_Write = File_Write;
    
    Smodem_Slave_Init(smodem, Packet_Length);
    
    while(1)
    {
        state = Smodem_Slave_Handler(smodem);
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
            Sum %= 0x100;
            Sum = 0x100 - Sum;
            while(1);
        }
        
        progress = Smodem_Slave_GetProgress(smodem);
        if(progress && progress <= 100)
        {
            Progress = progress;
        }
        
        FW_Delay_Ms(10);
    }
}

#endif

