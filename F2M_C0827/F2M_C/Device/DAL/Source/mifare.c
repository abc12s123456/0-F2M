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
#include "mifare.h"
#include "fw_debug.h"
#include "fw_print.h"

#include "libc.h"


void MF_Init(MF_Type *dev)
{
    MF_Driver_Type *drv;
    Bool state;
    
    FW_ASSERT(dev);
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    FW_ASSERT(drv->Init);
    FW_ASSERT(drv->Reset);
    FW_ASSERT(drv->Set_RFM);
    FW_ASSERT(drv->Set_CP);
    
    if(drv->Init)  drv->Init(dev);
    
    if(drv->Reset)  
    {
        state = drv->Reset(dev, SOFTWARE);
        if(state != True)
        {
            drv->Reset(dev, HARDWARE);
        }
    }
    
    if(drv->Set_RFM)  drv->Set_RFM(dev, (MF_RFM_Enum)dev->RF_Mode);
    if(drv->Set_CP)  drv->Set_CP(dev, (MF_CP_Enum)dev->Comm_Protocol);
}

Bool MF_Reset(MF_Type *dev, u8 hs_mode)
{
    MF_Driver_Type *drv = FW_Device_GetDriver(dev);
    Bool state = False;
    
    FW_ASSERT(drv->Reset);
    
    if(drv->Reset)  
    {
        state = drv->Reset(dev, hs_mode);
    }
    
    return state;
}

Bool MF_LPM_Config(MF_Type *dev, u8 state)
{
    MF_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->LPM_Config);
    
    if(drv->LPM_Config)  return drv->LPM_Config(dev, state);
    
    return False;
}

Bool MF_RF_Config(MF_Type *dev, MF_RFM_Enum mode)
{
    MF_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    return drv->Set_RFM(dev, mode);
}

Bool MF_Card_Halt(MF_Type *dev, Bool auth_flag)
{
    MF_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Card_Halt);
    
    if(drv->Card_Halt)  return drv->Card_Halt(dev, auth_flag);
    
    return False;
}

MF_SKey_Enum MF_Card_Active(MF_Type *dev, u8 *skey, u8 block_addr)
{
    MF_Driver_Type *drv = FW_Device_GetDriver(dev);
    Bool state;
    
    FW_ASSERT(drv->Card_Active);
    
    if(drv->Card_Active)  
    {
        state = drv->Card_Active(dev, skey, block_addr, MF_SKey_A);
        if(state != True)
        {
            state = drv->Card_Active(dev, skey, block_addr, MF_SKey_B);
            if(state != True)
            {
                return MF_SKey_Err;
            }
            else
            {
                return MF_SKey_B;
            }
        }
        else
        {
            return MF_SKey_A;
        }
    }
    
    return MF_SKey_Err;
}

MF_State_Enum MF_Card_Request(MF_Type *dev)
{
    MF_Driver_Type *drv = FW_Device_GetDriver(dev);
    Bool state;
    
//    state = MF_Card_Halt(pdev, False);
//    if(state != True)  return MF_State_HaltErr;
    
    FW_ASSERT(drv->Card_Request);
    
    if(drv->Card_Request)  
    {
        state = drv->Card_Request(dev, (u8 *)dev->UID);
    }
    else  
    {
        return MF_State_NoCard;
    }
    
    /* 执行一次激活操作，否则当卡一直在时，会导致读卡失败 */
    MF_Card_Active(dev, NULL, 0);
    
    return state ? MF_State_OK : MF_State_NoCard;
}

MF_State_Enum MF_Block_Read(MF_Type *dev, u8 *skey, u8 *pdata, u8 block_addr)
{
    MF_Driver_Type *drv = FW_Device_GetDriver(dev);
    Bool state;
    MF_SKey_Enum skey_s;
    
    state = MF_Card_Halt(dev, False);
    if(state != True)  return MF_State_HaltErr;
    
    skey_s = MF_Card_Active(dev, skey, block_addr);
    if(skey_s == MF_SKey_Err)  return MF_State_SKeyErr;
    
    FW_ASSERT(drv->Block_Read);
    
    state = drv->Block_Read(dev, pdata, block_addr);
    if(state != True)  return MF_State_ComErr;
    
    return MF_State_OK;
}

MF_State_Enum MF_Block_Write(MF_Type *dev, u8 *skey, u8 *pdata, u8 block_addr)
{
    MF_Driver_Type *drv = FW_Device_GetDriver(dev);
    Bool state;
    MF_SKey_Enum skey_s;
    
    state = MF_Card_Halt(dev, False);
    if(state != True)  return MF_State_HaltErr;
    
    skey_s = MF_Card_Active(dev, skey, block_addr);
    if(skey_s == MF_SKey_Err)  return MF_State_SKeyErr;
    
    FW_ASSERT(drv->Block_Write);
    
    state = drv->Block_Write(dev, pdata, block_addr);
    if(state != True)  return MF_State_ComErr;
    
    return MF_State_OK;
}

