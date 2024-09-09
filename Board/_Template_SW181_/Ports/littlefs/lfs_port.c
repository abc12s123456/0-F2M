#include "lfs.h"

#include "fw_flash.h"


static lfs_t lfs;
static lfs_file_t file;


#define DEV_EFLASH_NAME      "eflash"
static  FW_Flash_Type        *EFlash;


/* 该函数会在前期初始化中调用执行，但EFlash的初始化需要在主程序运行后执行 */
static void lfs_port_init(void *pdata)
{
    EFlash = FW_Device_Find(DEV_EFLASH_NAME);
    if(EFlash == NULL)
    {
        /* 存储设备未定义 */
        while(1);
    }
}
//FW_PRE_INIT(lfs_port_init, NULL);


static int lfs_port_read(const struct lfs_config *c, 
                         lfs_block_t block,
                         lfs_off_t off, 
                         void *buffer, 
                         lfs_size_t size)
{
    if(Flash_Read(EFlash, block * c->block_size + off, buffer, size) == 0)
    {
        return LFS_ERR_IO;
    }
    return LFS_ERR_OK;
}

static int lfs_port_prog(const struct lfs_config *c, 
                         lfs_block_t block,
                         lfs_off_t off, 
                         const void *buffer, 
                         lfs_size_t size)
{
    if(Flash_Write_Direct(EFlash, block * c->block_size + off, buffer, size) != FW_Flash_State_OK)
    {
        return LFS_ERR_IO;
    }
    return LFS_ERR_OK;
}

static int lfs_port_erase(const struct lfs_config *c, lfs_block_t block)
{
    if(Flash_Erase_Sector(EFlash, block * c->block_size) != FW_Flash_State_OK)
    {
        return LFS_ERR_IO;
    }
    return LFS_ERR_OK;
}

static int lfs_port_sync(const struct lfs_config *c)
{
    return LFS_ERR_OK;
}



static u8 read_buf[256];
static u8 prog_buf[256];
static u32 lookahead_buf[32] = {0};

const struct lfs_config cfg =
{
    /* 接口函数 */
    .read  = lfs_port_read,
    .prog  = lfs_port_prog,
    .erase = lfs_port_erase,
    .sync  = lfs_port_sync,
    
    /* 物理存储参数 */
    .read_size      = 256,             //允许读取的大小(与prog_size一致)
    .prog_size      = 256,             //允许写入的最小单元(芯片的page)
    .block_size     = 4096,            //允许擦除的最小单元大小(芯片的sector)
    .block_count    = 1024,            //当前文件系统扇区数量(划分的chip_size / block_size)
    .cache_size     = 256,             //LFS需要read cache, prog cache，每个文件也需要一个cacke.
                                       //cache越大性能越好，会减少flash的访问次数。cache必须是block
                                       //read size,prog size的倍数，同时是block size的因数
    .lookahead_size = (1024 / 8),      //lookahead buffer的尺寸。lookahead buffer主要是block alloctor
                                       //在分配块时用到。lookahead size必须是8的倍数
    .block_cycles   = 500,             //文件系统进行垃圾回收时block的擦除次数，推荐值100~1000
                                       //值越大垃圾回收次数越少，性能越好

//    .read_buffer = read_buf,
//    .prog_buffer = prog_buf,
//    .lookahead_buffer = lookahead_buf,
};





#include "fw_debug.h"
#if MODULE_TEST && FS_TEST && LFS_TEST

#include "fw_system.h"
#include "fw_delay.h"
#include "fw_usb.h"
#include "fw_flash.h"
#include "fw_gpio.h"
#include "fw_spi.h"

#include "mm.h"

#include <string.h>


static FW_SPI_Type SSPI0;
static void SSPI0_Config(void *dev)
{
    FW_SPI_Driver_Type *drv = FW_Driver_Find("io->spi");
    FW_Device_SetDriver(&SSPI0, drv);
}
FW_DEVICE_STATIC_REGIST("sspi0", &SSPI0, SSPI0_Config, SSPI0);


#define SNOR_SPI_NAME        "spi0"
//#define SNOR_SPI_NAME        "sspi0"
static void SPI_Pre_Init(void *pdata)
{
    FW_SPI_Type *spi = FW_Device_Find(SNOR_SPI_NAME);
    
    spi->CS_Pin = PA4;
    spi->SCK_Pin = PA5;
    spi->MISO_Pin = PA6;
    spi->MOSI_Pin = PA7;
    
    spi->Baudrate = 15000000;
    spi->First_Bit = FW_SPI_FirstBit_MSB;
    spi->Clock_Polarity = FW_SPI_ClockPolarity_H;
    spi->Clock_Phase = FW_SPI_ClockPhase_Edge2;
}
FW_PRE_INIT(SPI_Pre_Init, NULL);

static  SNOR_Type GD25Q64;
static void GD25Q64_Config(void *dev)
{
    SNOR_Type *snor = dev;
    
    FW_Device_SetParent(snor, FW_Device_Find(SNOR_SPI_NAME));
    FW_Device_SetDriver(snor, FW_Driver_Find("spi->dev"));
    
    snor->WP_Pin = PB0;
}
FW_DEVICE_STATIC_REGIST("gd25q64", &GD25Q64, GD25Q64_Config, GD25Q64);


/*  */
#define FLASH_DRV_NAME       "25q64->flash"
static FW_Flash_Type EXFlash;

static void FW_Flash_Config(void *dev)
{
    SNOR_Driver_Type *drv = FW_Driver_Find(FLASH_DRV_NAME);
    SNOR_Type *snor = FW_Device_Find("gd25q64");
    
    FW_Device_SetParent(dev, snor);
    FW_Device_SetDriver(dev, drv);
}
FW_DEVICE_STATIC_REGIST("eflash", &EXFlash, FW_Flash_Config, EXFlash);



void Test(void)
{
    FW_Flash_Type *flash;
    u16 VCC_EN = PA2;
    
    FW_System_Init();
    FW_Delay_Init();
//    MM_Init(8 * 1024);
    
    FW_GPIO_Init(VCC_EN, FW_GPIO_Mode_Out_PPU, FW_GPIO_Speed_Low);
    FW_GPIO_SET(VCC_EN);
    
    flash = FW_Device_Find("eflash");
    while(!flash);
    Flash_Init(flash);
    
    u32 cid = Flash_Get_CID(flash, &cid, sizeof(cid));
    if(cid)  cid = cid;
    
    int err = lfs_mount(&lfs, &cfg);
    if(err)
    {
        err = lfs_format(&lfs, &cfg);
        if(err)  err = err;
        
        err = lfs_mount(&lfs, &cfg);
        while(err);
    }
    
    u32 count = 0;
    
    lfs_file_open(&lfs, &file, "count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &count, sizeof(count));
    
    count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &count, sizeof(count));
    
    lfs_file_close(&lfs, &file);
    
    lfs_unmount(&lfs);
    
    count = count;
    
    while(1);
}


#endif

