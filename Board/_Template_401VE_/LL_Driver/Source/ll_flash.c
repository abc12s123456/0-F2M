//#include "ll_include.h"

//#include "fw_flash.h"


//#define SECTOR_BASE(index)     (0x08000000 + ((index) * 0x800))


//__INLINE_STATIC_ u32 IFlash_Get_SectorNum(FW_Flash_Type *dev)
//{
//    return 128;
//}

//__INLINE_STATIC_ u32 IFlash_Get_SectorBase(FW_Flash_Type *dev, u32 sector_index)
//{
//    return SECTOR_BASE(sector_index);
//}

//__INLINE_STATIC_ u32 IFlash_Get_SectorSize(FW_Flash_Type *dev, u32 sector_index)
//{
//    return 2048;
//}

//__INLINE_STATIC_ ffst IFlash_Get_Main(FW_Flash_Type *dev)
//{
//    ffst space;
//    space.Base = 0x08000000;
//    space.Size = 0x40000;
//    return space;
//}

//__INLINE_STATIC_ ffst IFlash_Get_CID(FW_Flash_Type *dev)
//{
//    ffst space;
//    space.Base = 0x1FFFF7E8;
//    space.Size = 12;
//    return space;
//}

//__INLINE_STATIC_ Bool IFlash_Get_PE(FW_Flash_Type *dev)
//{
//    return (Bool)ob_spc_get();
//}

//__INLINE_STATIC_ ffse IFlash_Erase_Sector(FW_Flash_Type *dev, u32 addr)
//{
//    fmc_state_enum state = FMC_READY;
//    
//    fmc_unlock();
//    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
//    state = fmc_page_erase(addr);
//    fmc_lock();
//    
//    return (state == FMC_READY) ? FW_Flash_State_OK : FW_Flash_State_DataErr;
//}

//#include "mblock.h"
///* GD32F303最小允许半字写，字节读，这里最小读写粒度统一设置为字操作 */
//__INLINE_STATIC_ u32 IFlash_Write_Gran(u32 addr, const void *pdata)
//{
//    if(fmc_word_program(addr, *(u32 *)pdata) != FMC_READY)  return 0;
//    return sizeof(u32);
//}

//__INLINE_STATIC_ u32 IFlash_Read_Gran(u32 addr, void *pdata)
//{
//    *(u32 *)pdata = *(u32 *)addr;
//    return sizeof(u32);
//}

///* 片上闪存允许半字/字方式写(这里使用字写)，当需要写入的数量为1字节时，需要调整 */
//__INLINE_STATIC_ ffse IFlash_Write(FW_Flash_Type *dev, u32 addr, const void *pdata, u32 num)
//{
//    MBlock_Type mb;
//    u32 value;
//    
//    mb.Addr = addr;
//    mb.Pdata = (void *)pdata;
//    mb.Num = num;
//    
//    mb.Buffer = (u8 *)&value;
//    mb.Gran_Size = sizeof(value);
//    
//    mb.Write_Gran = IFlash_Write_Gran;
//    mb.Read_Gran = IFlash_Read_Gran;
//    
//    fmc_unlock();
//    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
//    
//    if(MBlock_Write_SelfAlign(&mb) == 0)  
//    {
//        fmc_lock();
//        return FW_Flash_State_DataErr;
//    }
//    
//    fmc_lock();
//    
//    return FW_Flash_State_OK;
//}

///* GD32的片上Flash允许按字节读取 */
//__INLINE_STATIC_ u32 IFlash_Read(FW_Flash_Type *dev, u32 addr, void *pdata, u32 num)
//{
//    u8 *p = (u8 *)pdata;
//    
//    while(num--)
//    {
//        *p++ = *(u8 *)addr;
//        addr++;
//    }
//    
//    return FW_Flash_State_OK;
//}




/* Internal Flash Driver */
//__CONST_STATIC_ FW_Flash_Driver_Type IFlash_Driver =
//{
//    .Get_SectorNum  = IFlash_Get_SectorNum,
//    .Get_SectorBase = IFlash_Get_SectorBase,
//    .Get_SectorSize = IFlash_Get_SectorSize,
//    
//    .Get_Main       = IFlash_Get_Main,
//    .Get_CID        = IFlash_Get_CID,
//    
//    .Get_PE         = IFlash_Get_PE,
//    
//    .Erase_Sector   = IFlash_Erase_Sector,
//    
//    .Write          = IFlash_Write,
//    .Read           = IFlash_Read,
//};
//FW_DRIVER_REGIST("ll->flash", &IFlash_Driver, IFlash);




//#include "project_debug.h"
//#if MODULE_TEST && IFLASH_TEST
//#include "fw_system.h"
//#include "fw_delay.h"
//#include "fw_gpio.h"
//#include "fw_usb.h"

//#include "mm.h"

//#include <string.h>

////#include "ff.h"


//#define SIZE       0x200


//#define USB_DEV     "usb"
//#define USB_DRV_ID  FW_DRIVER_ID(FW_Device_USB, 0)


//static FW_USB_Type USB;

//static void USB_Config(void *dev)
//{
//    FW_USB_Driver_Type *drv = FW_Driver_Find("ll->usb");
//    FW_Device_SetDriver(&USB, drv);
//    USB.EN_Pin = PC1;
//    USB.EN_VL = LEVEL_L;
//}
//FW_DEVICE_STATIC_REGIST(USB_DEV, &USB, USB_Config, USB);


//#define STR(a)  (#a)

//void Test(void)
//{
//    u32 temp[SIZE] = {0x00000000, 0x11111111, 0x22222222, 0x33333333, 
//                      0x44444444, 0x55555555, 0x66666666, 0x77777777, 
//                      0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 
//                      0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF};
//    u32 msg[SIZE] = {0};
//    u32 i;
////    u32 addr = 0x08000000 + 100 * 1024 - 128 - 1;
//    u32 addr = 100 * 1024 - 128 - 1;
//    
//    FW_USB_Type *usb;
////    FATFS FS_Disk;
//    
//    
//    usb = FW_Device_Find(USB_DEV);
//    if(usb == NULL)
//    {
//        while(1);
//    }
//    
//    char *p = STR();
//    if(p)
//    {
//        p = p;
//    }
//    
//    FW_System_Init();
//    FW_Delay_Init();
//    
//    FW_Flash_Init();
//    MM_Init(8 * 1024);
//    
//    FW_GPIO_DeInit(PA0);
//    
//    FW_USB_Init(usb);
//    
////    FW_Flash_WriteDirect(addr, msg, SIZE);
////    
////    FW_Flash_Write(addr, msg, SIZE);
////    FW_Flash_Read(addr, msg, SIZE);
//    
////    f_mount(&FS_Disk, "0:", 1);
//    
////    FW_Flash_Read(addr, msg, SIZE);
////    
////    i = FW_Flash_Write(addr, temp, SIZE);
////    i = i;
////    
////    memset(msg, 0, SIZE * 4);
////    FW_Flash_Read(addr, msg, SIZE);
//    
//    while(1);
//}


//#endif

