#include "fw_debug.h"
#include "fw_print.h"

#include "fw_gpio.h"
#include "fw_delay.h"
#include "fw_spi.h"

#include "mifare.h"

#include "libc.h"



/**
 * @SPI驱动
 */
__INLINE_STATIC_ void SGP_Init(MF_S522_Type *dev)
{
//    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    
    if(dev->RST_Pin)
    {
        if(dev->RST_VL == LEVEL_L)
        {
            FW_GPIO_Init(dev->RST_Pin, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
            FW_GPIO_SET(dev->RST_Pin);
        }
        else
        {
            FW_GPIO_Init(dev->RST_Pin, FW_GPIO_Mode_Out_PPD, FW_GPIO_Speed_Low);
            FW_GPIO_CLR(dev->RST_Pin);
        }
    }
    
    if(dev->IRQ_Pin)
    {
    
    }
    
//    FW_ASSERT(drv->Init);
    
//    if(drv->Init)  drv->Init(spi);
    FW_SPI_Init(spi);
}

__INLINE_STATIC_ u8 SGP_Get_Reg(MF_S522_Type *dev, u8 reg_addr)
{
//    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    u8 value = (reg_addr << 1) | 0x80;
    
//    FW_ASSERT(drv->CS_CTL);
//    FW_ASSERT(drv->Write);
//    FW_ASSERT(drv->Read);
    
//    drv->CS_CTL(spi, Enable);
//    drv->Write(spi, 0, &value, 1);
//    drv->Read(spi, 0, &value, 1);
//    drv->CS_CTL(spi, Disable);
    FW_SPI_CSSet(spi, Enable);
    FW_SPI_Write(spi, 0, &value, 1);
    FW_SPI_Read(spi, 0, &value, 1);
    FW_SPI_CSSet(spi, Disable);
    
    return value;
}

__INLINE_STATIC_ void SGP_Set_Reg(MF_S522_Type *dev, u8 reg_addr, u8 value)
{
//    FW_SPI_Driver_Type *drv = FW_Device_GetDriver(dev);
    FW_SPI_Type *spi = FW_Device_GetParent(dev);
    u8 msg[2];
    
    msg[0] = (reg_addr << 1) & 0x7E;
    msg[1] = value;
    
//    FW_ASSERT(drv->CS_CTL);
//    FW_ASSERT(drv->Write);
//    FW_ASSERT(drv->Read);
    
//    drv->CS_CTL(spi, Enable);
//    drv->Write(spi, 0, msg, 2);
//    drv->CS_CTL(spi, Disable);
    
    FW_SPI_CSSet(spi, Enable);
    FW_SPI_Write(spi, 0, msg, 2);
    FW_SPI_CSSet(spi, Disable);
}




/**
 * @I2C驱动
 */




/**
 * @UART驱动
 */




/* FM17522的具体实现 */
#define MI_NOTAGERR       0xEE
#define MAXRLEN           64    /*FIFO最大长度*/  

#define Anticollision     0x02

/* 寄存器地址宏定义 */
#define CommandReg        0x01
#define ComIEnReg         0x02
#define DivIEnReg         0x03
#define ComIrqReg         0x04
#define DivIrqReg         0x05
#define ErrorReg          0x06
#define Status1Reg        0x07
#define Status2Reg        0x08
#define FIFODataReg       0x09
#define FIFOLevelReg      0x0A
#define WaterLevelReg     0x0B
#define ControlReg        0x0C
#define BitFramingReg     0x0D
#define CollReg           0x0E
#define ModeReg           0x11
#define TxModeReg         0x12
#define RxModeReg         0x13
#define TxControlReg      0x14
#define TxAutoReg         0x15
#define TxSelReg          0x16
#define RxSelReg          0x17
#define RxThresholdReg    0x18
#define DemodReg          0x19
#define MfTxReg           0x1C
#define MfRxReg           0x1D
#define SerialSpeedReg    0x1F
#define CRCMSBReg         0x21
#define CRCLSBReg         0x22
#define ModWidthReg       0x24
#define GsNOffReg         0x23
#define TxBitPhaseReg     0x25
#define RFCfgReg          0x26
#define GsNReg            0x27
#define CWGsPReg          0x28
#define ModGsPReg         0x29
#define TModeReg          0x2A
#define TPrescalerReg     0x2B
#define TReloadMSBReg     0x2C
#define TReloadLSBReg     0x2D
#define TCounterValMSBReg 0x2E
#define TCounterValLSBReg 0x2F
#define TestSel1Reg       0x31
#define TestSel2Reg       0x32
#define TestPinEnReg      0x33
#define TestPinValueReg   0x34
#define TestBusReg        0x35
#define AutoTestReg       0x36
#define VersionReg        0x37
#define AnalogTestReg     0x38
#define TestDAC1Reg       0x39
#define TestDAC2Reg       0x3A
#define TestADCReg        0x3B

/* 指令集宏定义 */
#define Idle              0x00  /* 无动作，取消当前指令执行                                                      */    
#define Mem               0x01  /* 配置FM17550为FeliCa、MIFARE和NFCIP-1通讯模式                                  */
#define RandomID          0x02  /* 产生一个10字节随机数                                                          */
#define CalcCRC           0x03  /* 激活CRC协处理器                                                               */
#define Transmit          0x04  /* 发射FIFO缓冲器中的数据                                                        */
#define NoCmdChange       0x07  /* 指令不改变，可以用来在不影响指令的情况下修改CommandReg寄存器，比如PowerDown位 */
#define Receive           0x08  /* 激活接收器电路                                                                */ 
#define Transceive        0x0C  /* 发射FIFO缓冲器中的数据到天线，并在发射后自动激活接收器                        */
#define MFAuthent         0x0E  /* 作为读写器模式执行MIFARE标准安全认证 密钥验证                                 */ 
#define SoftReset         0x0F  /* 复位FM17550                                                                   */


/* 实现方法 */
//__INLINE_STATIC_ u8 FM_Get_Reg(MF_S522_Type *pdev, u8 reg_addr)
//{
//    void *dev = FW_Device_GetPDevice(pdev);
//    return pdev->Get_Reg(dev, reg_addr);
//}

//__INLINE_STATIC_ void FM_Set_Reg(MF_S522_Type *pdev, u8 reg_addr, u8 value)
//{
//    void *dev = FW_Device_GetPDevice(pdev);
//    pdev->Set_Reg(dev, reg_addr, value);
//}

/*********************************************************************************************************
** Function name:       Read_Reg
** Descriptions:        读取寄存器                
** input parameters:    reg_add:寄存器数值
** output parameters:   N/A
** Returned value:      寄存器数值
*********************************************************************************************************/
__INLINE_STATIC_ u8 FM_Read_Reg(MF_S522_Type *pdev, u8 reg_addr)
{
    return pdev->Get_Reg(pdev, reg_addr);
}

/*********************************************************************************************************
** Function name:       Write_Reg
** Descriptions:        写寄存器操作                
** input parameters:    reg_add:寄存器地址
**                      reg_value:寄存器数值
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ void FM_Write_Reg(MF_S522_Type *pdev, u8 reg_addr, u8 reg_value)
{
    pdev->Set_Reg(pdev, reg_addr, reg_value);
}

/*********************************************************************************************************
** Function name:       SPIRead_Sequence
** Descriptions:        SPI读FIFO寄存器的值
** input parameters:    sequence_length 数据长度 ucRegAddr：寄存器地址  *reg_value 数据指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
__INLINE_STATIC_ void FM_Read_Sequence(MF_S522_Type *pdev, u8 sequence_length, u8 ucRegAddr, u8 *reg_value)
{
    u8 i;
    
    if(sequence_length == 0)  return;
    
    for(i = 0; i < sequence_length; i++)
    {
        *(reg_value + i) = FM_Read_Reg(pdev, ucRegAddr);
    }
}

/*********************************************************************************************************
** Function name:       SPIWrite_Sequence
** Descriptions:        SPI写FIFO的值
** input parameters:    sequence_length 数据长度 
**                      ucRegAddr：寄存器地址  
**                      *reg_value 数据指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
__INLINE_STATIC_ void FM_Write_Sequence(MF_S522_Type *pdev, u8 sequence_length, u8 ucRegAddr, u8 *reg_value)
{
    u8 i;
    
    if(sequence_length == 0)  return;
    
    for(i = 0; i < sequence_length; i++)
    {
        FM_Write_Reg(pdev, ucRegAddr, *(reg_value + i));
    }
}

/*********************************************************************************************************
** Function name:       Read_FIFO
** Descriptions:        读出FIFO的数据         
** input parameters:    length:读取数据长度
**                      *fifo_data:数据存放指针
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ void FM_Read_FIFO(MF_S522_Type *pdev, u8 length, u8 *fifo_data)
{
    FM_Read_Sequence(pdev, length, FIFODataReg, fifo_data);
}

/*********************************************************************************************************
** Function name:       Write_FIFO
** Descriptions:        写入FIFO         
** input parameters:    length:读取数据长度
**                      *fifo_data:数据存放指针
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ void FM_Write_FIFO(MF_S522_Type *pdev, u8 length, u8 *fifo_data)
{
    FM_Write_Sequence(pdev, length, FIFODataReg, fifo_data);
}

/*********************************************************************************************************
** Function name:       Set_BitMask
** Descriptions:        置位寄存器操作    
** input parameters:    reg_add，寄存器地址
**                      mask，寄存器写1位
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ void FM_Set_BitMask(MF_S522_Type *pdev, u8 reg_addr, u8 mask)
{
    u8 value = FM_Read_Reg(pdev, reg_addr);
    FM_Write_Reg(pdev, reg_addr, value | mask);
}

/*********************************************************************************************************
** Function name:       Clear_BitMask
** Descriptions:        清除位寄存器操作
** input parameters:    reg_add，寄存器地址
**                      mask，寄存器清除位
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ void FM_Clear_BitMask(MF_S522_Type *pdev, u8 reg_addr, u8 mask)
{
    u8 value = FM_Read_Reg(pdev, reg_addr);
    FM_Write_Reg(pdev, reg_addr, value & ~mask);
}

/*********************************************************************************************************
** Function name:       Clear_FIFO
** Descriptions:        清空FIFO              
** input parameters:   
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ u8 FM_Clear_FIFO(MF_S522_Type *pdev)
{
    FM_Set_BitMask(pdev, FIFOLevelReg, 0x80);
    if(FM_Read_Reg(pdev, FIFOLevelReg) == 0)  return True;
    else  return False;
}

/*********************************************************************************************************
** Function name:       Set_RF
** Descriptions:        设置射频输出
** input parameters:    mode，射频输出模式
**                      0，关闭输出
**                      1，仅打开TX1输出
**                      2，仅打开TX2输出
**                      3，TX1，TX2打开输出，TX2为反向输出
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ u8 FM_Set_RF(MF_S522_Type *pdev, u8 mode)
{
    if((FM_Read_Reg(pdev, TxControlReg) & 0x03) == mode)  return True;
    
    if(mode == 0)  FM_Clear_BitMask(pdev, TxControlReg, 0x03);
    else if(mode == 1)  FM_Clear_BitMask(pdev, TxControlReg, 0x01);
    else if(mode == 2)  FM_Clear_BitMask(pdev, TxControlReg, 0x02);
    else if(mode == 3)  FM_Set_BitMask(pdev, TxControlReg, 0x03);
    else  return False;
    
    FW_Delay_Ms(100);
    
    return True;
}

/*********************************************************************************************************
** Function name:       FM_Comm
** Descriptions:        读卡器通信 不利用IRQ管脚的情况
** input parameters:    Command:通信操作命令
**                      pInData:发送数据数组
**                      InLenByte:发送数据数组字节长度
**                      pOutData:接收数据数组
**                      pOutLenBit:接收数据的位长度
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ u8 FM_Comm(MF_S522_Type *pdev, u8 Command, u8 *pInData, u8 InLenByte, u8 *pOutData, u32 *pOutLenBit)
{
    u8 status   = False;
    u8 irqEn    = 0x00;                                                /* 使能的中断                   */
    u8 waitFor  = 0x00;                                                /* 等待的中断                   */
    u8 lastBits = 0x00;
    u8 n;                                                              /* 获取中断请求寄存器           */ 
    u8 sendLen  = 0x00;                                                /* 发送数据长度                 */
    u8 sendPi   = 0x00;
    u8 revLen   = 0x00;                                                /* 接收数据长度                 */
    u8 fifoLen  = 0x00;
    u8 errorReg = 0x00;                                                /* 错误标志位                   */
    u8 collPos  = 0x00;                                                /* 冲突位                       */
    u32 i;
    
    FM_Write_Reg(pdev, ComIrqReg, 0x7F);                               /* 清除IRQ标记                  */
    FM_Write_Reg(pdev, TModeReg, 0x80);                                /* 设置TIMER自动启动            */

    switch(Command)
    {
        case MFAuthent:                                                /* Mifare认证                   */
            irqEn   = 0x12;
            waitFor = 0x10;
            break;
        
        case Transceive:                                               /* 发送FIFO中的数据到天线，传输后激活接收电路 */
            irqEn   = 0x77;
            waitFor = 0x30;
            break;
        
        default:
            break;
    }

    FM_Write_Reg(pdev, ComIEnReg, irqEn | 0x80);
    FM_Write_Reg(pdev, CommandReg, Idle);
    FM_Set_BitMask(pdev, FIFOLevelReg, 0x80);
    FM_Clear_FIFO(pdev);                                               /* 清空FIFO数据 */
    sendLen = (InLenByte > MAXRLEN) ? MAXRLEN : InLenByte;             /* 限制发送数据长度，最大长度为FIFO长度64字节    */
    sendPi += sendLen;
    InLenByte -= sendLen;
    
    FM_Write_FIFO(pdev, sendLen, pInData);                             /* 数据写入FIFO                      */
    FM_Write_Reg(pdev, CommandReg, Command);                           /* 设置数据发送命令                  */
    
    if(Command == Transceive)
    {
        FM_Set_BitMask(pdev, BitFramingReg, 0x80);                     /* 开启数据发送                      */ 
    }
    
    i = 50;                                                           /* 根据时钟频率调整，操作M1卡最大等待时间25ms*/
    do
    {
        n = FM_Read_Reg(pdev, ComIrqReg);                              /* 读取控制中断请求寄存器            */
        i--;
        FW_Delay_Ms(1);
    }while((i != 0) && !(n & 0x01) && !(n & waitFor));                 /* 未超时且无错误 n&0x01!=1表示PCDsettimer时间未到 */
                                                                       /* n&waitFor!=1表示指令执行完成 */
    FM_Clear_BitMask(pdev, BitFramingReg, 0x80);                       /* 关闭发送                         */
    
    if(i != 0)
    {
        errorReg = FM_Read_Reg(pdev, ErrorReg);
        
        if(!(errorReg & 0x1B))                                         /* 无错误                           */
        {
            status = True;
            
            if(n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
        
            if(Command == Transceive)
            {
                fifoLen = FM_Read_Reg(pdev, FIFOLevelReg);             /* 读取接收数据字节数               */
                lastBits = FM_Read_Reg(pdev, ControlReg) & 0x07;       /* 读取该接收最后一字节的有效位     */
                if(lastBits)
                {
                    *pOutLenBit = lastBits;
                    if(fifoLen > 1)  *pOutLenBit += (fifoLen - 1) * 8;
                    else  fifoLen = 1;
                }
                else
                {
                    *pOutLenBit = fifoLen * 8;
                }
                FM_Read_FIFO(pdev, fifoLen, &pOutData[revLen]);
            }
        }
        else if(errorReg & 0x08)                                       /* 有冲突                          */
        {
            FM_Write_Reg(pdev, ErrorReg, ~0x08);                       /* 清除接收中断                    */
            collPos = FM_Read_Reg(pdev, CollReg);                      /* 获取冲突位置 冲突在第几位(1-32) */
            collPos &= 0x1F;
            *pOutLenBit = (collPos == 0) ? 32 : collPos;
            fifoLen = *pOutLenBit / 8 + ((*pOutLenBit % 8) ? 1 : 0);   /* 字节长度                        */
            FM_Read_FIFO(pdev, fifoLen, &pOutData[revLen]);
            status = Anticollision;
        }
        else
        {
            status = False;
        }
    }
    
    FM_Clear_BitMask(pdev, BitFramingReg, 0x80);                       /* 关闭发送                        */
    
    return status;
}

/*********************************************************************************************************
** Function name:       FM_SetTimer
** Descriptions:        设置接收延时
** input parameters:    delaytime，延时时间（单位为毫秒）  
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ u8 FM_Set_Timer(MF_S522_Type *pdev, u32 delaytime)
{
    u32 TimeReload = 0;
    u32 Prescaler = 0;
    
    while(Prescaler < 0xFFF)
    {
        TimeReload = ((delaytime * (long)13560) - 1) /  (Prescaler * 2 + 1);
        if(TimeReload < 0xFFFF)  break;
        Prescaler++;
    }
    
    TimeReload = TimeReload & 0xFFFF;
    FM_Set_BitMask(pdev, TModeReg, Prescaler >> 8);
    FM_Write_Reg(pdev, TPrescalerReg, Prescaler & 0xFF);
    FM_Write_Reg(pdev, TReloadMSBReg, TimeReload >> 8);
    FM_Write_Reg(pdev, TReloadLSBReg, TimeReload & 0xFF);
    
    return True;
}

/*********************************************************************************************************
** Function name:       FM_ConfigISOType
** Descriptions:        配置ISO14443A/B协议
** input parameters:    type = 0：ISO14443A协议；
**                      type = 1，ISO14443B协议；   
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ Bool FM_Config_ISOType(MF_S522_Type *pdev, u8 type)
{
    if(type == 0)
    {
        FM_Set_BitMask(pdev, ControlReg, 0x10);
        FM_Set_BitMask(pdev, TxAutoReg, 0x40);
        FM_Write_Reg(pdev, TxModeReg, 0x00);
        FM_Write_Reg(pdev, RxModeReg, 0x00);
        
        FM_Write_Reg(pdev, RxThresholdReg, 0x55);
        FM_Write_Reg(pdev, GsNReg, 0x88);
        FM_Write_Reg(pdev, CWGsPReg, 0x20);
    }
    else if(type == 1)
    {
        FM_Write_Reg(pdev, ControlReg, 0x10);
        FM_Write_Reg(pdev, TxAutoReg, 0x00);
        FM_Write_Reg(pdev, TxModeReg, 0x83);
        FM_Write_Reg(pdev, RxModeReg, 0x83);
        FM_Write_Reg(pdev, RxThresholdReg, 0x55);
        FM_Write_Reg(pdev, RFCfgReg, 0x48);
        FM_Write_Reg(pdev, TxBitPhaseReg, 0x87);
        FM_Write_Reg(pdev, GsNReg, 0x83);
        FM_Write_Reg(pdev, CWGsPReg, 0x30);
        FM_Write_Reg(pdev, GsNOffReg, 0x38);
        FM_Write_Reg(pdev, ModGsPReg, 0x20);
    }
    else
    {
        return False;
    }
    
    FW_Delay_Ms(3);
    
    return True;
}

/*********************************************************************************************************
** Function name:       FM_Request        
** Descriptions:        FM_Request卡片寻卡    
** input parameters:    N/A
** output parameters:   pTagType[0],pTagType[1] =ATQA 
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ u8 FM_Request(MF_S522_Type *pdev, u8 *pTagType)
{
    u8 result, send_buff[1], rece_buff[2];
    u32 rece_bitlen;
    
    FM_Clear_BitMask(pdev, TxModeReg, 0x80);
    FM_Clear_BitMask(pdev, RxModeReg, 0x80);
    FM_Set_BitMask(pdev, RxModeReg, 0x08);
    FM_Clear_BitMask(pdev, Status2Reg, 0x08);
    FM_Write_Reg(pdev, BitFramingReg, 0x07);
    
    FM_Set_Timer(pdev, 1);
    FM_Clear_FIFO(pdev);
    
    send_buff[0] = 0x26;
    
    result = FM_Comm(pdev, Transceive, send_buff, 1, rece_buff, &rece_bitlen);
    if((result == True) && (rece_bitlen == 2 * 8))
    {
        *pTagType = rece_buff[0];
        *(pTagType + 1) = rece_buff[1];
    }
    
    return result;
}

/*********************************************************************************************************
** Function name:       RightMoveBit        
** Descriptions:        该函数实现一个字节多少个1
** input parameters:    bitNum: 右边多少个1
** output parameters:   
** Returned value:      
*********************************************************************************************************/
__INLINE_STATIC_ u8 RightMoveBit(u8 bitNum)
{
    u8 temp,i;
    temp = 0x00;
    for (i=0; i < bitNum; i++) 
    {
       temp >>= 1;
       temp += 0x80;
    }
    return temp;
}

/*********************************************************************************************************
** Function name:       RightMoveBit        
** Descriptions:        该函数实现一个字节多少个1
** input parameters:    bitNum: 左边多少个1
** output parameters:   
** Returned value:      
*********************************************************************************************************/
__INLINE_STATIC_ u8 LeftMoveBit(u8 bitNum)
{
    u8 temp,i;
    temp = 0x00;
    for (i=0; i < bitNum; i++)
    {
        temp <<= 1;
        temp += 0x01;
    }
    return temp;
}

/*********************************************************************************************************
** Function name:       CollSaveUID        
** Descriptions:        通过前一次冲突位置和后一次冲突位置保存UID
** input parameters:    preColl:前一次冲突位置
                        nowColl:后一次冲突位置
                        uid:保存UID指针
** output parameters:   void
** Returned value:      
*********************************************************************************************************/
__INLINE_STATIC_ void FM_CollSaveUID(u8 preColl, u8 nowColl, u8 *buff, u8 *uid)
{
    u8 i,j,preBytes,preBits,nowBytes,nowBits;
    u8 temp;
    preBytes = preColl / 8;
    preBits  = preColl % 8;
    nowBytes = nowColl / 8;
    nowBits  = nowColl % 8;
    
    if(preColl == 0)                                                      /* 之前没有冲突位，直接保存     */
    {                 
        for(i=0; i < nowBytes; i++)
        {
            uid[i] = buff[i];  
        }            
        uid[nowBytes] = buff[nowBytes] & LeftMoveBit(nowBits);
    } 
    else                                                                  /* 之前有冲突                   */
    {                                                              
        if(nowBytes > preBytes) 
        {
            temp = RightMoveBit(8 - preBits);
            temp = buff[0] & temp;
            uid[preBytes] |= temp;
            if(nowBytes > (preBytes+1))                                   /* 保存完整字节uid              */
            { 
                for(i=1, j=0; j < (nowBytes-preBytes-1); i++, j++)
                {
                    uid[preBytes+i] = buff[i];  
                }
            }
            if(nowBits)                                                   /* 保存位UID                   */
            {                                                 
                uid[nowBytes] = buff[nowBytes] & LeftMoveBit(nowBits);
            }
        } 
        else 
        {
            temp = buff[0] & LeftMoveBit(nowBits);           
            uid[preBytes] |= temp;
        }
    }
}

/*********************************************************************************************************
** Function name:       Set_BitFraming        
** Descriptions:        该函数实现对收到的卡片的序列号的判断
** input parameters:    bytes: 已知的UID字节数  
**                      bits: 额外的已知UIDbits        
**                      length: 接収到的UID数据长度 
** output parameters:   NVB:防冲突种类
** Returned value:      void
*********************************************************************************************************/
__INLINE_STATIC_ void FM_Set_BitFraming(MF_S522_Type *pdev, u8 collBit, u8 *NVB)
{
    u8 bytes,bits;
    
    bytes = collBit / 8;
    bits = collBit % 8;
    
    switch(bytes) 
    {
        case 0: 
            *NVB = 0x20;
            break;
        case 1:
            *NVB = 0x30;
            break;
        case 2:
            *NVB = 0x40;
            break;
        case 3:
            *NVB = 0x50;
            break;
        case 4:
            *NVB = 0x60;
            break;
        case 5:
            *NVB = 0x70;                                                      
            break;
        default:
            break;
    }
    
    switch(bits) 
    {
        case 0:
            FM_Write_Reg(pdev, BitFramingReg, 0x00);
            break;
        case 1:
            FM_Write_Reg(pdev, BitFramingReg, 0x11);
            *NVB = (*NVB | 0x01);
            break;
        case 2:
            FM_Write_Reg(pdev, BitFramingReg, 0x22);
            *NVB = (*NVB | 0x02);
            break;
        case 3:
            FM_Write_Reg(pdev, BitFramingReg, 0x33);
            *NVB = (*NVB | 0x03);
            break;
        case 4:
            FM_Write_Reg(pdev, BitFramingReg, 0x44);
            *NVB = (*NVB | 0x04);
            break;
        case 5:
            FM_Write_Reg(pdev, BitFramingReg, 0x55);
            *NVB = (*NVB | 0x05);
            break;
        case 6:
            FM_Write_Reg(pdev, BitFramingReg, 0x66);
            *NVB = (*NVB | 0x06);
            break;
        case 7:
            FM_Write_Reg(pdev, BitFramingReg, 0x77);
            *NVB = (*NVB | 0x07);
            break;
        default:
            break;
    }
}

/*********************************************************************************************************
** Function name:       FM_Anticollision        
** Descriptions:        卡片防冲突    
** input parameters:    selcode：卡片选择编码 0x93，0x95，0x97    
**                      picc_uid：卡片UID号
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ u8 FM_Anticollision(MF_S522_Type *pdev, u8 selcode, u8 *uid)
{
    u8 send_buff[10];
    u8 rece_buff[5];
	u8 result;
    u8 nBytes, preCollBit, nowCollBit;    
    u32 rece_bitlen;
    
    FM_Clear_BitMask(pdev, TxModeReg, 0x80);
    FM_Clear_BitMask(pdev, RxModeReg, 0x80);
    FM_Clear_BitMask(pdev, Status2Reg, 0x08);
    FM_Write_Reg(pdev, BitFramingReg, 0x00);
    FM_Clear_BitMask(pdev, CollReg, 0x80);
        
    memset(uid, 0x00, 5);
    memset(rece_buff, 0x00, 5);
    preCollBit = 0;
	nowCollBit = 0;
    result = Anticollision; 
    while (result == Anticollision) 
    {
        FM_CollSaveUID(preCollBit, nowCollBit, &rece_buff[0], uid);					  
        preCollBit   = nowCollBit;
        send_buff[0] = selcode;                                             /* 防冲突命令                  */
        FM_Set_BitFraming(pdev, preCollBit, &send_buff[1]);                 /* 设置NVB发送                 */
        nBytes = (preCollBit / 8) + ((preCollBit % 8) ? 1 : 0);             /* 计算已有ID号长度              */
        memcpy(&send_buff[2], uid, nBytes);                                 /* 将有效字节拷贝到发送buff    */   
        FM_Set_Timer(pdev, 10);                                             /* 设置定时器                  */
        FM_Clear_FIFO(pdev);
        result = FM_Comm(pdev, Transceive, send_buff, 2 + nBytes, rece_buff, &rece_bitlen);
		nowCollBit = (u8)(rece_bitlen & 0xff);                              /* 保存当前冲突位置            */
        if (result == True) 
        {
            FM_CollSaveUID(preCollBit, nowCollBit, rece_buff, uid);         /* 存下已得到的UID             */
        }
    }
    
    if(uid[4] != (uid[0] ^ uid[1] ^ uid[2] ^ uid[3]))                       /* 异或UID校验                 */  
    {
        if(True == result)
        {
            result = False;
        }
    }
    
    return result;
}

