#ifndef _PROJECT_CONFIG_H_
#define _PROJECT_CONFIG_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "project_debug.h"


/* 板级配置 */
#define CHIP_TYPE                 "AT32F435VMT7"                                /*芯片型号*/
#define HSE_FREQ                  8000000                                       /*外部高速晶振频率*/
#define LSE_FREQ                  32768                                         /*外部低速速晶振频率*/

#define HSE_EN                    OFF
#define LSE_EN                    ON


/* 功能配置 */
#define ASSERT_EN                 OFF                                           /* 断言功能 0:关闭, 1:打开 */

#define ITM_LOG_EN                OFF                                           /* ITM调试输出，需要使用SDO */
#define HW_LOG_EN                 OFF                                           /* 硬件设备调试输出 */

#define OPS_CTL_ARGS_CHECK_EN     OFF

#define RTOS_EN                   OFF                                           /* RTOS功能使能 */
#define DELAY_CB_EN               OFF                                           /* 时钟节拍回调函数使能 */

#define FLASH_RELOAD_EN           OFF
#define GPIO_RELOAD_EN            OFF
#define RTC_RELOAD_EN             ON
#define IWDG_RELOAD_EN            ON


/*
工程版本说明：


*/


#ifdef __cplusplus
}
#endif

#endif
