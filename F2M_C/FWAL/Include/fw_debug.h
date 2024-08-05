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