/*********************************************************************************************************
** Function name:       FM_Select        
** Descriptions:        选择卡片
** input parameters:    selcode：卡片选择编码 0x93，0x95，0x97    
**                      pSnr：卡片UID号
**                      pSak：卡片选择应答
** output parameters:   N/A
** Returned value:      True：操作成功 ERROR：操作失败    
*********************************************************************************************************/
__INLINE_STATIC_ u8 FM_Select(MF_S522_Type *pdev, u8 selcode, u8 *pSnr, u8 *pSak)
{
    u8 result,i, send_buff[7], rece_buff[5];
    u32 rece_bitlen;
    
    FM_Write_Reg(pdev, BitFramingReg, 0x00);
    FM_Set_BitMask(pdev, TxModeReg, 0x80);                              /* 打开TX CRC                   */
    FM_Set_BitMask(pdev, RxModeReg, 0x80);                              /* 打开RX CRC校验               */
    FM_Clear_BitMask(pdev, Status2Reg, 0x08);                           /* 清除验证标志位               */
    
    send_buff[0] = selcode;                                             /* select命令                   */
    send_buff[1] = 0x70;                                                /* NVB                          */
    for (i = 0; i < 5; i++) 
    {
        send_buff[i+2] = *(pSnr+i);                                     /* 将接收到的UID写入，可能是4字节完整UID，也可能是CT连接字节+3字节UID */
    }
    send_buff[6] = pSnr[0]^pSnr[1]^pSnr[2]^pSnr[3];                     /* 尾部BCC字节 */
    
    FM_Set_Timer(pdev,  1);
    FM_Clear_FIFO(pdev);
    result = FM_Comm(pdev,  Transceive, send_buff, 7, rece_buff, &rece_bitlen);
    if(result == True) 
    {
        *pSak = rece_buff[0]; 
    }
    
    return result;
}

