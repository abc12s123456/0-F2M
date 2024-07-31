#ifndef _FW_DEBUG_H_
#define _FW_DEBUG_H_

#ifdef __cplusplus
extern "C"{
#endif


#include "project_debug.h"


#ifndef DEBUG_EN
    #error #error "undefined DEBUG_EN"
#endif


/*
 * @assert
 */
#ifndef ASSERT_EN
#define ASSERT_EN                 OFF
#endif

#if (ASSERT_EN == ON)
#define FW_ASSERT(x)              ((x) ? (void)0U : Assert_Failed((u8 *)__FILE__, __LINE__))
void Assert_Failed(u8 *file, u32 line);
#else
#define FW_ASSERT(x)
#endif


/*
 * @log
 */
#ifndef ITM_LOG_EN
#define ITM_LOG_EN                OFF
#endif

#ifndef HW_LOG_EN
#define HW_LOG_EN                 OFF
#endif


/*
 * @日志级别，DEBUG的优先级最高，表示此时会输出所有LOG信息
 */
#ifndef LOG_LEVEL
#define LOG_LEVEL                 LOG_LEVEL_DEBUG
#endif

#ifndef LOG_COLOR_EN
#define LOG_COLOR_EN              OFF
#endif


void Test(void);


#ifdef __cplusplus
}
#endif

#endif
