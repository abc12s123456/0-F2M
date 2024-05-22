#include "fw_driver.h"


/**
 * @驱动定义
 */
FW_DRIVER_DECLARE();


void *FW_Driver_Find(u32 drv_id)
{
    FW_Driver_Section_Type *pstart, *pend, *pdrv;
    
    pstart = SECTION_GET_START(Driver_Table, FW_Driver_Section_Type);
    pend = SECTION_GET_END(Driver_Table, FW_Driver_Section_Type);
    
    for(pdrv = pstart; pdrv < pend; pdrv++)
    {
        if(pdrv->ID == (drv_id & FW_DRIVER_ID_MASK))
        {
            return (void *)pdrv->Driver;
        }
    }
    
    return NULL;
}
/**/

u32  FW_Driver_GetID(void *drv)
{
    FW_Driver_Section_Type *pstart, *pend, *pdrv;
    
    pstart = SECTION_GET_START(Driver_Table, FW_Driver_Section_Type);
    pend = SECTION_GET_END(Driver_Table, FW_Driver_Section_Type);
    
    for(pdrv = pstart; pdrv < pend; pdrv++)
    {
        if(drv == pdrv->Driver)
        {
            return (pdrv->ID & FW_DRIVER_ID_MASK);
        }
    }
    
    return 0;
}
/**/

