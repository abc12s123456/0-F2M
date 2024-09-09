#include "ll_include.h"

#include "fw_flash.h"


#define CHIP_SIZE                 (248 * 1024)

#define SECTOR_BASE(index)        (0x00000000 + ((index) * SECTOR_SIZE))
#define SECTOR_SIZE               (4 * 1024)
#define SECTOR_NUM                (248 * 1024 / SECTOR_SIZE)


__INLINE_STATIC_ u32 IFlash_Get_SectorNum(FW_Flash_Type *dev)
{
    return SECTOR_NUM;
}

__INLINE_STATIC_ u32 IFlash_Get_SectorBase(FW_Flash_Type *dev, u32 sector_index)
{
    return SECTOR_BASE(sector_index);
}

__INLINE_STATIC_ u32 IFlash_Get_SectorSize(FW_Flash_Type *dev, u32 sector_index)
{
    return SECTOR_SIZE;
}

__INLINE_STATIC_ ffst IFlash_Get_Main(FW_Flash_Type *dev)
{
    ffst space;
    space.Base = 0x00000000;
    space.Size = CHIP_SIZE;
    return space;
}

__INLINE_STATIC_ ffst IFlash_Get_CID(FW_Flash_Type *dev)
{
    ffst space;
    space.Base = 0;
    space.Size = 0;
    return space;
}

__INLINE_STATIC_ Bool IFlash_Get_PE(FW_Flash_Type *dev)
{
    return False;
}

__INLINE_STATIC_ ffse IFlash_Erase_Sector(FW_Flash_Type *dev, u32 addr)
{
    FLASH_Erase(addr);
    return FW_Flash_State_OK;
}

#include "mblock.h"

#define FLASH_GRAN_SIZE      (64 * 4)

__INLINE_STATIC_ u32 IFlash_Write_Gran(u32 addr, const void *pdata)
{
    FLASH_Write(addr, (u32 *)pdata, FLASH_GRAN_SIZE >> 2);
    return FLASH_GRAN_SIZE;
}

__INLINE_STATIC_ u32 IFlash_Read_Gran(u32 addr, void *pdata)
{
    FLASH_Read(addr, (u32 *)pdata, FLASH_GRAN_SIZE >> 2);
    return FLASH_GRAN_SIZE;
}

/* 片上闪存允许半字/字方式写(这里使用字写)，当需要写入的数量为1字节时，需要调整 */
__INLINE_STATIC_ ffse IFlash_Write(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num)
{
    MBlock_Type mb;
    u8 buffer[FLASH_GRAN_SIZE];
    
    mb.Addr = addr;
    mb.Pdata = (void *)pdata;
    mb.Num = num;
    
    mb.Buffer = (u8 *)buffer;
    mb.Gran_Size = sizeof(buffer);
    
    mb.Write_Gran = IFlash_Write_Gran;
    mb.Read_Gran = IFlash_Read_Gran;
    
    if(MBlock_Write_SelfAlign(&mb) == 0)
    {
        return FW_Flash_State_DataErr;
    }
    
    return FW_Flash_State_OK;
}

__INLINE_STATIC_ u32 IFlash_Read(FW_Flash_Type *dev, u32 addr, void *pdata, u32 num)
{
    MBlock_Type mb;
    u8 buffer[FLASH_GRAN_SIZE];
    
    mb.Addr = addr;
    mb.Pdata = (void *)pdata;
    mb.Num = num;
    
    mb.Buffer = (u8 *)buffer;
    mb.Gran_Size = sizeof(buffer);
    
    mb.Write_Gran = IFlash_Write_Gran;
    mb.Read_Gran = IFlash_Read_Gran;
    
    if(MBlock_Read_SelfAlign(&mb) == 0)
    {
        return FW_Flash_State_DataErr;
    }
    
    return FW_Flash_State_OK;
}




/* Internal Flash Driver */
__CONST_STATIC_ FW_Flash_Driver_Type IFlash_Driver =
{
    .Get_SectorNum  = IFlash_Get_SectorNum,
    .Get_SectorBase = IFlash_Get_SectorBase,
    .Get_SectorSize = IFlash_Get_SectorSize,
    
    .Get_Main       = IFlash_Get_Main,
    .Get_CID        = IFlash_Get_CID,
    
    .Get_PE         = IFlash_Get_PE,
    
    .Erase_Sector   = IFlash_Erase_Sector,
    
    .Write          = IFlash_Write,
    .Read           = IFlash_Read,
};
FW_DRIVER_REGIST("ll->flash", &IFlash_Driver, IFlash);




#include "project_debug.h"
#if MODULE_TEST && IFLASH_TEST
#include "fw_system.h"
#include "fw_delay.h"
#include "fw_gpio.h"
#include "fw_usb.h"

#include "mm.h"

#include <string.h>

//#include "ff.h"


#define SIZE       0x200


#define USB_DEV     "usb"
#define USB_DRV_ID  FW_DRIVER_ID(FW_Device_USB, 0)


static FW_USB_Type USB;

static void USB_Config(void *dev)
{
    FW_USB_Driver_Type *drv = FW_Driver_Find("ll->usb");
    FW_Device_SetDriver(&USB, drv);
    USB.EN_Pin = PC1;
    USB.EN_VL = LEVEL_L;
}
FW_DEVICE_STATIC_REGIST(USB_DEV, &USB, USB_Config, USB);


#define STR(a)  (#a)

void Test(void)
{
    u32 temp[SIZE] = {0x00000000, 0x11111111, 0x22222222, 0x33333333, 
                      0x44444444, 0x55555555, 0x66666666, 0x77777777, 
                      0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 
                      0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF};
    u32 msg[SIZE] = {0};
    u32 i;
//    u32 addr = 0x08000000 + 100 * 1024 - 128 - 1;
    u32 addr = 100 * 1024 - 128 - 1;
    
    FW_USB_Type *usb;
//    FATFS FS_Disk;
    
    
    usb = FW_Device_Find(USB_DEV);
    if(usb == NULL)
    {
        while(1);
    }
    
    char *p = STR();
    if(p)
    {
        p = p;
    }
    
    FW_System_Init();
    FW_Delay_Init();
    
    FW_Flash_Init();
    MM_Init(8 * 1024);
    
    FW_GPIO_DeInit(PA0);
    
    FW_USB_Init(usb);
    
//    FW_Flash_WriteDirect(addr, msg, SIZE);
//    
//    FW_Flash_Write(addr, msg, SIZE);
//    FW_Flash_Read(addr, msg, SIZE);
    
//    f_mount(&FS_Disk, "0:", 1);
    
//    FW_Flash_Read(addr, msg, SIZE);
//    
//    i = FW_Flash_Write(addr, temp, SIZE);
//    i = i;
//    
//    memset(msg, 0, SIZE * 4);
//    FW_Flash_Read(addr, msg, SIZE);
    
    while(1);
}


#endif

