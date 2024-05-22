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

#ifndef _FW_DRIVER_H_
#define _FW_DRIVER_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "fw_define.h"
#include "fw_config.h"
#include "fw_list.h"


//#define FW_DRIVER_ID_MASK                     (0xFFFFFFFF)
//#define FW_DRIVER_ID(dev_type, drv_num)       ((((dev_type) << 16) | (drv_num)) & FW_DRIVER_ID_MASK)


/**
 * @驱动
 * 1、可通过name唯一确定一个驱动
 * 2、设备可以在使用时再进行定义，驱动则需要在编译前即完成定义；
 * 3、name的构成方式：dev_drv，表示设备dev的drv驱动
 */
typedef struct
{
    char Name[DRIVER_NAME_MAX];                  //驱动名称
    const void *Driver;                          //驱动实例
}FW_Driver_Section_Type;

#define FW_DRIVER_SECTION __USED_ __SECTION_("Driver_Table")

#define FW_DRIVER_REGIST(name, drv, desc)\
    FW_DRIVER_SECTION const FW_Driver_Section_Type __Driver_##desc =\
    {\
        name,\
        drv,\
    }
    
#define FW_DRIVER_DECLARE()  SECTION_DECLARE(Driver_Table, FW_Driver_Section_Type)


void *FW_Driver_Find(const char *name);
char *FW_Driver_GetName(void *drv);


#ifdef __cplusplus
}
#endif

#endif

