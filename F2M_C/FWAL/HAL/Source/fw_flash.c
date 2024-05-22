#include "fw_flash.h"

#include "fw_debug.h"
#include "fw_print.h"

#include "mm.h"


FW_Flash_Type IFlash;

static void IFlash_Driver_Probe(void *dev)
{
    FW_Flash_Driver_Type **drv = (FW_Flash_Driver_Type **)(&IFlash.Device.Driver);
    *drv = FW_Driver_Find("ll->flash");
    if(*drv == NULL)
    {
        /* internal flash driver is not instanced */
        LOG_D("未找到内部闪存驱动\r\n");
        while(1);
    }
}
FW_DEVICE_STATIC_REGIST("iflash", &IFlash, IFlash_Driver_Probe, IFlash);


void Flash_Init(FW_Flash_Type *dev)
{
    FW_Flash_Driver_Type *drv;
    
    FW_ASSERT(dev);
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv);
    
    FW_ASSERT(drv->Init);
    FW_ASSERT(drv->Get_Main);
    
    if(drv->Init)  drv->Init(dev);
    if(drv->Get_Main)  dev->Main = drv->Get_Main(dev);
}
/**/

u32  Flash_Get_Base(FW_Flash_Type *dev)
{
    return dev->Main.Base;
}
/**/

u32  Flash_Get_Size(FW_Flash_Type *dev)
{
    return dev->Main.Size;
}
/**/

u32  Flash_Get_SectorNum(FW_Flash_Type *dev)
{
    FW_Flash_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Get_SectorNum);
    
    return drv->Get_SectorNum(dev);
}
/**/

/**
@功能: 获取指定地址的扇区索引
@参数: pdev, 操作对象
       addr, Flash地址(相对地址)
@返回: INVALID_INDEX, 错误
       addr所在扇区的索引
@备注: 使用二分法查找，提高检索速度
*/
u32  Flash_Get_SectorIndex(FW_Flash_Type *dev, u32 addr)
{
    FW_Flash_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    u32 base;
    u32 num;
    u32 left = 0;
    s32 right = 0;
    u32 index = 0;
    s8 flag;
    
    FW_ASSERT(drv->Get_SectorNum);
    FW_ASSERT(drv->Get_SectorBase);
    
    num = drv->Get_SectorNum(dev);
    right = num - 1;
    
    /* 相对地址转换为绝对地址 */
    addr += Flash_Get_Base(dev);
    
    while(left <= right)
    {
        index = (left + right) >> 1;
        base = drv->Get_SectorBase(dev, index);
        
        if(addr > base)
        {
            left = index + 1;
            flag = 1;
        }
        else if(addr < base)
        {
            right = index - 1;
            flag = -1;
        }
        else
        {
            return index;
        }
    }
    
    return (flag == 1) ? index : right;
}
/**/

/* 返回值为绝对地址 */
u32  Flash_Get_SectorBase(FW_Flash_Type *dev, u32 addr)
{
    FW_Flash_Driver_Type *drv;
    u32 index;
    
    if(addr > Flash_Get_Size(dev))
    {
        LOG_D("地址%x超出范围\r\n", addr);
        return 0;
    }
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Get_SectorBase);
    
    index = Flash_Get_SectorIndex(dev, addr);
    return drv->Get_SectorBase(dev, index);
}
/**/

u32  Flash_Get_SectorSize(FW_Flash_Type *dev, u32 addr)
{
    FW_Flash_Driver_Type *drv;
    u32 index;
    
    if(addr > Flash_Get_Size(dev))
    {
        LOG_D("地址%x超出范围\r\n", addr);
        return 0;
    }
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Get_SectorSize);
    
    index = Flash_Get_SectorIndex(dev, addr);
    return drv->Get_SectorSize(dev, index);
}
/**/