/* 秘钥验证 */
__INLINE_STATIC_ u8 FM_Authent(MF_S522_Type *pdev, u8 mode, u8 sector, u8 *mifare_key, u8 *card_uid)
{
    u8 send_buff[12], rece_buff[1], result;
    u32  rece_bitlen;
    
    if (mode == 0x00)  send_buff[0] = 0x60;                              /* keyA 0x60                */
    if ( mode == 0x01)  send_buff[0] = 0x61;                             /* KeyB                     */

    send_buff[1]  = sector * 4;                                          /* 块地址                   */
    send_buff[2]  = mifare_key[0];
    send_buff[3]  = mifare_key[1];
    send_buff[4]  = mifare_key[2];
    send_buff[5]  = mifare_key[3];
    send_buff[6]  = mifare_key[4];
    send_buff[7]  = mifare_key[5];
    send_buff[8]  = card_uid[0];
    send_buff[9]  = card_uid[1];
    send_buff[10] = card_uid[2];
    send_buff[11] = card_uid[3];
    
    FM_Set_Timer(pdev, 1);
    FM_Clear_FIFO(pdev);
    result = FM_Comm(pdev, MFAuthent, send_buff, 12, rece_buff, &rece_bitlen);
    if(result == True)
    {
        if(FM_Read_Reg(pdev, Status2Reg) & 0x08)  return True;
        else  return False;
    }
    
    return False;
}




