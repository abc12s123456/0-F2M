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
 
#ifndef _FW_FLASH_H_
#define _FW_FLASH_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "fw_device.h"


/* Flash驱动编号 */
#define FLASH_DRV_ONCHIP_NUM      0   //内部存储
#define FLASH_DRV_SNOR_NUM        1   //外部串行NOR Flash

#define FLASH_CID_INVALUE         0x55555555   //CID无效值


typedef enum
{
    FW_Flash_State_OK = 0,        //OK
    FW_Flash_State_Busy,          //忙
    FW_Flash_State_Timeout,       //超时
    FW_Flash_State_AddrErr,       //地址错误
    FW_Flash_State_DataErr,       //数据错误
    FW_Flash_State_AcceErr,       //不允许访问
    FW_Flash_State_ArgErr,        //参数错误
}FW_Flash_State_Enum, ffse;


typedef enum
{
    FW_Flash_Protect_Read  = 0x01,     //读保护(禁止外部读取)
    FW_Flash_Protect_Write = 0x02,     //写保护(禁止任意写)
    FW_Flash_Protect_RW    = 0x03,     //读写保护
    FW_Flash_Protect_Erase = 0x04,     //擦除保护(禁止擦除)
    FW_Flash_Protect_OB    = 0x08,     //设置字节保护()
}FW_Flash_Protect_Enum, ffpe;


typedef struct
{
    u32 Base;
    u32 Size;
}FW_Flash_Space_Type, ffst;


typedef struct
{
    u32 Operate : 4;
    
    u32 : 28;
    
    /* 起始地址 */
    u32 Start_Addr;
    u32 End_Addr;
}FW_Flash_Protect_Type, ffpt;


/* Flash属于闪存存储的基类 */
typedef struct FW_Flash
{
    FW_Device_Type Device;
    
    /* 操作缓存 */
    __RO_ u8  *Buffer;
    
    /* 最小读写单元的大小(Min Write/Read Unit Size)，单位：Byte */
//    __RO_ u32 MWU_Size;
//    __RO_ u32 MRU_Size;
    
    /* 主区域 */
    ffst Main;
    
    /* Chip ID */
    u32  CID;
}FW_Flash_Type;


/* Driver中的addr统一为绝对地址(直接可以对底层进行操作) */
typedef struct FW_Flash_Driver
{
    void (*Init)(FW_Flash_Type *dev);
    
    u32  (*Get_SectorNum)(FW_Flash_Type *dev);
    u32  (*Get_SectorBase)(FW_Flash_Type *dev, u32 index);
    u32  (*Get_SectorSize)(FW_Flash_Type *dev, u32 index);
    
    ffst (*Get_Main)(FW_Flash_Type *dev);                                      /* 主区域 */
    ffst (*Get_OTP)(FW_Flash_Type *dev);                                       /* OTP区域 */
    ffst (*Get_CID)(FW_Flash_Type *dev);                                       /* Chip ID */
    ffst (*Get_OB)(FW_Flash_Type *dev);                                        /* Option Byte */
    
    Bool (*Set_PE)(FW_Flash_Type *dev);
    Bool (*Get_PE)(FW_Flash_Type *dev);                                        /* 获取程序加密状态 */
    
    ffse (*Erase_Chip)(FW_Flash_Type *dev);
    ffse (*Erase_Sector)(FW_Flash_Type *dev, u32 addr);                        /*  */
    
    ffse (*Write)(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num);   /*  */
    u32  (*Read)(FW_Flash_Type *dev, u32 addr, void *pdata, u32 num);          /*  */
}FW_Flash_Driver_Type, ffdt;


/**
 * @API
 */
void Flash_Init(FW_Flash_Type *dev);

u32  Flash_Get_Base(FW_Flash_Type *dev);
u32  Flash_Get_Size(FW_Flash_Type *dev);

u32  Flash_Get_SectorNum(FW_Flash_Type *dev);
u32  Flash_Get_SectorIndex(FW_Flash_Type *dev, u32 addr);
u32  Flash_Get_SectorBase(FW_Flash_Type *dev, u32 addr);
u32  Flash_Get_SectorSize(FW_Flash_Type *dev, u32 addr);

u32  Flash_Get_CID(FW_Flash_Type *dev, void *cid, u32 num);
Bool Flash_Set_PE(FW_Flash_Type *dev);
Bool Flash_Get_PE(FW_Flash_Type *dev);

ffse Flash_Erase_Chip(FW_Flash_Type *dev);
ffse Flash_Erase_Sector(FW_Flash_Type *dev, u32 addr);
ffse Flash_Erase_Inter(FW_Flash_Type *dev, u32 start_addr, u32 end_addr);

ffse Flash_Write_Direct(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num);
ffse Flash_Write_Unsafe(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num);
ffse Flash_Write(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num);