u32 Flash_Get_CID(FW_Flash_Type *dev, void *cid, u32 num)
{
    FW_Flash_Driver_Type *drv = FW_Device_GetDriver(dev);
    ffst space;
    
    FW_ASSERT(drv->Get_CID);
    
    space = drv->Get_CID(dev);
    
    /* 给定的缓存不足以保存cid数据 */
    if(num < space.Size)  return FW_Flash_State_AcceErr;
    
    if(space.Size)
    {
        /* CID = 0时，表示CID信息存储在*cid中 */
        dev->CID = 0;
        return Flash_Read(dev, space.Base, cid, space.Size);
    }
    else
    {
        if(space.Base == 0)  dev->CID = FLASH_CID_INVALUE;
        else  dev->CID = space.Base;
        return dev->CID;
    }
}
/**/

Bool Flash_Set_PE(FW_Flash_Type *dev)
{
    FW_Flash_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Set_PE);
    
    return drv->Set_PE(dev);
}

Bool Flash_Get_PE(FW_Flash_Type *dev)
{
    FW_Flash_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Get_PE);
    
    return drv->Get_PE(dev);
}
/**/

ffse Flash_Erase_Chip(FW_Flash_Type *dev)
{
    FW_Flash_Driver_Type *drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Erase_Chip);
    
    return drv->Erase_Chip(dev);
}
/**/

/**
@功能: 扇区擦除
@参数: pdev, 操作对象
       addr, Flash地址
@返回: Flash执行状态
@备注: 1、允许addr非边界对齐，函数内部会自动进行对齐处理
       2、允许非边界对齐地址输入，但应尽量使用边界对齐地址，便于程序可读
*/
ffse Flash_Erase_Sector(FW_Flash_Type *dev, u32 addr)
{
    FW_Flash_Driver_Type *drv;
    u32 index, base;
    
    if(addr > Flash_Get_Size(dev))
    {
        LOG_D("地址%x超出范围\r\n", addr);
        return FW_Flash_State_AddrErr;
    }
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Erase_Sector);
    FW_ASSERT(drv->Get_SectorBase);
    
    index = Flash_Get_SectorIndex(dev, addr);
    base = drv->Get_SectorBase(dev, index);
    return drv->Erase_Sector(dev, base);
}
/**/

/**
@功能: 区间扇区擦除
@参数: pdev, 操作对象
       start_addr, 起始地址
       end_addr, 结束地址
@返回: Flash执行状态
@备注: 1、允许addr非边界对齐，函数内部会自动进行对齐处理
       2、允许非边界对齐地址输入，但尽量使用边界对齐地址，便于程序可读
*/
ffse Flash_Erase_Inter(FW_Flash_Type *dev, u32 start_addr, u32 end_addr)
{
    FW_Flash_Driver_Type *drv;
    FW_Flash_State_Enum state;
    u32 ss_index, es_index, base;
    u32 i;
    
    if((start_addr > end_addr) ||
       (start_addr >= Flash_Get_Size(dev)) ||
       (end_addr >= Flash_Get_Size(dev)))
    {
        LOG_D("区间擦除时，地址输入异常\r\n");
        return FW_Flash_State_AddrErr;
    }
    
    ss_index = Flash_Get_SectorIndex(dev, start_addr);
    es_index = Flash_Get_SectorIndex(dev, end_addr);
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Erase_Sector);
    FW_ASSERT(drv->Get_SectorBase);
    
    for(i = ss_index; i < es_index; i++)
    {
        base = drv->Get_SectorBase(dev, i);
        state = drv->Erase_Sector(dev, base);
        if(state != FW_Flash_State_OK)  return state;
    }
    
    return FW_Flash_State_OK;
}
/**/

ffse Flash_Write_Direct(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num)
{
    FW_Flash_Driver_Type *drv;
    
    if(addr > Flash_Get_Size(dev))
    {
        LOG_D("地址%x超出范围\r\n", addr);
        return FW_Flash_State_AddrErr;
    }
    
    addr += Flash_Get_Base(dev);
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Write);
    
    return drv->Write(dev, addr, pdata, num);
}
/**/