/* 提供给Mifare的驱动实例 */
__INLINE_STATIC_ void MF_S522_Init(MF_Type *dev)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    void *p, *q;
    
    p = FW_Device_GetDriver(ms);
    if(p == NULL)
    {
        LOG_D("mf s522 %d driver is null\r\n", dev_id);
        return;
    }
    
    q = FW_Driver_Find("spi->dev");
    if(q && p == q)
    {
        /* SPI_GD_Driver */
        ms->Init    = SGP_Init;
        ms->Get_Reg = SGP_Get_Reg;
        ms->Set_Reg = SGP_Set_Reg;
    }
    
    if(ms->Init)  ms->Init(ms);
}

__INLINE_STATIC_ Bool MF_S522_Reset(MF_Type *dev, u8 hs_mode)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    
    if(hs_mode == SOFTWARE)
    {
        FM_Write_Reg(ms, CommandReg, SoftReset);
        FW_Delay_Ms(1);
        FM_Set_BitMask(ms, ControlReg, 0x10);
        return True;
    }
    else
    {
        if(ms->RST_Pin == PIN_NULL)  return False;
        FW_GPIO_Write(ms->RST_Pin, ms->RST_VL);
        FW_Delay_Ms(1);
        FW_GPIO_Write(ms->RST_Pin, !ms->RST_VL);
        FW_Delay_Ms(1);
        return True;
    }
}

