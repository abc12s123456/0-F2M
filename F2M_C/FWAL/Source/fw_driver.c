#include "fw_driver.h"


/**
 * @驱动定义
 */
FW_DRIVER_DECLARE();


/**
 * @外设占位驱动
 * 不包含具体的实现，只是表示设备与驱动之间的关联
 * 用于SPI/I2C/UART/1-Wire等总线端口驱动外部设备
 */
#define PH_SPI_DRV           ((void *)(-1))
FW_DRIVER_REGIST("spi->dev", PH_SPI_DRV, SPI_GPD);

#define PH_I2C_DRV           ((void *)(-2))
FW_DRIVER_REGIST("i2c->dev", PH_I2C_DRV, I2C_GPD);

#define PH_UART_DRV          ((void *)(-3))
FW_DRIVER_REGIST("uart->dev", PH_UART_DRV, UART_GPD);

#define PH_OW_DRV            ((void *)(-4))
FW_DRIVER_REGIST("ow->dev", PH_OW_DRV, OW_GPD);


void *FW_Driver_Find(const char *name)
{
    FW_Driver_Section_Type *pstart, *pend, *pdrv;
    
    pstart = SECTION_GET_START(Driver_Table, FW_Driver_Section_Type);
    pend = SECTION_GET_END(Driver_Table, FW_Driver_Section_Type);
    
    for(pdrv = pstart; pdrv < pend; pdrv++)
    {
        if(strncmp(pdrv->Name, name, DRIVER_NAME_MAX) == 0)
        {
            return (void *)pdrv->Driver;
        }
    }
    
    return NULL;
}
/**/

char *FW_Driver_GetName(void *drv)
{
    FW_Driver_Section_Type *pstart, *pend, *pdrv;
    
    pstart = SECTION_GET_START(Driver_Table, FW_Driver_Section_Type);
    pend = SECTION_GET_END(Driver_Table, FW_Driver_Section_Type);
    
    for(pdrv = pstart; pdrv < pend; pdrv++)
    {
        if(drv == pdrv->Driver)
        {
            return pdrv->Name;
        }
    }
    
    return NULL;
}
/**/

