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