__INLINE_STATIC_ Bool MF_S522_LPMConfig(MF_Type *dev, u8 state)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    u8 value;
    
    if(state == Enable)
    {
        value = FM_Read_Reg(ms, CommandReg);
        if((value & 0x10) == 0)                     
        {
            /* 处于正常模式时，关闭输出，进入低功耗模式 */
            FM_Set_RF(ms, 0);
            FM_Set_BitMask(ms, CommandReg, 0x10);
            return True;
        }
    }
    else
    {
        value = FM_Read_Reg(ms, CommandReg);
        if((value & 0x10) != 0)
        {
            /* 处于低功耗模式时，打开输出，退出低功耗模式 */
            FM_Set_RF(ms, 3);
            FM_Clear_BitMask(ms, CommandReg, 0x10);
            return True;
        }
    }
    
    return False;
}

__INLINE_STATIC_ Bool MF_S522_SetCP(MF_Type *dev, MF_CP_Enum cp)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    return FM_Config_ISOType(ms, cp);
}

__INLINE_STATIC_ Bool MF_S522_SetRFM(MF_Type *dev, MF_RFM_Enum mode)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    return (Bool)FM_Set_RF(ms, mode);
}

__INLINE_STATIC_ Bool MF_S522_CardHalt(MF_Type *dev, Bool auth_flag)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    u8 result;
    u8 send_buff[2], rece_buff[1];
    u32 rece_bitlen;
    
    send_buff[0] = 0x50;
    send_buff[1] = 0x00;
    
    FM_Write_Reg(ms, BitFramingReg, 0x00);
    FM_Set_BitMask(ms, TxModeReg, 0x80);
    FM_Set_BitMask(ms, RxModeReg, 0x80);
    if(auth_flag == False)
    {
        /* 明文通讯，需要清除验证标志 */
        FM_Clear_BitMask(ms, Status2Reg, 0x08);
    }
    FM_Set_Timer(ms, 1);
    FM_Clear_FIFO(ms);
    
    result = FM_Comm(ms, Transmit, send_buff, 2, rece_buff, &rece_bitlen);
    if(result != True)  return False;
    return True;
}