/**
@功能: 非安全方式写数据
@参数: pdev, 操作对象
       addr, 写入地址
       pdata, 需要写入数据的指针
       num, 需要写入的数据数量, 单位: byte
@返回: Flash执行状态
@备注: 不检查数据，直接擦除相应扇区，然后写入
*/
ffse Flash_Write_Unsafe(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num)
{
    FW_Flash_Driver_Type *drv;
    FW_Flash_State_Enum state;
    u32 flash_addr, base;
    u32 data_length;
    u8 value;
    
    if(addr >= Flash_Get_Size(dev))
    {
        LOG_D("地址%x超出范围\r\n", addr);
        return FW_Flash_State_AddrErr;
    }
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Erase_Sector);
    
    flash_addr = addr;
    data_length = num;
    
    while(data_length--)
    {
        /* 待写区域任意数据不为0xFF，即需要整块擦除后才能写入 */
        Flash_Read(dev, flash_addr, &value, 1);
        if(value != 0xFF)
        {
            base = Flash_Get_SectorBase(dev, flash_addr);
            state = drv->Erase_Sector(dev, base);
            if(state != FW_Flash_State_OK)  return state;
        }
        else
        {
            flash_addr++;
        }
    }
    
    FW_ASSERT(drv->Write);
    
    return drv->Write(dev, addr, pdata, num);
}
/**/

ffse Flash_Write(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num)
{
    FW_Flash_State_Enum state;
    u32 sector_index;
    u32 sector_offset;
    u32 sector_remain;
    u32 sector_size;
    u32 i;
    u8 *p;
    
    sector_size = Flash_Get_SectorSize(dev, addr);
    
    sector_index = addr / sector_size;                //获取当前地址所在扇区的索引编号
    sector_offset = addr % sector_size;
    sector_remain = sector_size - sector_offset;
    
    if(num <= sector_remain)  sector_remain = num;    //需要写入的数据不足当前扇区剩余的空间
    
    p = (u8 *)pdata;
    
    dev->Buffer = (u8 *)MM_Malloc(sector_size);
    if(dev->Buffer == NULL)
    {
        LOG_D("未申请到足够(%d字节)的内存", sector_size);
        return FW_Flash_State_DataErr;
    }
    
    while(1)
    {
        Flash_Read(dev, sector_index * sector_size, dev->Buffer, sector_size);
        
        for(i = 0; i < sector_remain; i++)
        {
            if(dev->Buffer[sector_offset + i] != 0xFF)  break;
        }
        
        if(i < sector_remain)
        {
            state = Flash_Erase_Sector(dev, sector_index * sector_size);
            if(state != FW_Flash_State_OK)
            {
                MM_Free(dev->Buffer);
                return state;
            }
            
            for(i = 0; i < sector_remain; i++)
            {
                dev->Buffer[sector_offset + i] = p[i];
            }
            
            state = Flash_Write_Direct(dev, sector_index * sector_size, dev->Buffer, sector_size);
            if(state != FW_Flash_State_OK)
            {
                MM_Free(dev->Buffer);
                return state;
            }
        }
        else
        {
            state = Flash_Write_Direct(dev, addr, p, sector_remain);
            if(state != FW_Flash_State_OK)
            {
                MM_Free(dev->Buffer);
                return state;
            }
        }
        
        if(num == sector_remain)  break;
        else
        {
            sector_index++;
            sector_offset = 0;
            p += sector_remain;
            addr += sector_remain;
            num -= sector_remain;
            
            if(num > sector_size)  sector_remain = sector_size;
            else  sector_remain = num;
        }
    }
    
    MM_Free(dev->Buffer);
    
    return FW_Flash_State_OK;
}
/**/

u32  Flash_Read(FW_Flash_Type *dev, u32 addr, void *pdata, u32 num)
{
    FW_Flash_Driver_Type *drv;
    
    if(addr > Flash_Get_Size(dev))
    {
        LOG_D("地址%x超出范围\r\n", addr);
        return 0;
    }
    
    FW_ASSERT(pdata);
    
    if(pdata == NULL)
    {
        LOG_D("读取缓存为空\r\n");
        return FW_Flash_State_ArgErr;
    }
    
    addr += Flash_Get_Base(dev);
    
    drv = FW_Device_GetDriver(dev);
    
    FW_ASSERT(drv->Read);
    
    return drv->Read(dev, addr, pdata, num);
}
/**/