/**
@功能：写数据块秘钥(修改秘钥)
@参数：pdev, 操作对象
       o_skey, 老秘钥
       n_skey, 需要写入的新秘钥
       block_addr, 需要更新秘钥的数据块
       skey_t, 秘钥类型，决定修改KeyA还是KeyB
@返回：其它，秘钥修改失败
       MF_State_OK, 秘钥修改成功
@说明：@修改秘钥前必须知道当前的秘钥;
*/
MF_State_Enum MF_Skey_Write(MF_Type *dev, u8 *o_skey, u8 *n_skey, u8 block_addr, MF_SKey_Enum skey_t)
{
    MF_State_Enum state;
    u8 msg[16];
    
    /* 调整为控制块地址 */
    block_addr = ((block_addr >> 2) << 2) + 3;

    /* 读取秘钥块信息 */
    state = MF_Block_Read(dev, o_skey, msg, block_addr);
    if(state != MF_State_OK)  return state;
    
    /* 修改原秘钥 */
    if(skey_t == MF_SKey_A)  memcpy(msg, n_skey, 6);
    else  memcpy(msg + 10, n_skey, 6);
    
    /* 写入修改后的秘钥 */
    state = MF_Block_Write(dev, o_skey, msg, block_addr);
    
    return state;
}

__INLINE_STATIC_ void Bit_Set(u8 *pdata, u8 bit_position, u8 value)
{
    if(value)  *pdata |= (1 << bit_position);
    else       *pdata &= ~(1 << bit_position);
}

MF_State_Enum MF_CB_Write(MF_Type *dev, u8 *skey, u8 block_addr, u8 cb_value)
{
    MF_State_Enum state;
    u8 msg[16];
    u8 b1, b2, b3;
    u8 i;
    
    i = block_addr % 4;
    block_addr = ((block_addr >> 2) << 2) + 3;
    
    b1 = (cb_value >> 2) & 0x01;
    b2 = (cb_value >> 1) & 0x01;
    b3 = (cb_value >> 0) & 0x01;
    
    state = MF_Block_Read(dev, skey, msg, block_addr);
    if(state != MF_State_OK)  return state;
    
    Bit_Set(&msg[7], i + 4, b1);
    Bit_Set(&msg[6], i, !b1);
    
    Bit_Set(&msg[6], i + 4, !b2);
    Bit_Set(&msg[8], i, b2);
    
    Bit_Set(&msg[8], i + 4, b3);
    Bit_Set(&msg[7], i, !b3);
    
    state = MF_Block_Write(dev, skey, msg, block_addr);
    return state;
}

__INLINE_STATIC_ u8 Bit_Get(u8 *pdata, u8 bit_position)
{
    if(*pdata & (1 << bit_position))  return 1;
    else  return 0;
}

/**/
u8   MF_CB_Read(MF_Type *dev, u8 *skey, u8 block_addr, u8 cb_value)
{
    MF_State_Enum state;
    u8 msg[16];
    u8 b1, b2, b3;
    u8 i;
    
    i = block_addr % 4;
    block_addr = ((block_addr >> 2) << 2) + 3;
    
    b1 = (cb_value >> 2) & 0x01;
    b2 = (cb_value >> 1) & 0x01;
    b3 = (cb_value >> 0) & 0x01;
    
    state = MF_Block_Read(dev, skey, msg, block_addr);
    if(state != MF_State_OK)  return state;
    
    if(Bit_Get(&msg[7], i + 4) != Bit_Get(&msg[6], i) &&
       Bit_Get(&msg[6], i + 4) != Bit_Get(&msg[8], i) &&
       Bit_Get(&msg[8], i + 4) != Bit_Get(&msg[7], i))
    {
        b1 = Bit_Get(&msg[7], i + 4);
        b2 = Bit_Get(&msg[6], i);
        b3 = Bit_Get(&msg[8], i + 4);
    }
    else  return 0;
    
    return ((b1 << 2) + (b2 << 1) + b3);
}

MF_State_Enum MF_Sector_Restore(MF_Type *dev, u8 *skey, u8 sector_addr)
{
    MF_State_Enum state;
    u8 msg[16] = {0};
    u8 block_addr;
    
    block_addr = sector_addr * 4;
    
    /* 扇区块0，0扇区除外 */
    if(block_addr)
    {
        state = MF_Block_Write(dev, skey, msg, block_addr);
        if(state != MF_State_OK)  return state;
    }
    
    /* 块1 */
    state = MF_Block_Write(dev, skey, msg, block_addr + 1);
    if(state != MF_State_OK)  return state;
    
    /* 块2 */
    state = MF_Block_Write(dev, skey, msg, block_addr + 2);
    if(state != MF_State_OK)  return state;
    
    memset(msg, 0xFF, sizeof(msg));
    msg[7] = 0x07, msg[8] = 0x80, msg[9] = 0x69;
    
    /* 块3，控制块 */
    state = MF_Block_Write(dev, skey, msg, block_addr + 3);
    return state;
}