u32  Flash_Read(FW_Flash_Type *dev, u32 addr, void *pdata, u32 num);


extern FW_Flash_Type IFlash;

#if defined(FLASH_RELOAD_EN) && (FLASH_RELOAD_EN)

#define FW_Flash_Init(...)\
do{\
    u8 n = VA_NUM(__VA_ARGS__);\
    if(n == 0)  Flash_Init(&IFlash);\
    else if(n == 1)  Flash_Init((FW_Flash_Type *)VA0(__VA_ARGS__));\
    else  VA_ARGS_NUM_ERR();\
}while(0)

#define FW_Flash_GetBase(...)(\
(VA_NUM(__VA_ARGS__) == 0) ? Flash_Get_Base(&IFlash) :\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Get_Base((FW_Flash_Type *)VA0(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_Flash_GetSize(...)(\
(VA_NUM(__VA_ARGS__) == 0) ? Flash_Get_Size(&IFlash) :\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Get_Size((FW_Flash_Type *)VA0(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_Flash_GetSectorNum(...)(\
(VA_NUM(__VA_ARGS__) == 0) ? Flash_Get_SectorNum(&IFlash) :\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Get_SectorNum((FW_Flash_Type *)VA0(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_Flash_GetSectorIndex(...)(\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Get_Index(&IFlash, (u32)VA0(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 2) ? Flash_Get_Index((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_Flash_GetSectorBase(...)(\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Get_Base(&IFlash, (u32)VA0(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 2) ? Flash_Get_Base((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_Flash_GetSectorSize(...)(\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Get_Size(&IFlash, (u32)VA0(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 2) ? Flash_Get_Size((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_Flash_GetCID(...)(\
(VA_NUM(__VA_ARGS__) == 2) ? Flash_Get_CID(&IFlash, (void *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 3) ? Flash_Get_CID((FW_Flash_Type *)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__), (u32)VA2(__VA_ARGS__)) :\
VA_ARGS_NUM_ERR())

#define FW_Flash_SetPE(...)(\
(VA_NUM(__VA_ARGS__) == 0) ? Flash_Set_PE(&IFlash) :\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Set_PE((FW_Flash_Type *)VA0(__VA_ARGS__)) :\
(Bool)VA_ARGS_NUM_ERR())

#define FW_Flash_GetPE(...)(\
(VA_NUM(__VA_ARGS__) == 0) ? Flash_Get_PE(&IFlash) :\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Get_PE((FW_Flash_Type *)VA0(__VA_ARGS__)) :\
(Bool)VA_ARGS_NUM_ERR())

#define FW_Flash_EraseChip(...)(\
(VA_NUM(__VA_ARGS__) == 0) ? Flash_Erase_Chip(&IFlash) :\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Erase_Chip((FW_Flash_Type *)VA0(__VA_ARGS__)) :\
(ffse)VA_ARGS_NUM_ERR())

#define FW_Flash_EraseSector(...)(\
(VA_NUM(__VA_ARGS__) == 1) ? Flash_Erase_Sector(&IFlash, (u32)VA0(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 2) ? Flash_Erase_Sector((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__)) :\
(ffse)VA_ARGS_NUM_ERR())

#define FW_Flash_EraseInter(...)(\
(VA_NUM(__VA_ARGS__) == 2) ? Flash_Erase_Inter(&IFlash, (u32)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 3) ? Flash_Erase_Inter((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__), (u32)VA2(__VA_ARGS__)) :\
(ffse)VA_ARGS_NUM_ERR())

#define FW_Flash_WriteDirect(...)(\
(VA_NUM(__VA_ARGS__) == 3) ? Flash_Write_Direct(&IFlash, (u32)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__), (u32)VA2(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 4) ? Flash_Write_Direct((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__), (void *)VA2(__VA_ARGS__), (u32)VA3(__VA_ARGS__)) :\
(ffse)VA_ARGS_NUM_ERR())

#define FW_Flash_WriteUnsafe(...)(\
(VA_NUM(__VA_ARGS__) == 3) ? Flash_Write_Unsafe(&IFlash, (u32)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__), (u32)VA2(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 4) ? Flash_Write_Unsafe((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__), (void *)VA2(__VA_ARGS__), (u32)VA3(__VA_ARGS__)) :\
(ffse)VA_ARGS_NUM_ERR())

#define FW_Flash_Write(...)(\
(VA_NUM(__VA_ARGS__) == 3) ? Flash_Write(&IFlash, (u32)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__), (u32)VA2(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 4) ? Flash_Write((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__), (void *)VA2(__VA_ARGS__), (u32)VA3(__VA_ARGS__)) :\
(ffse)VA_ARGS_NUM_ERR())

#define FW_Flash_Read(...)(\
(VA_NUM(__VA_ARGS__) == 3) ? Flash_Read(&IFlash, (u32)VA0(__VA_ARGS__), (void *)VA1(__VA_ARGS__), (u32)VA2(__VA_ARGS__)) :\
(VA_NUM(__VA_ARGS__) == 4) ? Flash_Read((FW_Flash_Type *)VA0(__VA_ARGS__), (u32)VA1(__VA_ARGS__), (void *)VA2(__VA_ARGS__), (u32)VA3(__VA_ARGS__)) :\
(ffse)VA_ARGS_NUM_ERR())

#else

__INLINE_STATIC_ void FW_Flash_Init(void)
{
    Flash_Init(&IFlash);
}

__INLINE_STATIC_ u32  FW_Flash_GetBase(void)
{
    return Flash_Get_Base(&IFlash);
}

__INLINE_STATIC_ u32  FW_Flash_GetSize(void)
{
    return Flash_Get_Size(&IFlash);
}


__INLINE_STATIC_ u32  FW_Flash_GetSectorNum(void)
{
    return Flash_Get_SectorNum(&IFlash);
}

__INLINE_STATIC_ u32  FW_Flash_GetSectorIndex(u32 addr)
{
    return Flash_Get_SectorIndex(&IFlash, addr);
}

__INLINE_STATIC_ u32  FW_Flash_GetSectorBase(u32 addr)
{
    return Flash_Get_SectorBase(&IFlash, addr);
}

__INLINE_STATIC_ u32  FW_Flash_GetSectorSize(u32 addr)
{
    return Flash_Get_SectorSize(&IFlash, addr);
}

__INLINE_STATIC_ u32  FW_Flash_GetCID(void *cid, u32 num)
{
    return Flash_Get_CID(&IFlash, cid, num);
}

__INLINE_STATIC_ Bool FW_Flash_SetPE(void)
{
    return Flash_Set_PE(&IFlash);
}

__INLINE_STATIC_ Bool FW_Flash_GetPE(void)
{
    return Flash_Get_PE(&IFlash);
}

__INLINE_STATIC_ ffse FW_Flash_EraseChip(void)
{
    return Flash_Erase_Chip(&IFlash);
}

__INLINE_STATIC_ ffse FW_Flash_EraseSector(u32 addr)
{
    return Flash_Erase_Sector(&IFlash, addr);
}

__INLINE_STATIC_ ffse FW_Flash_EraseInter(u32 start_addr, u32 end_addr)
{
    return Flash_Erase_Inter(&IFlash, start_addr, end_addr);
}


__INLINE_STATIC_ ffse FW_Flash_WriteDirect(u32 addr, const void *pdata, u32 num)
{
    return Flash_Write_Direct(&IFlash, addr, pdata, num);
}

__INLINE_STATIC_ ffse FW_Flash_WriteUnsafe(u32 addr, const void *pdata, u32 num)
{
    return Flash_Write_Unsafe(&IFlash, addr, pdata, num);
}

__INLINE_STATIC_ ffse FW_Flash_Write(u32 addr, const void *pdata, u32 num)
{
    return Flash_Write(&IFlash, addr, pdata, num);
}

__INLINE_STATIC_ u32  FW_Flash_Read(u32 addr, void *pdata, u32 num)
{
    return Flash_Read(&IFlash, addr, pdata, num);
}

#endif  /* #if defined(FLASH_RELOAD_EN) && (FLASH_RELOAD_EN) */


/**
 * @串行NOR Flash闪存
 */
#define SNOR_DRV_GP_NUM      0         //通用驱动编号

 
typedef struct SNOR SNOR_Type;

    
struct SNOR
{
    FW_Device_Type Device;
    
    u32  Hold_Pin : 16;
    u32  WP_Pin : 16;
    
    /* 有效电平值 */
    u32  Hold_VL : 1;
    u32  WP_VL : 1;
    
    u32  : 30;
    
    void (*Init)(SNOR_Type *dev);
    ffst (*Get_CID)(SNOR_Type *dev);
    ffse (*Erase_Chip)(SNOR_Type *dev);
    ffse (*Erase_Sector)(SNOR_Type *dev, u32 addr);
    ffse (*Write)(SNOR_Type *dev, u32 addr, const void *pdata, u32 num);
    u32  (*Read)(SNOR_Type *dev, u32 addr, void *pdata, u32 num);
};


/* 串行NOR Flash通用驱动 */
typedef struct
{
    void (*Init)(SNOR_Type *dev);
    
    ffst (*Get_CID)(SNOR_Type *dev);
    
    ffse (*Erase_Chip)(SNOR_Type *dev);
    ffse (*Erase_Sector)(SNOR_Type *dev, u32 addr);
    
    ffse (*Write)(SNOR_Type *dev, u32 addr, const void *pdata, u32 num);
    u32  (*Read)(SNOR_Type *dev, u32 addr, void *pdata, u32 num);
}SNOR_Driver_Type;


#ifdef __cplusplus
}
#endif

#endif