__INLINE_STATIC_ Bool MF_S522_CardRequest(MF_Type *dev, u8 *uid)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    u8 result;
    u8 card_type[2];
    u8 ack[3];
    
    /* 寻卡 */
    result = FM_Request(ms, card_type);
    if(result != True)  return False;
    
    /* M1卡[7:6]=00, ID号为4B */
    if((card_type[0] & 0xC0) == 0x00)
    {
        /* 防冲突 */
        result = FM_Anticollision(ms, 0x93, uid);
        if(result != True)  return False;
        
        /* 选卡 */
        result = FM_Select(ms, 0x93, uid, ack);
        if(result != True)  return False;
        
        return True;
    }
    
    /* M1卡[7:6]=01, ID号为7B */
    if((card_type[0] & 0xC0) == 0x40)
    {
        result = FM_Anticollision(ms, 0x93, uid);
        if(result != True)  return False;
        
        result = FM_Select(ms, 0x93, uid, ack);
        if(result != True)  return False;
        
        result = FM_Anticollision(ms, 0x95, uid + 5);
        if(result != True)  return False;
        
        result = FM_Select(ms, 0x95, uid + 5, ack + 1);
        if(result != True)  return False;
        
        return True;
    }
    
    /* M1卡[7:6]=10，ID号为10B */
    if((card_type[0] & 0xC0) == 0x80)
    {
        result = FM_Anticollision(ms, 0x93, uid);
        if(result != True)  return False;
        
        result = FM_Select(ms, 0x93, uid, ack);
        if(result != True)  return False;
        
        result = FM_Anticollision(ms, 0x95, uid + 5);
        if(result != True)  return False;
        
        result = FM_Select(ms, 0x95, uid + 5, ack + 1);
        if(result != True)  return False;
        
        result = FM_Anticollision(ms, 0x97, uid + 10);
        if(result != True)  return False;
        
        result = FM_Select(ms, 0x97, uid + 10, ack + 2);
        if(result != True)  return False;
        
        return True;
    }
    
    return False;
}

