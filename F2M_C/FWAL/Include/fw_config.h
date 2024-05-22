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
 
#ifndef _FW_CONFIG_H_
#define _FW_CONFIG_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "project_config.h"


/*
 * @设备名称长度
 */
#ifndef DEVICE_NAME_MAX
#define DEVICE_NAME_MAX           12
#endif

/*
 * @驱动名称长度
 */
#ifndef DRIVER_NAME_MAX
#define DRIVER_NAME_MAX           16
#endif

/*
 * @OPS驱动名称长度
 */
#ifndef DEVOPS_NAME_MAX
#define DEVOPS_NAME_MAX           16
#endif


/*
 * @系统大小端模式，默认小端模式
 */
#ifndef SYSTEM_ENDIAN_MODE
#define SYSTEM_ENDIAN_MODE        ENDIAN_LITTLE
#endif

/*
 * @系统对齐字节，默认4B
 */
#ifndef SYSTEM_ALIGN_SIZE
#define SYSTEM_ALIGN_SIZE         4
#endif

/*
 * @OPS->Control函数入参检测，默认关闭
 */
#ifndef OPS_CTL_ARGS_CHECK_EN
#define OPS_CTL_ARGS_CHECK_EN     OFF
#endif

/*
 * @外部高速晶振使能，默认关闭
 */
#ifndef HSE_EN
#define HSE_EN                    OFF
#endif
 
/*
 * @外部低速晶振使能，默认关闭
 */
#ifndef LSE_EN
#define LSE_EN                    OFF
#endif

/*
 * @RTOS使能，默认关闭
 */
#ifndef RTOS_EN
#define RTOS_EN                   OFF
#endif

/*
 * @Delay时钟节拍回调使能，默认关闭
 */
#ifndef DELAY_CB_EN
#define DELAY_CB_EN               OFF
#endif

/*
 * @可变参数函数的参数个数最大值
 */
#ifndef VA_ARGS_NUM_MAX
#define VA_ARGS_NUM_MAX           5
#endif

/*
 * @GPIO重载功能
 */
#ifndef GPIO_RELOAD_EN
#define GPIO_RELOAD_EN            OFF
#endif

/*
 * @Flash重载功能
 */
#ifndef FLASH_RELOAD_EN
#define FLASH_RELOAD_EN           OFF
#endif

/*
 * @RTC重载功能
 */
#ifndef RTC_RELOAD_EN
#define RTC_RELOAD_EN             OFF
#endif

/*
 * @IWDG重载功能
 */
#ifndef IWDG_RELOAD_EN
#define IWDG_RELOAD_EN            OFF
#endif

/*
 * @打印输出功能，默认使用模拟串口
 */
#ifndef HUART_PRINT_EN
#define HUART_PRINT_EN            OFF
#endif

#if !defined(HUART_PRINT_EN) || (!HUART_PRINT_EN)
#ifndef SUART_PRINT_EN
#define SUART_PRINT_EN            ON
#endif
#endif  /* #if !defined(HUART_PRINT_EN) || (!HUART_PRINT_EN) */


#ifdef __cplusplus
}
#endif

#endif

