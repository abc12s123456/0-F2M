#include "ll_include.h"

#include "fw_flash.h"


/* PY32F002A 20KB 128B 实际的扇区大小为4K，但支持page擦除，所以这里使用page作为基本的擦除单元*/
#define SECTOR_Base(num)  (0x08000000 + (num * 128))
#define SECTOR_SIZE       128
#define SECTOR_NUM        (0x5000 / SECTOR_SIZE)


__INLINE_STATIC_ u32 IFlash_Get_SectorNum(FW_Flash_Type *dev)
{
    return SECTOR_NUM;
}

__INLINE_STATIC_ u32 IFlash_Get_SectorBase(FW_Flash_Type *dev, u32 sector_index)
{
    return SECTOR_Base(sector_index);
}

__INLINE_STATIC_ u32 IFlash_Get_SectorSize(FW_Flash_Type *dev, u32 sector_index)
{
    return SECTOR_SIZE;
}

__INLINE_STATIC_ ffst IFlash_Get_Main(FW_Flash_Type *dev)
{
    ffst space;
    space.Base = 0x08000000;
    space.Size = 0x5000;
    return space;
}

__INLINE_STATIC_ ffst IFlash_Get_CID(FW_Flash_Type *dev)
{
    ffst space;
    space.Base = 0x1FFF0E00;
    space.Size = 16;
    return space;
}

//__INLINE_STATIC_ Bool IFlash_Get_PEState(FW_Flash_Type *dev)
//{
//    return False;
//}

__INLINE_STATIC_ ffse IFlash_Erase_Sector(FW_Flash_Type *dev, u32 addr)
{
//    fmc_state_enum state = FMC_READY;
//    
//    fmc_unlock();
//    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
//    state = fmc_page_erase(addr);
//    fmc_lock();
//    
//    return (state == FMC_READY) ? Flash_State_OK : Flash_State_DataErr;
    
    u32 PAGEError = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;
    
    HAL_FLASH_Unlock();
    
    /*擦写类型FLASH_TYPEERASE_PAGEERASE=Page擦, FLASH_TYPEERASE_SECTORERASE=Sector擦*/
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGEERASE;  
    /*擦写起始地址*/
    EraseInitStruct.PageAddress = addr;
    /*需要擦写的扇区数量*/
    EraseInitStruct.NbPages  = 1 ;
    /*执行sector擦除,PAGEError返回擦写错误的sector,返回0xFFFFFFFF,表示擦写成功*/
    if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return FW_Flash_State_DataErr;
    }
    
    HAL_FLASH_Lock();
    
    return FW_Flash_State_OK;
}

#include "mblock.h"

/* PY32F002A内部Flash最小写入单元为1页，即128字节 */
__INLINE_STATIC_ u32 IFlash_Write_Gran(u32 addr, const void *pdata)
{
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, addr, (u32 *)pdata) != HAL_OK)  return 0;
    return SECTOR_SIZE;
}

__INLINE_STATIC_ u32 IFlash_Read_Gran(u32 addr, void *pdata)
{
    memcpy(pdata, (void *)addr, SECTOR_SIZE);
    return SECTOR_SIZE;
}

__INLINE_STATIC_ ffse IFlash_Write(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num)
{
    MBlock_Type mb;
    u8 buffer[SECTOR_SIZE];
    
    mb.Addr = addr;
    mb.Pdata = (void *)pdata;
    mb.Num = num;
    
    mb.Buffer = (u8 *)buffer;
    mb.Gran_Size = sizeof(buffer);
    
    mb.Write_Gran = IFlash_Write_Gran;
    mb.Read_Gran = IFlash_Read_Gran;
    
    HAL_FLASH_Unlock();
    
    if(MBlock_Write_SelfAlign(&mb) == 0)  
    {
        HAL_FLASH_Lock();
        return FW_Flash_State_DataErr;
    }
    
    HAL_FLASH_Lock();
    
    return FW_Flash_State_OK;
}

/* GD32的片上Flash允许按字节读取 */
__INLINE_STATIC_ u32 IFlash_Read(FW_Flash_Type *dev, u32 addr, void *pdata, u32 num)
{
    u8 *p = (u8 *)pdata;
    
    while(num--)
    {
        *p++ = *(u8 *)addr;
        addr++;
    }
    
    return FW_Flash_State_OK;
}




/* Internal Flash Driver */
#define IFLASH_DRV_ID        FW_DRIVER_ID(FW_Device_Flash, IFLASH_DRV_NUM)

const static FW_Flash_Driver_Type IFlash_Driver =
{
    .Get_SectorNum  = IFlash_Get_SectorNum,
    .Get_SectorBase = IFlash_Get_SectorBase,
    .Get_SectorSize = IFlash_Get_SectorSize,
    
    .Get_Main       = IFlash_Get_Main,
    .Get_CID        = IFlash_Get_CID,
    
//    .Get_PEState    = IFlash_Get_PEState,
    
    .Erase_Sector   = IFlash_Erase_Sector,
    
    .Write          = IFlash_Write,
    .Read           = IFlash_Read,
};
FW_DRIVER_REGIST("ll->flash", &IFlash_Driver, IFlash);




#include "project_config.h"
#if MODULE_TEST && IFLASH_TEST
#include "fw_system.h"
#include "fw_delay.h"
#include "fw_gpio.h"
#include "fw_usb.h"

#include "mm.h"

#include <string.h>


#define SIZE       0x10


void Test(void)
{
    u32 temp[SIZE] = {0x00000000, 0x11111111, 0x22222222, 0x33333333, 
                      0x44444444, 0x55555555, 0x66666666, 0x77777777, 
                      0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 
                      0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF};
    u32 msg[SIZE] = {0};
    u32 i;
    u32 addr = 0x08000000 + 20 * 1024 - 128 - 1;
    
    FW_Flash_Init();
//    MM_Init(8 * 1024);
    
//    f_mount(&FS_Disk, "0:", 1);
//    
    FW_Flash_Read(addr, msg, SIZE * 4);
    
    i = FW_Flash_Write(addr, temp, SIZE * 4);
    i = i;
    
    memset(msg, 0, SIZE * 4);
    FW_Flash_Read(addr, msg, SIZE * 4);
    
    while(1);
}


#endif

