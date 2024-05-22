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
 
#include "fw_device.h"


/* 初始化函数项声明 */
FW_INIT_DECLARE(Pre_Init_Table, FW_Function_Type);
FW_INIT_DECLARE(Device_Init_Table, FW_Function_Type);
FW_INIT_DECLARE(Board_Init_Table, FW_Function_Type);


__WEAK_ void FW_System_Init(void){}
__WEAK_ void FW_Delay_Init(void){}


#if defined(__CC_ARM) || defined(__CLANG_ARM)
int $Sub$$main(void)
{
    /* 系统初始化 */
    extern void FW_System_Init(void);
    FW_System_Init();
    
    /* 系统延时节拍初始化(使用OS时，应当在OS开启后再执行一次初始化) */
    extern void FW_Delay_Init(void);
    FW_Delay_Init();
    
    /* 设备绑定 */
    extern void FW_Device_PreBind(void);
    FW_Device_PreBind();
    
    /* 设备前期初始化 & 驱动实例化 */
    extern void FW_Device_PreInit(void);
    FW_Device_PreInit();
    
    /* 前期初始化函数 */
    FW_INIT_LINK(Pre_Init_Table, FW_Function_Type);
    
    /* 设备初始化 */
    FW_INIT_LINK(Device_Init_Table, FW_Function_Type);
    
    /* Board初始化 */
    FW_INIT_LINK(Board_Init_Table, FW_Function_Type);
    
    /* 跳转至main函数 */
    extern int $Super$$main(void);
    $Super$$main();
    
    return 0;
}
#elif defined(__ICCARM__)
int __low_level_init(void)
{
    return 0;
}
#elif defined(__GNUC__)
int entry(void)
{
    return 0;
}
#endif

