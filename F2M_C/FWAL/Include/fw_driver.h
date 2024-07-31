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