__INLINE_STATIC_ Bool MF_S522_CardActive(MF_Type *dev, u8 *skey, u8 block_addr, MF_SKey_Enum skey_t)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    u8 result;
    u8 uid[16];
    u8 dkey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    result = MF_S522_CardRequest(dev, uid);
    if(result == False)  return False;
    
    if(skey == NULL)
    {
        result = FM_Authent(ms, skey_t, block_addr >> 2, dkey, uid);
    }
    else
    {
        result = FM_Authent(ms, skey_t, block_addr >> 2, skey, uid);
    }
    
    if(result != True)  return False;
    return True;
}

__INLINE_STATIC_ Bool MF_S522_BlockRead(MF_Type *dev, u8 *pdata, u8 block_addr)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    u8 send_buff[2], result;
    u32 rece_bitlen;
    
    send_buff[0] = 0x30;
    send_buff[1] = block_addr;
    
    FM_Set_Timer(ms, 1);
    FM_Clear_FIFO(ms);
    result = FM_Comm(ms, Transceive, send_buff, 2, pdata, &rece_bitlen);
    if((result != True) || (rece_bitlen != 16 * 8))  return False;
    return True;
}

__INLINE_STATIC_ Bool MF_S522_BlockWrite(MF_Type *dev, u8 *pdata, u8 block_addr)
{
    MF_S522_Type *ms = FW_Device_GetParent(dev);
    u8 send_buff[16], rece_buff[1], result;
    u32 rece_bitlen;
    
    send_buff[0] = 0xA0;
    send_buff[1] = block_addr;
    
    FM_Set_Timer(ms, 1);
    FM_Clear_FIFO(ms);
    result = FM_Comm(ms, Transceive, send_buff, 2, rece_buff, &rece_bitlen);
    if((result != True) || ((rece_buff[0] & 0x0F) != 0x0A))  return False;
    
    FM_Set_Timer(ms, 5);
    FM_Clear_FIFO(ms);
    result = FM_Comm(ms, Transceive, pdata, 16, rece_buff, &rece_bitlen);
    if((result != True) || ((rece_buff[0] & 0x0F) != 0x0A))  return False;
    return True;
}




