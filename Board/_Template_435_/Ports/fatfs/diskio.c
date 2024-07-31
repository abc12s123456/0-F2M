/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "fw_flash.h"


/* Definitions of physical drive number for each drive */
#define DEV_EFLASH_0         0
#define DEV_EFLASH_1         1

#define FS_SECTOR_SIZE       512       //扇区大小，可以自由定义，太大太小都会导致哪些问题
#define FS_BLOCK_SIZE        8         //BLOCK为物理存储介质的最小擦除单元，这里表示为BLOCK_SIZE个扇区

#define FS_SECTOR_NUM        ((8 * 1024 * 1024) / FS_SECTOR_SIZE)

#define FS_START_SECTOR      0

#define DEV_EFLASH_NAME      "eflash"
static  FW_Flash_Type        *EFlash;


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	if(pdrv == DEV_EFLASH_0)  return RES_OK;
    return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    EFlash = FW_Device_Find(DEV_EFLASH_NAME);
    if(EFlash == NULL)  return RES_PARERR;
    Flash_Init(EFlash);
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    while(count--)
    {
        Flash_Read(EFlash, sector * FS_SECTOR_SIZE, buff, FS_SECTOR_SIZE);
        sector++;
        buff += FS_SECTOR_SIZE;
    }
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    while(count--)
    {
        Flash_Write(EFlash, sector * FS_SECTOR_SIZE, buff, FS_SECTOR_SIZE);
        sector++;
        buff += FS_SECTOR_SIZE;
    }
    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch(cmd)
    {
        case CTRL_SYNC:
            return RES_OK;
        
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = FS_SECTOR_NUM;
            return RES_OK;
        
        case GET_SECTOR_SIZE:
            *(DWORD *)buff = FS_SECTOR_SIZE;
            return RES_OK;
        
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = FS_BLOCK_SIZE;
            return RES_OK;
        
        default:
            return RES_PARERR;
    }
}

DWORD get_fattime(void)
{
    return 0;
}


