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
 
#ifndef _MIFARE_H_
#define _MIFARE_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_device.h"


#define MF_S522_DRV_NUM           0x00


typedef enum
{
    MF_State_OK = 0,              //操作成功
    MF_State_NoCard,              //无卡
    MF_State_HaltErr,             //待机错误
    MF_State_SKeyErr,             //密钥错误
    MF_State_ComErr,              //通讯
}MF_State_Enum;


typedef enum
{
    MF_SKey_Err = 0,              //错误秘钥
    MF_SKey_A,
    MF_SKey_B,
}MF_SKey_Enum;


typedef enum
{
    MF_CardType_S50,
    MF_CardType_S70,
}MF_CardType_Enum;


/* 通讯协议 */
typedef enum
{
    MF_CP_14443A = 0,             //ISO14443A
    MF_CP_14443B,                 //ISO14443B
    MF_CP_15693,                  //ISO15693
}MF_CP_Enum;


/* 射频输出模式 */
typedef enum
{
    MF_RFM_Close = 0,             //关闭输出
    MF_RFM_TX1,                   //仅TX1输出
    MF_RFM_TX2,                   //仅TX2输出
    MF_RFM_All,                   //全部输出
}MF_RFM_Enum;


typedef struct
{
    FW_Device_Type Device;
    
    u32 UID[8];                        //卡唯一ID，一般为32位
    
    u32 Card_Type : 8;                 //卡类型
    u32 Block_Num : 8;                 //扇区数量
    u32 Comm_Protocol : 4;             //通讯协议
    u32 RF_Mode : 2;                   //天线打开模式，参考MF_RFM_Enum
    
    u32 : 10;
}MF_Type;


typedef struct MF_Driver
{
    void (*Init)(MF_Type *dev);
    
    Bool (*Reset)(MF_Type *dev, u8 hs_mode);
    Bool (*LPM_Config)(MF_Type *dev, u8 state);
    Bool (*Set_CP)(MF_Type *dev, MF_CP_Enum cp);
    Bool (*Set_RFM)(MF_Type *dev, MF_RFM_Enum mode);
    
    Bool (*Card_Halt)(MF_Type *dev, Bool auth_flag);                                //中止
    Bool (*Card_Request)(MF_Type *dev, u8 *uid);                                    //请求
    Bool (*Card_Active)(MF_Type *dev, u8 *skey, u8 block_addr, MF_SKey_Enum skey_t);//激活
    
    Bool (*Block_Read)(MF_Type *dev, u8 *pdata, u8 block_addr);
    Bool (*Block_Write)(MF_Type *dev, u8 *pdata, u8 block_addr);
    
    MF_State_Enum (*SInc)(MF_Type *dev, u8 *pdata, u8 block_addr);      //自加
    MF_State_Enum (*SDec)(MF_Type *dev, u8 *pdata, u8 block_addr);      //自减
    MF_State_Enum (*Srecovery)(MF_Type *dev, u8 *pdata);                //自恢复
    MF_State_Enum (*EWStore)(MF_Type *dev, u8 block_addr);              //电子钱包保存
    
    MF_State_Enum (*ARead)(MF_Type *dev, u8 *password, u8 *pdata, u8 block_addr);  //高级读
    MF_State_Enum (*AWrite)(MF_Type *dev, u8 *password, u8 *pdata, u8 block_addr); //高级写
}MF_Driver_Type;


void MF_Init(MF_Type *dev);

Bool MF_Reset(MF_Type *dev, u8 hs_mode);
Bool MF_LPM_Config(MF_Type *dev, u8 state);
Bool MF_RF_Config(MF_Type *dev, MF_RFM_Enum mode);

MF_State_Enum MF_Card_Request(MF_Type *dev);

MF_State_Enum MF_Block_Read(MF_Type *dev, u8 *skey, u8 *pdata, u8 block_addr);
MF_State_Enum MF_Block_Write(MF_Type *dev, u8 *skey, u8 *pdata, u8 block_addr);

MF_State_Enum MF_Skey_Write(MF_Type *dev, u8 *o_skey, u8 *n_skey, u8 block_addr, MF_SKey_Enum skey_t);
MF_State_Enum MF_Skey_Reset(MF_Type *dev, u8 *skey);

MF_State_Enum MF_CB_Write(MF_Type *dev, u8 *skey, u8 block_addr, u8 cb_value);
u8   MF_CB_Read(MF_Type *dev, u8 *skey, u8 block_addr, u8 cb_value);

MF_State_Enum MF_Sector_Restore(MF_Type *dev, u8 *skey, u8 sector_addr);




/* 5xx系列NFC驱动 */
typedef struct MF_S522 MF_S522_Type;

struct MF_S522
{
    __RO_ FW_Device_Type Device;
    
    u16 RST_Pin;
    u16 IRQ_Pin;
    
    u32 RST_VL : 1;
    u32 : 31;
    
    __RO_ void (*Init)(MF_S522_Type *dev);
    __RO_ u8   (*Get_Reg)(MF_S522_Type *pdev, u8 reg_addr);
    __RO_ void (*Set_Reg)(MF_S522_Type *pdev, u8 reg_addr, u8 value);
};


#ifdef __cplusplus
}
#endif

#endif