/**
 * @ Mifare Driver
 */
__CONST_STATIC_ MF_Driver_Type MF_S522_Driver = 
{
    .Init = MF_S522_Init,
    
    .Reset = MF_S522_Reset,
    .LPM_Config = MF_S522_LPMConfig,
    .Set_CP = MF_S522_SetCP,
    .Set_RFM = MF_S522_SetRFM,
    
    .Card_Halt = MF_S522_CardHalt,
    .Card_Request = MF_S522_CardRequest,
    .Card_Active = MF_S522_CardActive,
    
    .Block_Read = MF_S522_BlockRead,
    .Block_Write = MF_S522_BlockWrite,
};
FW_DRIVER_REGIST("17522->mf", &MF_S522_Driver, FM17522);




#include "project_debug.h"
#if MODULE_TEST && SPI_DEVICE_TEST && FM17522_TEST
#include "fw_system.h"
#include "fw_delay.h"

#include "mm.h"

#include <string.h>


//#define SPI_DEV_NAME  "hspi1"
#define SPI_DEV_NAME  "sspi0"

static FW_SPI_Type SSPI0;
static void SSPI0_Config(void *dev)
{
    FW_SPI_Driver_Type *drv = FW_Driver_Find("io->spi");
    FW_Device_SetDriver(&SSPI0, drv);
}
FW_DEVICE_STATIC_REGIST("sspi0", &SSPI0, SSPI0_Config, SSPI0);

/*  */
static MF_S522_Type FM17522;

static void FM17522_Config(void *pdev)
{
    FW_SPI_Driver_Type *drv = FW_Driver_Find("spi->dev");
    FW_Device_SetDriver(&FM17522, drv);
    FM17522.Device.Parent = FW_Device_Find(SPI_DEV_NAME);
    FM17522.RST_Pin = PE0;
}
FW_DEVICE_STATIC_REGIST("fm17522", &FM17522, FM17522_Config, FM17522);


/*  */
//#define MIFARE_DEV_ID      FW_DEVICE_ID(FW_DT_Mifare, MF_S522_DRV_NUM, NULL, NULL)
#define MIFARE_DEV_ID   "mifare"

static MF_Type Mifare;

static void Mifare_Config(void *pdev)
{
    FW_Device_SetDriver(&Mifare, (void *)&MF_S522_Driver);
    
    Mifare.Device.Parent = (void *)&FM17522;
}
FW_DEVICE_STATIC_REGIST("mifare", &Mifare, Mifare_Config, Mifare);


void Test(void)
{
    MF_Type *mf;
    volatile FW_SPI_Type *spi;
    
    u16 VCC_EN = PB7;
    
    u8 msg[16];
    u8 skey[6] = {0x57,0x49,0x4E,0x5A,0x49,0x48};
    u8 addr;
    u8 i;
    
    MF_State_Enum state;
    
    u8 m1_info[1024];
    u32 offset = 0;
    
    
//    FW_System_Init();
//    FW_Delay_Init();
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    
    mf = FW_Device_Find(MIFARE_DEV_ID);
    if(mf == NULL)  while(1);
    
    spi = FW_Device_Find(SPI_DEV_NAME);
    if(spi == NULL)  while(1);
    
//    spi->CS_Pin = PB12;
//    spi->SCK_Pin = PB13;
//    spi->MISO_Pin = PB14;
//    spi->MOSI_Pin = PB15;
    spi->CS_Pin = PB3;
    spi->SCK_Pin = PB4;
    spi->MISO_Pin = PB5;
    spi->MOSI_Pin = PB6;
    
    spi->Baudrate = 5000000;
    spi->First_Bit = FW_SPI_FirstBit_MSB;
    spi->Clock_Polarity = FW_SPI_ClockPolarity_L;
    spi->Clock_Phase = FW_SPI_ClockPhase_Edge1;
    
    mf->RF_Mode = MF_RFM_All;
    
    MF_Init(mf);
    
    for(i = 0; i < 64; i++)
    {
        state = MF_Block_Read(mf, NULL, msg, i);
        if(state != MF_State_OK)
        {
            if(state == MF_State_SKeyErr)
            {
                state = MF_Block_Read(mf, skey, msg, i);
            }
        }
        
        if(state == MF_State_OK)
        {
            memcpy(m1_info + offset, msg, sizeof(msg));
        }
        else
        {
            memset(m1_info + offset, 0x55, sizeof(msg));
        }
        
        offset += sizeof(msg);
    }
    
    while(1);
}


#endif

